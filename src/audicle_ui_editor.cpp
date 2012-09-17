/*----------------------------------------------------------------------------
    Audicle Context-sensitive, On-the-fly Audio Programming Environment
      for the ChucK Programming Language

    Copyright (c) 2005 Ge Wang, Perry R. Cook, Ananya Misra, Philip Davidson.
      All rights reserved.
      http://audicle.cs.princeton.edu/
      http://chuck.cs.princeton.edu/
      http://soundlab.cs.princeton.edu/

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
    U.S.A.
-----------------------------------------------------------------------------*/

//-----------------------------------------------------------------------------
// name: audicle_ui_editor.cpp
// desc: ...
//
// authors: Philip Davidson (philipd@cs.princeton.edu)
//          Ge Wang (gewang@cs.princeton.edu)
//          Perry R. Cook (prc@cs.princeton.edu)
//          Ananya Misra (amisra@cs.princeton.edu)
// date: Autumn 2004
//-----------------------------------------------------------------------------
#include "audicle_ui_editor.h"

#include "chuck_vm.h"

AudicleFont * bufferFont; 
LineRenderer * bufferDraw;

bool _buffer_font_inited = false;
int buffer_font_index = 0;

void init_Buffer_Font() { 
#ifdef _USE_FTGL_FONTS_
    bufferFont = AudicleFont::loadFont ( "FTGL:LUCON.TTF" );
#else
    bufferFont = AudicleFont::loadFont ( "OpenGL:mono" );
#endif
	bufferDraw = new LineRenderer();
	bufferDraw->font = bufferFont; 
    _buffer_font_inited = true;
}

void switch_Buffer_Font() { 
    
    AudicleFont::releaseFont ( bufferFont );
    buffer_font_index = ( buffer_font_index + 1 ) % AudicleFont::available_fonts().size();
    fprintf ( stderr, "selecting %s , font %d of %ld \n", AudicleFont::available_fonts()[buffer_font_index].c_str(), buffer_font_index, AudicleFont::available_fonts().size() );
    bufferFont = AudicleFont::loadFont ( (char*) AudicleFont::available_fonts()[buffer_font_index].c_str() );

}


/*
    COMMA = 0,
    SEMICOLON,
    DBLCOLON,
    PAREN,
    DOT,
    CHUCK_OP,
    OPERATOR,
    KEYWORD,
    DEBUG_PRINT,
    SPORK,
    INTEGER,
    FLOATING,
    STRING,
    COMMENT,
    OTHER,

    NUM_SYNTAX_TYPES

*/
LineRenderer::LineRenderer() { 
	initColors();
}

void
LineRenderer::setFont( AudicleFont * f ) { font = f; } 

void 
LineRenderer::initColors() { 

	token_colors.resize( NUM_SYNTAX_TYPES, Color4D(0,0,0) );

	token_colors[PAREN]		= Color4D(0,0,1);
	token_colors[CHUCK_OP]	= Color4D(1.0,0.2,0);
	token_colors[KEYWORD]	= Color4D(0.6,0.6,0);
	token_colors[INTEGER]	= Color4D(0.3,0,0);
	token_colors[FLOATING]	= Color4D(0.0,0.3,0);
	token_colors[STRING]	= Color4D(0.0,0.4,0);
	token_colors[COMMENT]	= Color4D(0.8,0.4,0);

}

int
LineRenderer::charAtX( TextLine& line, double x) { 
	return 0;
}

void
LineRenderer::setFontTransform() { 
//	font->scale( _fontScale , _fontAspect );
}


