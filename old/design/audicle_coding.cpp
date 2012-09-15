#include "audicle_coding.h"


ck_socket CodeRevision::_tcp = NULL;
int CodeRevision::_connected = false;
char CodeRevision::_hostname[512] = "127.0.0.1";
int CodeRevision::_port = 8888;

void
CodeRevision::draw() { 
	//assume we are a scale of 1
	glPushMatrix();
	glPushName(_id);

		UIButton rev;

		switch ( _status ) { 
            case rev_PARSED		: rev.setCols( UI_BASE, Color4D( 1.0,0.85,0.85,1.0 ), Color4D(0,0,0,1.0)); break;
            case rev_TYPED		: rev.setCols( UI_BASE, Color4D( 1.0,0.92,0.85,1.0 ), Color4D(0,0,0,1.0));  break;
            case rev_COMPILED	: rev.setCols( UI_BASE, Color4D( 1.0,1.0,0.85,1.0 ), Color4D(0,0,0,1.0));  break;
            case rev_READY		: 
            case rev_RUNNING	: rev.setCols( UI_BASE, Color4D( 0.85,1.0,0.85,1.0 ), Color4D(0,0,0,1.0));  break;
			case rev_EDITED		: rev.setCols( UI_BASE, Color4D( 0.9,0.9,1.0,1.0 ), Color4D(0,0,0,1.0));  break;
            default				: rev.setCols( UI_BASE, Color4D( 1.0,1.0,1.0,1.0 ), Color4D(0,0,0,1.0));  break;
        }

        rev.seth( 1.0 );
        rev.setLabel( (char*)_buffer->filename().c_str() );
        rev.fitLabel();
        rev.draw(0.75);

        glTranslated ( 0, -1.0 , 0 );
        rev.seth ( ( size() - 1 )  );
        glColor4d(0.8, 1.0, 0.9, 0.3);
        glBegin(GL_POLYGON);
        rev.roundVerts();
        glEnd();

        glTranslated ( 1.0, 0 , 0 );
        for ( int i=0; i < _shreds.size() ; i++ ) { 
			_shreds[i].draw();
			glTranslated ( 0, -1.0 , 0 );
        }
        glTranslated ( -1.0, 0, 0);

		glPopName();
		glPopMatrix();
}



