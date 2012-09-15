#include "texteditor.h"

std::vector < TextSpan > * TextContent::_kring = NULL;
bool TextContent::_kring_inited = false;

ck_socket TextContent::_tcp = NULL;
int TextContent::_connected = false; 
char TextContent::_hostname[512] = "127.0.0.1";
int TextContent::_port = 8888;


TextContent::TextContent() :
    WindowContent(),
    _leading(0.05),
    _fontScale(0.04),
    _fontAspect(0.8),
    _fontWeight(1),
    _windowLinePos(0),
    _windowLineSpan(0),
    _magicCharPos(0),
    _title("buffer:"),
    _canvasDirty(true),
    _parsed(NULL),
    _parse_tree(NULL),
    _parse_edit_point(NULL),
    _viewportDirty(true)
{ 
    _loc.line = 0;
    _loc.chr = 0;
    _markLoc = _loc;
    _wSpace  = (int)drawString_length(" ");
    _wTab    = _wSpace * 4;
    _viewport.setScale( 1.1, 1.1 );
    _buf = new TextBuffer(); //good!

    //should be in CodeRevision
}

#ifdef __PLATFORM_WIN32__
#define STRCAST (string)
#else
#define STRCAST
#endif

void
TextContent::setBuf(TextBuffer * buf) { 

    if ( _buf ) { 
        _buf = buf ;
        _title = _buf->filename();
        _markSpan.begin = _markSpan.end = _loc = TextLoc(0,0);
        _canvasDirty = true;
        _viewportDirty = true;
        _containerDirty = true;
    }
}
void
TextContent::open( char * filename ) { 
    _buf->open(filename);
    _title = _buf->filename();
    _markSpan.begin = _markSpan.end = _loc = TextLoc(0,0);
    _canvasDirty = true;
    _viewportDirty = true;
    _containerDirty = true;
}

void
TextContent::write( char * filename) { 

    _buf->write(filename);
    _title = _buf->filename();
    _canvasDirty = true;
    _viewportDirty = true;
    _containerDirty = true;
}


void
TextContent::connect_tcp(char * hostname, int port) { 

    if ( !_tcp ) { 
        _tcp = ck_tcp_create ( 0 );
        if ( !_tcp ) 
            fprintf(stderr, "textcontent:: cannot open socket\n");
    }

    if ( _tcp && ( !_connected || hostname || port != _port ) ) { 
        _connected = false;

        if ( hostname ) strncpy ( _hostname, hostname, 512 );
        if ( port != _port ) _port = port;

        if ( !ck_connect ( _tcp, _hostname, _port ) ) { 
            fprintf(stderr, "textcontent :: socket could not connect to %s:%i...\n", _hostname, _port );
        }
        else _connected = true;
    }
}

void 
TextContent::close_tcp() { 
    if ( _tcp ) { 
        ck_close( _tcp );
        _connected = false;
        _tcp = NULL;
    }
}

void
TextContent::sendtcp () { 
    string blob = "";
    for ( int i = 0 ; i < _buf->nlines() ; i++ ) { 
        blob += _buf->line(i).str();
        if ( i != _buf->nlines() - 1 ) blob += "\n";
    }

    connect_tcp();

    if ( _connected ) { 
        //DO THAT TCP MAGIC THING
        
        Net_Msg msg;
        msg.type = MSG_ADD;
        msg.param = 1;
        strcpy ( (char*)msg.buffer, "editor->" );
        strcat ( (char*)msg.buffer, _buf->filename().c_str() );
        msg.param2 = (t_CKUINT) blob.size(); //trailing NUL?
        msg.length = 0;
        otf_hton( &msg );
        ck_send ( _tcp , (char *)&msg, sizeof(msg) );
        
        t_CKUINT left = blob.size() ; //include trailing NUL ?
        t_CKUINT done = 0;
        while ( left ) { 
            msg.length = ( left > NET_BUFFER_SIZE ) ? NET_BUFFER_SIZE : left; 
            msg.param3 = msg.length;
            memcpy( (void*)msg.buffer, (void*) (blob.c_str() + done ), msg.length *  sizeof (char)  );
            left -= msg.length;
            done += msg.length;
            msg.param2 = left;            
            fprintf(stderr, "sending %d to %d of %s\n", done - msg.length, done, _buf->filename().c_str());
            otf_hton( &msg );
            ck_send ( _tcp, (char*)&msg, sizeof(msg) );
        }

        msg.type = MSG_DONE;
        otf_hton( &msg );
        ck_send (_tcp , (char*)&msg, sizeof(msg));

        close_tcp();
    }
    else { 
        fprintf(stderr,"sendtcp::unable to establish connection to ChucK process...\n");
    }
}