bool
fakeTokenizer( string line, SyntaxTokenList& tkl ) { 

	int walk = 0;
	int last = 0;
	SyntaxToken tt;
	int ln = line.length();
	tkl.list.clear();
//	fprintf(stderr, "refresh\n");
	while ( walk < ln ) { 
		while ( last < ln && line[last] == ' ' ) last++;
		walk = last;
		while ( walk < ln && line[walk] != ' ' ) walk++;
		tt.begin = last;
		tt.end = walk;
		tt.type = walk - last;
		tt.token = line.substr ( tt.begin, tt.end - tt.begin );
		tkl.list.push_back(tt);
//		fprintf(stderr, "+token %d %d (%d) %s \n", tt.begin, tt.end, tt.type, tt.token.c_str());
		last = walk;
	}
	tkl.howmany = tkl.list.size(); 

	/*
	tt.begin = line.length(); 
	tt.end = tt.begin + 4;
	tt.token = KEYWORD;
	tt.token = "YAY!";
	tkl.list.push_back( SyntaxToken );
	*/
	return true;
}

void
LineRenderer::refresh_tokens( TextLine & line ) { 
	
	if ( AudicleWindow::main()->m_syntax_query->parseLine ( line.str(), tokenized ) ) { 
	//if ( fakeTokenizer( line.str() , tokenized ) ) { 
		line.tokens() = tokenized;
	}
	
	line.dirty() = false;
}

void
LineRenderer::draw_tokens( TextLine &line ) { 

	char lc = 0;
	int last = 0;
	std::vector<SyntaxToken>::size_type i;
	SyntaxToken tk;

	if ( line.dirty() ) refresh_tokens( line );
	SyntaxTokenList tklist = line.tokens();
	if ( tklist.howmany == 0 ) { draw(line); return; } 

	for ( i = 0 ; i < tklist.howmany ; i++ ) {
		tk = tklist.list[i];
		if ( last != tk.begin ) 
			lc = font->draw_sub( line.substr(last, tk.begin - last ), lc );
		glColor4dv( token_colors[ tk.type ].data() );
		lc = font->draw_sub( tk.token, lc );
		last = tk.end;
	}

}

void 
LineRenderer::draw( TextLine &line ) { //easy!
	font->draw(line.str());
}




std::vector < TextSpan > * TextContent::_kring = NULL;
bool TextContent::_kring_inited = false;

ck_socket TextContent::_tcp = NULL;
int TextContent::_connected = false; 
char TextContent::_hostname[512] = "127.0.0.1";
int TextContent::_port = 8888;


TextContent::TextContent() :
    WindowContent(),
    _leading(0.06),
    _fontScale(0.04),
    _fontAspect(1.0),
    _fontWeight(1),
    _windowLinePos(0),
    _windowLineSpan(0),
    _magicCharPos(0),
    _title("buffer:"),
    _canvasDirty(true),
    _parsed(NULL),
    _parse_tree(NULL),
    _parse_edit_point(0),
    _viewportDirty(true)
{ 
    EM_log( CK_LOG_FINER, "(audicle) TextContent ctor..." );
    _loc.line = 0;
    _loc.chr = 0;
    _markLoc = _loc;
    _wSpace  = bufferFont->length("m");
    _wTab    = _wSpace * 4;
    _viewport.setScale( 1.1, 1.1 );
    _fontColor = Color4D ( 0,0,0,0.95 );
    _buf = new_buffer();
    
    //should be in CodeRevision
}

#ifdef __PLATFORM_WIN32__
#define STRCAST (string)
#else
#define STRCAST
#endif

void
TextContent::refreshView() { 
    _title = _buf->filename();
    _canvasDirty = true;
    _viewportDirty = true;
    _containerDirty = true;
}



void
TextContent::setBuf(TextBuffer * buf) { 

    if ( _buf ) { 
        _buf = buf ;
         _markSpan.begin = _markSpan.end = _loc = TextLoc(0,0);
         refreshView();
    }
}

void
TextContent::open( const char * filename ) { 
    _buf->open(filename);
    _markSpan.begin = _markSpan.end = _loc = TextLoc(0,0);
    refreshView();

}

void
TextContent::write( const char * filename) { 
    fprintf(stderr, "textcontent-write called with %s" , ( filename==NULL) ? "NULL" : filename  );
    _buf->write(filename);
    refreshView();
}