void
CodeRevision::connect_tcp(char * hostname, int port) { 

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
CodeRevision::close_tcp() { 
    if ( _tcp ) { 
        ck_close( _tcp );
        _connected = false;
        _tcp = NULL;
    }
}


CodeRevision::CodeRevision() :
    _buffer(NULL),
    _bytecode(NULL),
    _parsed_program(NULL),
    _parse_tree(NULL),
    _original(NULL)
    { 
        _buffer = new TextBuffer();
        _id = IDManager::instance()->getPickID();
        setVersion();
        _status = rev_NEW;
    }

    
CodeRevision::CodeRevision(TextBuffer * b) :
    _buffer(NULL),
    _bytecode(NULL),
    _parsed_program(NULL),
    _parse_tree(NULL),
    _original(NULL)
{ 
       _buffer= new TextBuffer();
       if ( b ) { 
            *_buffer = *b; //copy buffer to new revision 
       }
       _id = IDManager::instance()->getPickID();
       setVersion();
       _status = rev_NEW;
}

CodeRevision::CodeRevision(CodeRevision * c) :
    _buffer(NULL),
    _bytecode(NULL),
    _parsed_program(NULL),
    _parse_tree(NULL),
    _original(NULL)
{ 
    *this = *c;
    _buffer = new TextBuffer();
    *_buffer = *(c->_buffer);
    _original = new TextBuffer();
    *_original = *(c->_original);
    _id = IDManager::instance()->getPickID();
    _shreds.clear();
    _status = rev_EDITED;
}

CodeRevision * 
CodeRevision::dup() { 
    return new CodeRevision ( _buffer );
}

CodeRevision * 
CodeRevision::splitModified() {
    if ( _buffer->modified() && _status >= rev_COMPILED ) {         fprintf(stderr, "splitting modified branch\n");
        CodeRevision * branch = new CodeRevision( this );
        *_buffer = *_original; //repair our copy
        return branch;
    }
    else return NULL;
}

void
CodeRevision::open(char *c) { 
    if ( ! _buffer ) _buffer = new TextBuffer();
    _buffer->open(c);
    _status = rev_OPENED;
}

void
CodeRevision::setVersion() {
    _buffer->setVersion();
    if ( !_original ) _original = new TextBuffer();
    *_original = *_buffer;
    _original->setVersion();
}

void
ShredInstance::draw() { 

	UIButton shredbutton;
	
	char name[32];
	shredbutton.setCols(UI_BASE, Color4D(0.8,1.0,0.8,1.0), Color4D(0,0,0,1.0) );
	shredbutton.seth(1.0);
    sprintf(name, "%d", procID);
	shredbutton.setLabel( (char*)name );
    shredbutton.fitLabel();
    
	glPushName(pickID);
	shredbutton.draw(0.75);
    glPopName();
}

void
ShredInstance::handleMouse( const InputEvent & e ) { 
    DragManager * dm = DragManager::instance();
    selected = false;
    if ( e.checkID(pickID) ) { 
        if ( e.state == input_DOWN ) selected = true;
        dragAction( dm );
    }
    
}

void
ShredInstance::dragAction( DragManager * d) { 

    draggable_action dg = d->mode();
    if ( dg == drag_is_Picking ) { 
        d->setobject ((void*)this, drag_ShredInstance);
    }
    else if ( dg == drag_is_Dropping ) { 
        switch ( d->type() ) { 
        case drag_CodeRevision:
            fprintf(stderr, "drag shred to revision to replace...\n");
            break;
        case drag_ShredInstance:
            if ( this == (ShredInstance*)d->object() ) {
                //remove yourself...
                code->removetcp( code->findShred ( procID ) );
                d->setmode ( drag_is_Empty );
            }
            break;
        default:
            fprintf(stderr, "drag not handled\n");
            break;
        }
        
    }

}
void
CodeRevision::handleMouse(const InputEvent & e) { 

    _selected = e.checkID(_id);
    if ( _selected ) { 
        //match lower on stack, but it's not our match...
        //are we selected or not?
        _selected = true;

        InputEvent sub = e;
        sub.popStack();
        for ( int j =0 ; j < _shreds.size(); j++ ) { 
            _shreds[j].handleMouse( sub );
            if ( _shreds[j].selected ) {
                _selected = false;
            }
            
        }
        dragAction( DragManager::instance() );
    }

}

void
CodeRevision::dragAction( DragManager * d) { 
    draggable_action dg = d->mode();
    if ( dg == drag_is_Picking ) { 
        d->setobject ((void*)this, drag_CodeRevision);
    }
    else if ( dg == drag_is_Dropping ) { 
        ShredInstance * shrd;
        switch ( d->type() ) { 
        case drag_ShredInstance:
            shrd =( ShredInstance *) d->object();
            if ( shrd->code != this ) { 
                spork ( shrd->procID );
                shrd->code->removeShred( shrd->procID );
                d->setmode(drag_is_Empty );
            } else { 
                fprintf ( stderr, " ");
            }
            break;
        case drag_CodeRevision:
            d->setmode( drag_is_Empty );
            d->setobject( NULL, drag_None );
            break;
        default:
            fprintf( stderr, "drag not handled\n" );
            break;
        }
        
    }

 }

int 
CodeRevision::findShred ( int procid ) { 
    for ( int i = 0 ; i < _shreds.size() ; i++ ) 
        if ( procid == _shreds[i].procID ) return i;
    return -1;
}

void
CodeRevision::removeShred( int procid ) { 
    int ind = findShred ( procid );
    if ( ind >= 0 ) { 
        _shreds.erase (_shreds.begin() + ind );
    }
}

int
CodeRevision::parse() { 

    if ( _status >= rev_PARSED ) return 1; //already parsed

    _parsed_program = parse_buffer( _buffer );
    if ( _parsed_program ) { 
      //        if ( _parse_tree ) { 
      //      delete _parse_tree;
      //      _parse_tree = NULL;
      //  }
      //  _parse_tree = new tree_a_File_(_parsed_program, 1, _buffer->nlines() );
      //  int f = _parse_tree->findfirst();
      //  _parse_tree->firstline = 1;
      //
        _status = rev_PARSED;
        fprintf (stderr, "rev-parsed\n");

        return 1;
    }
    else 
        return 0;
}

int 
CodeRevision::typecheck() { 
    if ( _status >= rev_TYPED ) return 1;
    if ( !parse() ) return 0;
    //type checkery
    //our tree gets neater by the moment
    _status = rev_TYPED;
    fprintf (stderr, "rev- typed\n");
    return 1;
}

int
CodeRevision::compile() { 
    if ( _status >= rev_COMPILED ) return 1;
    if ( !typecheck() ) return 0;
    //compilating
    //now have some bytecode to point to...

    _status = rev_COMPILED;
    fprintf (stderr, "rev- compiled\n");
    setVersion();
    return 1;
}

int 
CodeRevision::spork( int replace) { 
    static int shredid = 1;
    if ( !compile() ) return 0;

    //pretend to receive results from the TCP server

    ShredInstance nshred;

    if ( nshred.procID = sendtcp( replace ) ) {
        nshred.procID = ( replace ) ? replace : shredid++; //TEMP
        nshred.status = shred_RUNNING;
        nshred.code = this;
        _shreds.push_back( nshred );
        _status = rev_RUNNING;
        return nshred.procID;
    }
    else return 0;

}

int
CodeRevision::removetcp(int i) { 
	fprintf(stderr, "removing shred %d\n", _shreds[i].procID );
	connect_tcp();
	if ( _connected ) { 
		Net_Msg msg;
		msg.type = MSG_REMOVE;
//		msg.param = _shreds[i].procID;
		msg.param = 0xffffffff;		
		otf_hton( &msg);
		ck_send( _tcp , (char*)&msg, sizeof(msg) );

		msg.type = MSG_DONE;
		otf_hton( &msg);
		ck_send( _tcp , (char*)&msg, sizeof(msg) );

		//        Net_Msg ret;
//        if ( ck_recv( _tcp , (char *)&ret, sizeof(ret) ) ) { 
//            fprintf( stderr, "[chuck(remote)]: operation %s\n", ( ret.param ? "successful" : "failed (sorry)" ) );
			_shreds.erase( _shreds.begin() + i );
//        }

	}
	close_tcp();
	return 1;
}


int
CodeRevision::sendtcp( int replace ) { 

    string blob = "";
    for ( int i = 0 ; i < _buffer->nlines() ; i++ ) { 
        blob += _buffer->line(i).str();
        if ( i != _buffer->nlines() - 1 ) blob += "\n";
    }

    connect_tcp();

    if ( _connected ) { 

        //DO THAT TCP MAGIC THING
        
        Net_Msg msg;
        if ( replace == 0 ) { 
            msg.type = MSG_ADD;
            msg.param = 1;
        }
        else { 
            msg.type = MSG_REPLACE;
            msg.param = replace;;
        }

        strcpy ( (char*)msg.buffer, "editor->" );
        strcat ( (char*)msg.buffer, _buffer->filename().c_str() );
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
            fprintf(stderr, "sending %d to %d of %s\n", done - msg.length, done, _buffer->filename().c_str());
            otf_hton( &msg );
            ck_send ( _tcp, (char*)&msg, sizeof(msg) );
        }

        msg.type = MSG_DONE;
        otf_hton( &msg );
        ck_send (_tcp , (char*)&msg, sizeof(msg));

        Net_Msg ret;
        if ( ck_recv( _tcp , (char *)&ret, sizeof(ret) ) ) { 
            fprintf( stderr, "[chuck(remote)]: operation %s\n", ( ret.param ? "successful" : "failed (sorry)" ) );
        }

        close_tcp();
        return ( ret.param ) ? 1 : 0 ;
    }
    else { 
        fprintf(stderr,"sendtcp::unable to establish connection to ChucK process...\n");
    }
    return 0;
}