const char *
TextContent::posMesg() 
{ 

  char mesgbuf[512];
  sprintf( mesgbuf , "l: %3d c:%3d", _loc.line, _loc.chr );
  _posMesg = mesgbuf;
  //    _posMesg = MAKE_STRING ( std::setw(4) <<  _loc.line << ch << std::setw(4) << _loc.chr ); 
    //_posMesg = (string)"l:" + (string)_loc.line;
    return _posMesg.c_str();
}

void 
TextContent::_fixCanvas() { 
    if ( _canvasDirty ) { 
        
        double w = 0;  //max length of lines
        for ( int i = 0 ; i < _buf->nlines(); i++ )
            w = max ( _buf->line(i).len(), w ) ;
        
        _canvas.resize( w * 0.01 * _fontScale * _fontAspect , \
                        _buf->nlines()*_leading ); //easy!
        
        _canvasDirty = false;
    }
}

std::vector < TextSpan > *
TextContent::_killring() { 
    if ( !_kring_inited ) { 
        _kring = new std::vector < TextSpan > ;
        _kring_inited = true;
    }
    return _kring;
}

void
TextContent::_fixView() { 
    
    if ( _viewportDirty ) { 
        
        double vpw = _viewport.w() / _viewport.sx();
        double vph = _viewport.h() / _viewport.sy();
        
        double hbuf = _leading;
        double vbuf = _leading; 
        
        double curs = 0.01 * _fontScale * _fontAspect * drawString_length( _buf->line(_loc.line).str().substr( 0, _loc.chr).c_str() );
        double lineTop = (_loc.line ) * -_leading;
        double lineBot = (_loc.line + 1) * -_leading;
        
        double docBot =  _buf->nlines() * -_leading;
		
        //fix x
        if ( curs < _viewport.x() + hbuf ) 
            _viewport.moveto( max(0, curs - hbuf) , _viewport.y() );
        else if ( curs > _viewport.x() + vpw - hbuf ) 
            _viewport.moveto( curs - vpw + hbuf , _viewport.y() ); 
        
        //fix y
        if ( lineTop > _viewport.y() - vbuf ) 
            _viewport.moveto( _viewport.x(), min( 0, lineTop + vbuf ) );
        else if ( lineBot < _viewport.y() - vph + vbuf )    
            _viewport.moveto( _viewport.x(), lineBot + vph - vbuf );
        else if ( docBot < -vph && docBot > _viewport.y() - vph + vbuf )
            _viewport.moveto( _viewport.x(), min ( 0, docBot + vph - vbuf ) );
        
		
        _windowLinePos = (int)floor ( _viewport.y() / -_leading );
        
        _viewportDirty = false;
    }
}