void
TextContent::connect_tcp( const char * hostname, int port ) { 
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
            fprintf(stderr, "sending %ld to %ld of %s\n", done - msg.length, done, _buf->filename().c_str());
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
  sprintf( mesgbuf , "l: %3d c:%3d", _loc.line + 1 , _loc.chr + 1 );
  _posMesg = mesgbuf;
  //    _posMesg = MAKE_STRING ( std::setw(4) <<  _loc.line << ch << std::setw(4) << _loc.chr ); 
    //_posMesg = (string)"l:" + (string)_loc.line;
    return _posMesg.c_str();
}

void 
TextContent::_fixCanvas() { 
    if ( _canvasDirty ) { 
//        fprintf (stderr, "fixing canvas \n" );
        double w = 0;  //max length of lines

        for ( int i = 0 ; i < _buf->nlines(); i++ )
            w = max ( bufferFont->length( _buf->line(i).str() ) , w ) ;

        w /= bufferFont->height();
        
        _canvas.resize( w * _fontScale * _fontAspect , \
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
        
        double curs = _fontScale * _fontAspect * bufferFont->length( _buf->line(_loc.line).substr( 0, _loc.chr).c_str() ) / bufferFont->height() ;
//        double curs = 0.01 * _fontScale * _fontAspect * _wSpace * _loc.chr; 

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

//    glColor4dv ( _fontColor.data() );
    glColor4d( 0,0,0,0.9);
    glLineWidth (2.0);
        
    if (_parse_tree) { 

        glPushMatrix();
        glScaled( _leading * 1.2 , -_leading, 1.0 ); //scale to match line positions
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
    bool render_pass = ( AudicleWindow::main()->m_render_mode == GL_RENDER );

    //this is the font's particular idea of a line-height
    // not including descenders..FIX LATER

    double font_text_height = bufferFont->height();

	//bufferFont will be phased out for bufferDraw, which will handle
	//more of the details of line rendering

/*
    double m_time = AudicleWindow::main()->get_current_time();    
    Color4D tdark =  Color4D ( 0.6, 0.8, 0.6, 0.5 );
    Color4D tlight =  Color4D ( 0.6, 0.9, 0.8, 0.4 );
    double blink_per_sec = 1.0;
    double glotime = m_time * blink_per_sec; 
    glotime -= floor ( glotime );
    double glomod =  fabs( -1.0 + 2.0 * ( glotime ) );
    Color4D tglow = tlight.interp( tdark , glomod );
*/

    Color4D tglow = Color4D ( 0.6, 0.8, 0.6, 0.4 );


    for  (int i = _windowLinePos ; i < n ; i++ ) { 
        glTranslated( 0, -_leading, 0 );
        
        if ( render_pass ) { 
            //we put a lot inside the render pass, so we may have broken picking. 
            //we shall see...
            glPushMatrix();

//			bufferDraw->setFontTransform(); // font scale 
			bufferFont->scale( _fontScale, _fontAspect );

            if ( _markSpan.begin.line <= i && i <= _markSpan.end.line ) {  
                //highlight span of selection;
                    
                double x1,x2;
                
                if ( i == _markSpan.begin.line ) 
                    x1 = bufferFont->length( _buf->line(i).substr( 0, _markSpan.begin.chr ) ); 
                else 
                    x1 = 0;

                if ( i == _markSpan.end.line ) { 
                    x2 = bufferFont->length( _buf->line(i).substr( 0, _markSpan.end.chr ) ); 
                }
                else 
                    x2 = bufferFont->length( _buf->line(i).str() ) + 0.5 * font_text_height; 

                if ( x1 != x2  ) { 
                    
                    glColor4dv ( tglow.data() );
//                    glColor4d( 0.3, 0.9, 0.9, 0.4 );
                    
                    glBegin(GL_QUADS);
                    glVertex2d( x1 , -0.4 * font_text_height );
                    glVertex2d( x2 , -0.4 * font_text_height );
                    glVertex2d( x2 ,  1.1 * font_text_height );
                    glVertex2d( x1 ,  1.1 * font_text_height );
                    glEnd();
                }
                
            }
            
            
            if (  i == _loc.line && WindowManager::getRenderMode() == WINDOW_RENDER_BLEND ) { 

                //highlight current line

                double xv = _viewport.vpW() * 100 * 40; 
                double xc = bufferFont->length( _buf->line(i).substr( 0, _loc.chr ) );

                glColor4d( 0.8, 0.8, 0.8, 0.2 );

                glBegin(GL_QUADS); 

                 glVertex2d(   -10 ,  -0.1 * font_text_height );
                 glVertex2d( xv + 10 , -0.1 * font_text_height );
                 glVertex2d( xv + 10 ,  1.1 * font_text_height );
                 glVertex2d(   -10 ,   1.1 * font_text_height );

                glEnd();
 
                glColor4d( 1.0 , 0.0, 0.0, 0.9 );
               
                glBegin(GL_LINES);
                                
                 glVertex2d ( xc, -0.1 * font_text_height );
                 glVertex2d ( xc,  1.1 * font_text_height );
                
                glEnd();
                
            }   

            //draw the line
            glColor4d( 0,0,0,1.0);
//			bufferDraw->draw( _buf->line(i) );
			bufferDraw->draw_tokens( _buf->line(i) );
//            bufferFont->draw( _buf->line(i).str() );
            glPopMatrix();
           
        }

    }
    glColor4d( 0,0,0,1.0);

    glPopMatrix();
}

void
TextContent::handleKey(const InputEvent &e) { 
    
    _inp.setEvent(e);
    
    //  These are the key commands that are handled by 
    //  texteditor.
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
        case KEY_CTRL_D:
            switch_Buffer_Font();
            break;
        case KEY_CTRL_V:
            if ( _killring()->size() > 0 ) { 
                paste( _killring()->back().content , _markSpan );
            }
            break;
        case KEY_CTRL_S:
            if ( _inp.shiftDown ) write("");
            else write();            
            break;
        case KEY_CTRL_F:
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
//          if ( !_markSpan.empty() ) _buf->removeSpan ( _markSpan );
            _buf->insertSpan( "\n", _markSpan);
            break;
        default:
//          if ( !_markSpan.empty() ) _buf->removeSpan ( _markSpan );
            _buf->insertSpan ( cs, _markSpan );
            break;
        }
    }

    _markSpan.close();
    _markLoc = _loc = _markSpan.end;

//  adjust_tree ( prev, _loc );

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
    
	// this needs to be split out the linerenderer if we start getting funky with spacing. 
    TextLoc loc;
    loc.line= max( 0, min ( _buf->nlines() - 1, (int)( pt[1] / -_leading ) ) );

    //check...
    double fontx= bufferFont->height() * pt[0] / ( _fontScale * _fontAspect );

    double x = 0;
    double next = 0;
    loc.chr = 0;
    while ( x < fontx && loc.chr < _buf->line(loc.line).size() ) { 
        next = bufferFont->length( _buf->line(loc.line).substr( loc.chr, 1 ) );
        if ( x + 0.5 * next > fontx ) break; 
        else { 
            loc.chr++;
            x += next;   
        }
    }

    loc.chr = min ( loc.chr, _buf->line(loc.line).size() );
    //fprintf ( stderr, "mtoloc - %f %d\n", fontx, loc.chr);
    return loc;

}

bool 
TextContent::checkIDList(t_CKUINT * idlist, int n) 
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
TextContent::setLoc(TextLoc n, bool mark ) { 
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
    /*
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
    */

}


void
TextContent::handleMouse(const InputEvent &e) { 

    _inp.setEvent(e, _viewport.vpToLocal(e.pos));
    
    //_inp.lastPos.printnl();
    if ( e.state == ae_input_DOWN ) { 
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
        case KEY_CTRL_F:
        case KEY_CTRL_O:    //create new window and pass the call down...
            if ( !_wm->top()) { 
                DirScanner dr;
                fileData * f = dr.openFileDialog();
                fileData * nf = f;
                int c = 0;
                while ( nf ) { 
                    EM_log( CK_LOG_INFO, "(audicle) opening file %d : %s", c, nf->fileName.c_str());

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