CodeWindow::CodeWindow() :
    DisplayWindow(), 
    _current(NULL),
    _textcontent(new TextContent())
    { 
        initShapes();
        //a new window
        _content = _textcontent;
        setRevision(new CodeRevision());
    }

CodeWindow::CodeWindow(CodeRevision * r ) :
    DisplayWindow(), 
    _current(NULL),
    _textcontent(new TextContent())
    {
        initShapes();
        //need a TextContent constructor with buf arguments
        _content = _textcontent;

        //create for existing revision
        setRevision(r);
    }

void
CodeWindow::handleKey(const InputEvent & e) { 
//CodeWindow::handleKey(char c, Point2D pt) { 

    _inp.setEvent ( e, _baseWindow.toLocal(e.pos) );

    bool handled = false;

    if ( _inp.ctrlDown ) { 

        handled = true;
        switch ( _inp.lastKey ) { 
            case KEY_CTRL_W:
                close();
                break;
            case KEY_CTRL_H:
                _current->parse();
                break;
            case KEY_CTRL_J:
                _current->typecheck();
                break;
            case KEY_CTRL_K:
                _current->compile();
                break;
            case KEY_CTRL_L:
                _current->spork( 0 );
                break;
            case KEY_CTRL_B:
                addRevision ( new CodeRevision(_current), true );
                break;
            default:
                handled = false;
        }

    }
    //send unhandled keys down to DisplayWindow 
    //which may take the key, or pass further down 
    //to the textcontent.  
    if ( !handled ) DisplayWindow::handleKey( e );
    
    //opening a file is not properly detected as a modification...
    //we should check for this.
 
    //    fprintf(stderr, " status of revision : %d mod: %d\n", _current->status(), _current->getBuffer()->modified() );
    //test if we made an illegal modification

    CodeRevision * branch = _current->splitModified();
    if ( branch )  { //if so, branch it, add, and set as current 
        TextLoc n  = _textcontent->loc();
        addRevision ( branch, true );
        _textcontent->setLoc( n, true );
    }
    _current->checkModified();
} 