void
TextContent::draw() {
    
    _fixCanvas();
    _fixView();
    
    glPushMatrix();
    
    glPushMatrix();
    _viewport.setCols( UI_BASE, Color4D( 1.0,1.0,1.0, 0.8 ), Color4D(0.2, 0.5, 0.2, 0.8 ) );
    _viewport.viewTranslate();
    _viewport.filledQuad();
    glPopMatrix();
    
    _viewport.scale();
    _viewport.viewTranslate();
    
    glColor4d( 0,0,0,0.9);
    glLineWidth (2.0);

        
    if (_parse_tree) { 
        glPushMatrix();
        glScaled( _leading * 1.2 , -_leading, 0 ); //scale to match line positions
        glTranslated ( 1.0 , 0.0 ,0.0 );
        glColor4d ( 1,0,0,0.2 );
        _parse_tree->draw_tree();
        glPopMatrix();

        //glPushMatrix();
        //glTranslated( _leading * 5.0, 0, 0 );
        //glPushMatrix();
        //glScaled( _leading * 0.7 , _leading, 0 ); //scale to match line positions
        //_parse_tree->draw_tree_buffer(_buf);
        //glPopMatrix();
        //glPopMatrix();
    }
	
    glLineWidth ( ( _selected ) ? _fontWeight : _fontWeight );

    glTranslated( 0, _windowLinePos * -_leading, 0 ); //for all the lines we are skipping
    
    int n = min ( _windowLinePos + _windowLineSpan , _buf->nlines() );
    for  (int i = _windowLinePos ; i < n ; i++ ) { 
        glTranslated( 0, -_leading, 0 );
        
        glPushMatrix();
        scaleFont(_fontScale , _fontAspect );
        
        //
        if ( RenderMode == GL_RENDER &&  _markSpan.begin.line <= i && i <= _markSpan.end.line ) {  
            
            double x1,x2;
            
            x1 = 0;
            if ( i == _markSpan.begin.line ) 
                x1 = drawString_length(_buf->line(i).str().substr(0, _markSpan.begin.chr).c_str());
            
            
            if ( i == _markSpan.end.line )
                x2 = drawString_length(_buf->line(i).str().substr(0, _markSpan.end.chr).c_str());
            else 
                x2 = drawString_length(_buf->line(i).str().c_str());
            
            glColor4d( 0.0, 0.8, 0.8, 0.6 );
            glBegin(GL_QUADS);
            glVertex2d( x1-10 ,    -10 );
            glVertex2d( x2 + 10 , -10 );
            glVertex2d( x2 + 10 , 110 );
            glVertex2d( x1-10 ,    110 );
            glEnd();
        }
        
        if ( RenderMode == GL_RENDER &&  i == _loc.line ) { 
            
            //highlight span;
                    
            //highlight current line
            glColor4d( 0.8, 0.8, 0.0, 0.2 );
            double x = _canvas.w() * 100 * 200; //drawString_length(_lines[i].cstr());
            glBegin(GL_QUADS); 
            glVertex2d( -10 ,    -10 );
            glVertex2d( x + 10 , -10 );
            glVertex2d( x + 10 , 110 );
            glVertex2d( -10 ,    110 );
            glEnd();
            
            //draw cursor
            x = drawString_length(_buf->line(i).str().substr(0, _loc.chr).c_str());
            double x2 = drawString_length(_buf->line(i).str().substr(0, _loc.chr).c_str());
            glBegin(GL_LINES);
            
            glColor4d( 1.0 , 0.0, 0.0, 0.9 );
            
            glVertex2d ( x, -10 );
            glVertex2d ( x, 110 );
            glVertex2d ( x, -10 );
            glVertex2d ( x2, -10);
            
            glEnd();
            
        }   
        //draw the line
        glColor4d( 0,0,0,0.9);
        _buf->line(i).draw();
        glPopMatrix();
    }
    glPopMatrix();
}

void
TextContent::handleKey(const InputEvent &e) { 
    
    _inp.setEvent(e);
    
    //	These are the key commands that are handled by 
    //	texteditor.
    if ( _inp.ctrlDown ) { 
//        fprintf ( stderr, "ctrl - %d\n", _inp.lastKey );
        switch ( _inp.lastKey ) { 
        case KEY_CTRL_Q:
            break;
        case KEY_CTRL_E:
            _buf->printEdits();
            break;
        case KEY_CTRL_P:
            fprintf(stderr, "parsing...\n");
            parse();
            break;
        case KEY_CTRL_M:
            fprintf(stderr, "send via TCP...\n");
            sendtcp();
            break;
        case KEY_CTRL_A:
            //selectall();
            break;
        case KEY_CTRL_Z:
            _buf->undo(_markSpan.end);
            break;
        case KEY_CTRL_Y:
            _buf->redo(_markSpan.end );
            break;
        case KEY_CTRL_X:
            cut( _markSpan );
            break;
        case KEY_CTRL_C:
            copy( _markSpan );
            break;
        case KEY_CTRL_V:
            if ( _killring()->size() > 0 ) { 
                paste( _killring()->back().content , _markSpan );
            }
            break;
        case KEY_CTRL_S:
            if ( _inp.shiftDown ) write(NULL);
            else write();            
            break;
        case KEY_CTRL_O:
            open(NULL);
            break;
        case KEY_CTRL_B:
            break;
        }
    }
    else { //character input
        string cs;
        cs = _inp.lastKey;
        switch ( _inp.lastKey ) { 
        case SYS_BACKSPACE:
            if ( !_markSpan.empty() ) 
                _buf->removeSpan ( _markSpan );
            else _buf->removeChar ( _markSpan.end );      
            break;
        case SYS_RETURN:
//			if ( !_markSpan.empty() ) _buf->removeSpan ( _markSpan );
            _buf->insertSpan( "\n", _markSpan);
            break;
        default:
//			if ( !_markSpan.empty() ) _buf->removeSpan ( _markSpan );
            _buf->insertSpan ( cs, _markSpan );
            break;
        }
    }

    _markSpan.close();
    _markLoc = _loc = _markSpan.end;

//	adjust_tree ( prev, _loc );

    _viewportDirty = true;
    _canvasDirty = true;
    _magicCharPos = _loc.chr;
    
}