void
CodeWindow::handleMotion( const InputEvent &e) { 
//CodeWindow::handleMotion( Point2D pt ) { 
    DisplayWindow::handleMotion(e);
} 

void
CodeWindow::handleMouse( const InputEvent &e ) { 
//CodeWindow::handleMouse( int button, int state, Point2D pt, uint * pick_stack, uint pick_size ) { 
    
    DisplayWindow::handleMouse( e );
        //calls the handleMouseUI function.  
    	
    if ( _selected  ) { 
        if ( e.state == input_UP ) { 
		    if ( _testButton.selected() ) 
			    handleButton( _testButton.getCode() );

		    if ( _compileButton.selected() ) 
			    handleButton( _compileButton.getCode() );

		    if ( _sporkButton.selected() ) 
			    handleButton( _sporkButton.getCode() );
        }
    }
    
} 


void
CodeWindow::handleSpec( const InputEvent & e ) { 
//CodeWindow::handleSpec(int c, Point2D pt) { 
    DisplayWindow::handleSpec(e);
} 


void
CodeWindow::handleMotionUI () { 

    DisplayWindow::handleMotionUI();

   	_testButton.checkHover      ( _inp.lastPos );
    _compileButton.checkHover   ( _inp.lastPos );
    _sporkButton.checkHover     ( _inp.lastPos );
}

void
CodeWindow::handleMouseUI( const InputEvent & e ) { 
    //CodeWindow::handleMouseUI(int button, int state, Point2D pt, uint * stack, uint size ) { 
    //point transformed to window
    //substack

    DisplayWindow::handleMouseUI( e );

    _testButton.handleMouse     ( e );
    _compileButton.handleMouse  ( e );
    _sporkButton.handleMouse    ( e );

    for ( int j = 0 ; j < _revisions.size(); j++ ) { 
        _revisions[j]->handleMouse( e );
        if ( _revisions[j]->selected() ) 
            setRevision(j);
    }

}   


void
CodeWindow::initShapes() { 

    DisplayWindow::initShapes();

    Color4D border (0.45,0.45,0.45,1.0);

    fprintf(stderr, "code:initshapes\n");
	_testButton.setLabel("T");
    _testButton.setCode(KEY_CTRL_H);
	_testButton.setCols		(UI_BASE, Color4D(0.9,0.7,0.7,1.0), border );
	_testButton.setCols		(UI_HOVER, Color4D(0.8,0.1,0.1,1.0), border );
	_testButton.setCols		(UI_SEL, Color4D(0.8,0.4,0.4,1.0), border );

	_compileButton.setLabel("C");
    _compileButton.setCode(KEY_CTRL_K);
	_compileButton.setCols	(UI_BASE, Color4D(0.9,0.9,0.7,1.0), border );
	_compileButton.setCols	(UI_HOVER, Color4D(0.8,0.8,0.1,1.0), border );
	_compileButton.setCols	(UI_SEL, Color4D(0.8,0.8,0.4,1.0), border );

	_sporkButton.setLabel("S");
    _sporkButton.setCode(KEY_CTRL_L);
	_sporkButton.setCols	(UI_BASE, Color4D(0.7,0.9,0.7,1.0), border );
	_sporkButton.setCols	(UI_HOVER, Color4D(0.1,0.8,0.1,1.0), border );
	_sporkButton.setCols	(UI_SEL, Color4D(0.4,0.8,0.4,1.0), border );

	_revisionBox.setCols	(UI_BASE, Color4D(0.8,0.8,0.8,1.0), border );
	_revisionBox.setCols	(UI_HOVER, Color4D(0.8,0.8,0.8,1.0), border );
	_revisionBox.setCols	(UI_SEL, Color4D(0.8,0.8,0.8,1.0), border );
}

void
CodeWindow::reshape() { 

    DisplayWindow::reshape();

    //test button
	_testButton.moveto( _titleBox.right() + _marginSize, _titleBox.top()  );
	_testButton.resize( _titleBox.h(), _titleBox.h());

	//compile button
	_compileButton.moveto( _testButton.right() + _marginSize, _testButton.top() );
	_compileButton.resize( _titleBox.h(), _titleBox.h());

	//spork button
	_sporkButton.moveto( _compileButton.right() + _marginSize, _compileButton.top() );
	_sporkButton.resize( _titleBox.h(), _titleBox.h());
    
    UIRectangle bW = _baseWindow;

    _revisionBox.moveto( bW.w() , - _titleBox.h() * 0.5 );
    _revisionBox.resize( _titleBox.h(), bW.h() - _titleBox.h() * 0.5 );

}

void
CodeWindow::drawContent() { 
    _textcontent->draw();
    //use parse tree to draw
}

void
CodeWindow::drawUILayerInactive() { 
	
	DisplayWindow::drawUILayerInactive();

	_testButton.drawNamed(0.7);
    _compileButton.drawNamed(0.7);
    _sporkButton.drawNamed(0.7);
	
    drawRevisionList();
}

void
CodeWindow::drawUILayer() { 

    //general window functions
    DisplayWindow::drawUILayer();

	_testButton.drawNamed(0.7);
    _compileButton.drawNamed(0.7);
    _sporkButton.drawNamed(0.7);

    drawRevisionList();

}

void
CodeWindow::dragAction() { 
    DragManager  * dgm = DragManager::instance();
    if ( dgm->mode() == drag_is_Picking ) { 
        //dgm->setobject ( this , drag_DisplayWindow );
    } else if ( dgm->mode() == drag_is_Dropping ) { 
        //drag Actions             
        switch ( dgm->type() ) { 

        case drag_CodeRevision: 
            fprintf ( stderr, "hey ho\n" );
            addRevision((CodeRevision*)dgm->object(), true);
            dgm->setmode( drag_is_Empty);
            dgm->setobject(NULL, drag_None );
            break;
        }
    }
    
}