void
TextContent::handleButton( int buttonID ) { 
    switch ( buttonID ) { 
    case KEY_CTRL_P:
        parse();
        break;
    default:
        break;
    }
}

void
TextContent::paste(string s, TextSpan &span) { 
    if ( !span.empty() ) _buf->removeSpan(span);
    _buf->insertSpan( s, span );
    _buf->closeEdit();
}


void
TextContent::cut(TextSpan &span) { 
    _buf->fillSpan(span);
    _killring()->push_back( span );
    _buf->removeSpan( span );
    _buf->closeEdit();
}

void
TextContent::copy(TextSpan &span) { 
    _buf->fillSpan(span);
    _killring()->push_back( span );
}


void
TextContent::handleSpec( const InputEvent &e) { 
    _inp.setEvent(e);
   
    switch(_inp.lastSpec) { 

    case KEY_UPARROW:
        moveToLine(_loc.line-1);
        break;
    case KEY_DOWNARROW:
        moveToLine(_loc.line+1);
        break;
    case KEY_LEFTARROW:
        moveToChar(_loc.chr-1 );
        break;
    case KEY_RIGHTARROW:
        moveToChar(_loc.chr+1 );
        break;
    case KEY_F2:
        parse();
    
    }

    if ( !_inp.shiftDown ) _markLoc  = _loc;
    _markSpan = TextSpan(_loc, _markLoc );
    _buf->closeEdit();
}

TextLoc
TextContent::mouseToLoc( Point2D pt ) { 
    
    TextLoc loc;
    loc.line= max( 0, min ( _buf->nlines() - 1, (int)( pt[1] / -_leading ) ) );
    double fontx= pt[0] / ( 0.01 * _fontScale * _fontAspect );
    //this is cheaper with monospace
    //loc.chr =0;
    //    double x=0;
    //while ( x < fontx && loc.chr < _buf->line(loc.line).size() ) { 
    //    x += drawString_length(_buf->line(loc.line).str().substr(loc.chr,1).c_str());
    //    loc.chr++;
    //}

    double charwidth = drawString_length("m");
    loc.chr = max ( 0, (int)( 0.5 + fontx / charwidth ) );
    loc.chr = min ( loc.chr, _buf->line(loc.line).size() );
    //fprintf ( stderr, "mtoloc - %f %d\n", fontx, loc.chr);
    return loc;

}

bool 
TextContent::checkIDList(uint * idlist, int n) 
{ 
  //different dynamics
  _selected = false;
  for ( int i = 0 ; i < n ; i++ ) { 
    if ( idlist[i] == _id ) { 
      //match lower on stack, but it's not our match...
      //are we selected or not?
      _selected = true;
      if ( i !=  n-1) {
		_parse_tree->checkIDList( idlist+(i+1), n - (i+1) );
	//check subelements
      }
      else _selected = true;
    }
  }
  return _selected;
}

void
TextContent::setLoc(TextLoc &n, bool mark ) { 
    moveToLine(n.line);
    moveToChar(n.chr );
    if ( mark ) {
        _markSpan.end = _loc;
        _markSpan.close();
        _markLoc = _markSpan.end;
    }
}
void 
TextContent::setX(double x) { 
    _viewport.moveto( max ( 0, min( _canvas.right() - _viewport.vpW() , x ) )  ,_viewport.y());
}

void 
TextContent::setY(double y) { 
    _viewport.moveto( _viewport.x() , min ( 0, max( _canvas.bottom() + _viewport.vpH() , y ) )  );
    _windowLinePos = (int)floor ( _viewport.y() / -_leading );
}