void
CodeWindow::drawRevisionList() { 

    double total = 0;
    int i;

	for ( i=0; i < _revisions.size() ; i++ ) total += _revisions[i]->size() ;
    double bh = _titleBox.h();
    double scaling = min( 1.0, _revisionBox.h() / ( bh * total ) ) ;

    if ( !_iconified ) { 
        _revisionBox.filledRounded();
        _revisionBox.outlineRounded();
    }
	glPushMatrix();

    if ( !_iconified ) { 
        glTranslated(_revisionBox.left(), _revisionBox.top(), 0.0);
    } else { 
        glTranslated(_titleBox.left() + _titleBox.h() , _revisionBox.top(), 0.0 );
    }

    glScaled ( bh * scaling , bh * scaling , 1.0 );
        
    for ( i=0; i < _revisions.size(); i++ ) { 
        if ( _revisions[i] == _current )  glLineWidth(2.0);
        else glLineWidth(1.0);      
        _revisions[i]->draw();
        glTranslated( 0, -_revisions[i]->size(), 0 );
    }

    glPopMatrix();

}

int 
CodeWindow::findRevision(CodeRevision * r) { 
    for ( int i = 0 ; i < _revisions.size() ;i++ ) 
        if ( _revisions[i] == r )
            return i;
    
    return -1;
}
void
CodeWindow::addRevision(CodeRevision * r, bool makeCurrent ) { 
    if ( findRevision(r) < 0 ) { 
        _revisions.push_back(r);
        if ( makeCurrent ) 
            setRevision(r);
    }   
}

void
CodeWindow::setRevision(CodeRevision *r ) { 
    int ri = findRevision(r);
    if ( ri >= 0 )   setRevision( ri );
    else addRevision(r, true);
}
void
CodeWindow::setRevision( int i) { 
    if ( i < 0 && i >= _revisions.size() ) return;
    if ( _current != _revisions[i] ) { 
        _current = _revisions[i];
//        _revisions.erase( _revisions.begin() + i );
//        _revisions.insert( _revisions.begin(), _current );
    }
    _textcontent->setBuf( _current->getBuffer() );
}

void
CodeWindow::removeRevision( CodeRevision * r ) { 
    int ri = findRevision(r);
    if( ri < 0 ) return;
    _revisions.erase( _revisions.begin() + ri );
    if ( _revisions.size() == 0 ) setRevision( new CodeRevision() );
    else ( setRevision ( max ( 0, min(_revisions.size()-1, ri ) ) ) );
} 

bool
CodeWindowHandler::winFromRev ( CodeRevision * r ) { 
    CodeWindow * d = NULL;
    InputState * wimp = _wm->wimp();
    if ( _wm->top() )
        ((CodeWindow *)_wm->top())->removeRevision(r);
    d = new CodeWindow(r);
    _wm->addWindow(d);
    _wm->setTopWindow(d);
    d->moveto(wimp->lastPos[0], wimp->lastPos[1] );
    return true;
}

bool
CodeWindowHandler::handleKey(const InputEvent &e) { 
    CodeWindow * d = NULL;
    //wm set the event before this was called. 
    InputState * wimp = _wm->wimp();

    bool ret = false;

    if (wimp->ctrlDown ) {

        ret = true;
        switch ( wimp->lastKey ) { 

        case KEY_CTRL_N:
            fprintf(stderr,"Make new code window\n");
            d = new CodeWindow();
            _wm->addWindow(d);
            _wm->setTopWindow(d);
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

                    d = new CodeWindow(); //new codewindow, new textwindow, empty buffer
                    d->getTextContent()->open( (char*)nf->fileName.c_str());
                    d->current()->setBuffer( d->getTextContent()->getBuf() );

                    Point2D np = wimp->lastPos + Point2D(0.03, -0.03 ) * (double)c;
                    d->moveto(np[0], np[1] );
                    _wm->addWindow(d);
                    _wm->setTopWindow(d);

                    c++;
                    nf = nf->next;
                }

            }
            else ret = false;
            break;
        default:
            ret = false;
        }
    }
    return ret;

}

bool
CodeWindowHandler::handleSpec(const InputEvent &e) { 
    return false;
}