void
TextContent::moveToLine( int nLine ) { 
    
    if ( nLine < 0 ) _loc.line = 0;
    else if ( nLine >= _buf->nlines() ) _loc.line = _buf->nlines() - 1;
    else { 
        _loc.line = nLine;
        _loc.chr = _magicCharPos;
        if( _loc.chr > _buf->line(_loc.line).size() ) 
            _loc.chr = _buf->line(_loc.line).size();
    }
    _viewportDirty = true;
}

void
TextContent::moveToChar( int nChar ) { 
    if ( nChar < 0 ) { 
        if ( _loc.line > 0 ) { 
            _loc.line--;
            _loc.chr = _buf->line(_loc.line).size();
        }
    }
    else if ( nChar > _buf->line(_loc.line).size() ) { 
        if ( _loc.line < _buf->nlines() -1 ) { 
            _loc.line++;
            _loc.chr = 0;
        }
    }
    else { 
        _loc.chr = nChar;
        _magicCharPos = _loc.chr;
    }
    
    _viewportDirty = true;
    
}

void
TextContent::parse() 
{ 
    _parsed = parse_buffer( _buf ); 
    if ( _parsed ) { 
        if ( _parse_tree ) { 
            delete _parse_tree;
            _parse_tree = NULL;
        }
        _parse_tree = new tree_a_File_(_parsed, 1, _buf->nlines() );
        int f = _parse_tree->findfirst();
        _parse_tree->firstline = 1;
        fprintf(stderr, "first is %d\n", f);
    }
}


void
TextContent::handleMouse(const InputEvent &e) { 

    _inp.setEvent(e, _viewport.vpToLocal(e.pos));
    
    //_inp.lastPos.printnl();
    if ( e.state == input_DOWN ) { 
        _selected = e.checkID(_id);
    }

    //check the parse tree?
    if ( _selected ) { 

        if ( _inp.isDepressed ) { 
            _loc = mouseToLoc( _inp.lastPos );
            _magicCharPos = _loc.chr;
            _markLoc = _loc;
        }

        _markSpan = TextSpan(_loc, _markLoc );
    
        _buf->closeEdit();

    }
}

void
TextContent::handleMotion(const InputEvent &e) { 

    _inp.setEvent( e, _viewport.vpToLocal(e.pos) );

    if ( _inp.isDepressed ) { 
        _loc = mouseToLoc( _inp.lastPos );
        _magicCharPos = _loc.chr;
        _buf->closeEdit();
    }

    _markSpan = TextSpan(_loc, _markLoc );
}


void
TextContent::parse_adjust_edits(TextEdit e) { 
    if ( !_parse_tree ) return;
    while ( _parse_edit_point < _buf->nEdits() - 1  ) { 
        ++_parse_edit_point;
    }
}


bool
TextWindowHandler::handleKey(const InputEvent &e) { 

    DisplayWindow * d;

    e.fprint( stderr );
    InputState * wimp = _wm->wimp();
	wimp->setEvent(e);

    bool ret = false; 
    if (wimp->ctrlDown ) {

	    bool ret = true;
        switch ( wimp->lastKey ) { 
        case KEY_CTRL_N:
            fprintf(stderr,"Make new window\n");
            d = new DisplayWindow();
            d->setContent(new TextContent());
            _wm->addWindow(d);
            d->moveto(wimp->lastPos[0], wimp->lastPos[1] );
            break;
        case KEY_CTRL_O:	//create new window and pass the call down...
            if ( !_wm->top()) { 
                DirScanner dr;
                fileData * f = dr.openFileDialog();
                fileData * nf = f;
                int c = 0;
                while ( nf ) { 
                    fprintf(stderr, "opening file %d ... %s\n", c, nf->fileName.c_str());

                    d = new DisplayWindow();

                    TextContent * ntext = new TextContent();
                    ntext->open((char*)nf->fileName.c_str());
                    d->setContent(ntext);

                    Point2D np = wimp->lastPos + Point2D(0.03, -0.03 ) * (double)c; 
                    d->moveto(np[0], np[1] );
                    _wm->addWindow(d);
                    _wm->setTopWindow(d);

                    c++;
                    nf = nf->next;
                }

            }
            break;
        default:
            ret = false;
        }
    }

    return ret;

}

bool
TextWindowHandler::handleSpec(const InputEvent &e) { 
    return false;
}
