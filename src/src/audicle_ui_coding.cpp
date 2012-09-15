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
// name: audicle_ui_coding.cpp
// desc: ...
//
// authors: Philip Davidson (philipd@cs.princeton.edu)
//          Ge Wang (gewang@cs.princeton.edu)
//          Perry R. Cook (prc@cs.princeton.edu)
//          Ananya Misra (amisra@cs.princeton.edu)
// date: Autumn 2004
//-----------------------------------------------------------------------------
#include "audicle_ui_coding.h"
#include "audicle_nexus.h"
#include "audicle_ui_console.h"
#include "audicle_face_shredder.h"

#ifdef __PLATFORM_WIN32__
#include <windows.h>
#include <io.h>
#include <fcntl.h>
#endif

ck_socket CodeRevision::_tcp = NULL;
int CodeRevision::_connected = false;
char CodeRevision::_hostname[512] = "127.0.0.1";
int CodeRevision::_port = 8888;


t_CKBOOL CodeRevision::_buttonsready = false;
UIButton * CodeRevision::_sporkButton = NULL;
UIButton * CodeRevision::_killButton = NULL;

t_CKBOOL ShredInstance::_buttonsready = false;
UIButton * ShredInstance::_killButton = NULL;


void
CodeRevision::_setupButtons() { 
    if ( _buttonsready ) return;
    _sporkButton = new UIButton();
    _sporkButton->resize(0.8,0.8);
    _sporkButton->fillCol(Color4D(0.3,1.0, 0.3, 0.9));
    _sporkButton->lineCol(Color4D(0.2,0.2, 0.2, 0.9));
    _sporkButton->setLabel( "S" );
    _killButton = new UIButton();
    _killButton->resize(0.8,0.8);
    _killButton->fillCol(Color4D(1.0,0.3, 0.3, 0.9));
    _killButton->lineCol(Color4D(0.2,0.2, 0.2, 0.9));
    _killButton->setLabel( "X" );

    _buttonsready = true;
}

void
CodeRevision::draw() { 
    //assume we are a scale of 1
    glPushMatrix();
    glPushName(_id);

        UIButton rev;

        switch ( _status ) { 
            case rev_PARSED     : rev.setCols( UI_BASE, Color4D( 1.0,0.85,0.85,1.0 ), Color4D(0,0,0,1.0)); break;
            case rev_TYPED      : rev.setCols( UI_BASE, Color4D( 1.0,0.92,0.85,1.0 ), Color4D(0,0,0,1.0));  break;
            case rev_COMPILED   : rev.setCols( UI_BASE, Color4D( 1.0,1.0,0.85,1.0 ), Color4D(0,0,0,1.0));  break;
            case rev_READY	    :
            case rev_RUNNING    : rev.setCols( UI_BASE, Color4D( 0.85,1.0,0.85,1.0 ), Color4D(0,0,0,1.0));  break;
            case rev_EDITED     : rev.setCols( UI_BASE, Color4D( 0.9,0.9,1.0,1.0 ), Color4D(0,0,0,1.0));  break;
            default             : rev.setCols( UI_BASE, Color4D( 1.0,1.0,1.0,1.0 ), Color4D(0,0,0,1.0));  break;
        }

        rev.seth( 1.0 );
        rev.setLabel( (char*)_buffer->filename().c_str() );
        rev.fitXLabel( 0.75 );
        rev.draw(0.75);

        if ( _selected ) { 
            _killButton->moveto( rev.left()-_killButton->w(), rev.top() - 0.5* ( rev.h()-_killButton->h() )  );
            _killButton->drawNamed(0.75);
            _sporkButton->moveto( rev.right(), rev.top() - 0.5* ( rev.h()-_sporkButton->h() ) );
            _sporkButton->drawNamed( 0.75 );
        }
        

        glTranslated ( 0, -1.0 , 0 );
        rev.seth ( ( size() - 1 )  );
        glColor4d(0.8, 1.0, 0.9, 0.3);
        glBegin(GL_POLYGON);
        rev.roundVerts();
        glEnd();

        glTranslated ( 1.0, 0 , 0 );
        for ( int i=0; i < _shreds.size() ; i++ ) { 
			ShredInstance * s = _shreds[i];
			if ( s->stat == NULL ) { 
				EM_log( CK_LOG_WARNING, "CodeRevision::draw: (%s) shred stat is NULL (%d)!", (char*)_buffer->filename().c_str(), _shreds[i]->procID); 
				s->stat = Chuck_Stats::instance()->get_shred( s->procID );
				if ( !s->stat ) { 
					EM_log( CK_LOG_WARNING, "CodeRevision::draw: adding as dead(%d)!", (char*)_buffer->filename().c_str(), _shreds[i]->procID); 
					s->stat = new Shred_Stat();
					s->stat->state = shred_KILLED;
				}
				if ( s->stat && !s->stat->data )  {
					AudicleFaceShredder::initialize_stat_data ( s->stat );
				}
			} else {
	            s->draw();
	            glTranslated ( 0, -1.0 , 0 );
			}
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
    _selected(false),
    _tmp_fd(NULL),
    _branch_count( 0 ),
    _original(NULL)
    { 
        _setupButtons();
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
    _selected(false),
    _branch_count(0),
    _tmp_fd(NULL),
    _original(NULL)
{ 
        _setupButtons();
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
    _selected(false),
    _branch_count(0),
    _tmp_fd(NULL),
    _original(NULL)
{ 
    _setupButtons();
    *this = *c;
    _branch_count = 0;
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
    if ( _status == rev_NEED_SPLIT || (  _buffer->modified() && _status >= rev_COMPILED ) ) {
        EM_log( CK_LOG_INFO, "(audicle) splitting modified branch..." );
        CodeRevision * branch = new CodeRevision( this );
        if ( _status != rev_NEED_SPLIT ) { 
            branch->getBuffer()->setFileName( name_branch() );
        }
        *_buffer = *_original; //repair our copy
        return branch;
    }
    else return NULL;
}

std::string 
CodeRevision::name_branch() { 
    char nbuf[32];
    if ( _buffer ) {
        std::string bf = _buffer->filename();
        
        //find revision. ( last element in parens ) 
        int clparen = bf.rfind (')');
        int oparen = bf.rfind ( '(', clparen );

        if ( clparen == std::string::npos || oparen == std::string::npos ) 
        { 
            sprintf (nbuf, "(%d)", ++_branch_count);
            int dotck = bf.rfind ( ".ck" );
            if ( dotck == std::string::npos ) 
                return bf + nbuf;
            else
                return ( bf.substr ( 0, dotck ) + nbuf + bf.substr ( dotck )  );
        }
        else 
        {
            std::string current_rev = bf.substr ( oparen+1, clparen-(oparen+1) ) ;
            if ( current_rev.size() == 0 ) sprintf(nbuf, "%d", ++_branch_count );
            else sprintf(nbuf, ".%d", ++_branch_count );
            return ( bf.substr (0, oparen+1) + current_rev + nbuf + bf.substr ( clparen ) );                                          
        }
    }
    return "-empty-";
    
}


void
CodeRevision::open(char *c) { 

    if ( ! _buffer ) _buffer = new TextBuffer();
    _buffer->open(c);
    if ( _status >= rev_COMPILED ) _status = rev_NEED_SPLIT;
    else _status = rev_OPENED;

}

void
CodeRevision::setVersion() {
    _buffer->setVersion();
    if ( !_original ) _original = new TextBuffer();
    *_original = *_buffer;
    _original->setVersion();
}


void
ShredInstance::_setupButtons() { 
    if ( _buttonsready ) return;
    _killButton = new UIButton();
    _killButton->resize(0.8,0.8);
    _killButton->fillCol(Color4D(1.0,0.3, 0.3, 0.9));
    _killButton->lineCol(Color4D(0.2,0.2, 0.2, 0.9));
    _killButton->setLabel( "X" );

    _buttonsready = true;
}

void
ShredInstance::draw() { 

    UIButton shredbutton;
    
    char name[32];

    shredbutton.lineCol(Color4D(0,0,0,1.0) );

    switch ( stat->state ) { 
        case 0:
            shredbutton.fillCol(stat->data->color.interp( Color4D (1.0,0.8,0.8,1.0) , 0.25)  );
            break;
        case 1:
            shredbutton.fillCol(stat->data->color.interp( Color4D (0.8,1.0,0.8,1.0) , 0.25)  );
            break;
        case 2:
            shredbutton.fillCol(stat->data->color.interp( Color4D (1.0,1.0,0.8,1.0) , 0.25)  );
            break;
        case 3:
            shredbutton.fillCol(stat->data->color.interp( Color4D (0.5,0.5,0.5,1.0) , 0.7)  );
            break;
    }

    shredbutton.moveto(0,0);
    shredbutton.seth(1.0);
    if ( stat->children.size() ) 
        sprintf(name, "%d (%d)", procID, stat->children.size() );
    else
        sprintf(name, "%d", procID, stat->children.size() );
    shredbutton.setLabel( (char*)name );
    shredbutton.fitLabel();
    
    glPushName(pickID);
    
    shredbutton.draw(0.75);

    if ( selected ) { 
        _killButton->moveto( 0.5 * ( shredbutton.w() - _killButton->w() ) , \
                             shredbutton.top()  - 0.5 * ( shredbutton.h() - _killButton->h() ) );
        _killButton->drawNamed();
    }

    if ( selected ) { 
        glPushMatrix();
        glTranslated ( shredbutton.w(), 0, 0);
        shredbutton.resize(1.0,1.0);
        int ndead= 0;
        for ( int i = 0; i < stat->children.size(); i++ ) {         
            if ( stat->children[i]->state == 3 )
                ndead++;
            else { 
                sprintf(name, "%d", stat->children[i]->xid );
                shredbutton.setLabel(name);
                shredbutton.fitLabel();
                switch ( stat->children[i]->state ) { 
                case 0:
                    shredbutton.fillCol(Color4D (1.0,0.4,0.4,1.0) );
                    break;
                case 1:
                case 2:
                    shredbutton.fillCol(Color4D (0.4,1.0,0.4,1.0) );
                    break;
                }
                shredbutton.draw(0.75);
                glTranslated( shredbutton.w(), 0, 0);
            }
        }
        if ( ndead ) { 
            shredbutton.fillCol( Color4D ( 0.5,0.5,0.5, 0.6 ) );
            sprintf ( name, "%d dead", ndead );
            shredbutton.setLabel(name);
            shredbutton.fitLabel();
            shredbutton.draw(0.75);
        }
        glPopMatrix();
    }
    glPopName();

}

void
ShredInstance::handleMouse( const InputEvent & e ) { 
    DragManager * dm = DragManager::instance();
    selected = false;
    if ( e.checkID(pickID) ) { 
        InputEvent sub = e;
        sub.popStack();
        _killButton->handleMouse(sub);
        if ( e.state == ae_input_DOWN ) selected = true;
        dragAction( dm );
    }
    
    
}

void
ShredInstance::dragAction( DragManager * d) { 

    draggable_action dg = d->mode();
    if ( dg == ae_drag_is_Picking ) { 
        d->setobject ((void*)this, ae_drag_ShredInstance);
    }
    else if ( dg == ae_drag_is_Dropping ) { 
        switch ( d->type() ) { 
        case ae_drag_CodeRevision:
            fprintf(stderr, "drag shred to revision to replace...\n");
            break;
        case ae_drag_ShredInstance:
//            if ( this == (ShredInstance*)d->object() && _killButton->selected() ) {
            if ( this == (ShredInstance*)d->object() ) {
                //remove yourself...
                fprintf(stderr, "( kill button hit ) , removing self\n");
				if ( stat && status != shred_KILLED && stat->state != 3 ) { 
	                code->local_remove( procID );
				} else {
					//we don't pull a remove if the shred's dead, or the shred's been killed already 
					code->removeShred( this );
				}
                d->setmode ( ae_drag_is_Empty );
            }
            else if ( this != (ShredInstance*)d->object() ){ 
                fprintf(stderr, "replacing shred with self\n");
                ShredInstance * ob = (ShredInstance*)d->object();
                code->local_spork (ob->procID);
                ob->code->removeShred(ob->procID);
                d->setmode ( ae_drag_is_Empty );
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
            _shreds[j]->handleMouse( sub );
            if ( _shreds[j]->selected ) {
                _selected = false;
            }
        }
        //not a shred...
        _killButton->handleMouse(sub);
        _sporkButton->handleMouse(sub);
        dragAction( DragManager::instance() );
    }

}

void
CodeRevision::dragAction( DragManager * d) { 
    draggable_action dg = d->mode();
    if ( dg == ae_drag_is_Picking ) { 
        d->setobject ((void*)this, ae_drag_CodeRevision);
    }
    else if ( dg == ae_drag_is_Dropping ) { 
        ShredInstance * shrd;
        switch ( d->type() ) { 
        case ae_drag_ShredInstance:
            shrd =( ShredInstance *) d->object();
            if ( shrd->code != this ) { 
                local_spork( shrd->procID );
                shrd->code->removeShred( shrd->procID );
                d->setmode(ae_drag_is_Empty );
            } else { 
//                fprintf ( stderr, "");
            }
            break;
        case ae_drag_CodeRevision:
            if ( _sporkButton->selected() ) { 
                fprintf(stderr, "revision - spork self\n");
                local_spork();
            }
            if ( _killButton->selected() ) { 
                fprintf(stderr, "revision kill- all id's removed\n");
                for ( int i = _shreds.size()-1; i >= 0 ; i-- )
                    local_remove(_shreds[i]->procID);
            }
            d->setmode( ae_drag_is_Empty );
            d->setobject( NULL, ae_drag_None );
            break;
        default:
            fprintf( stderr, "drag not handled\n" );
            break;
        }
        
    }

 }

void
CodeRevision::addShred ( ShredInstance * s ) { 
    _shreds.push_back( s );
}
int 
CodeRevision::findShred ( int procid ) { 
    for ( int i = 0 ; i < _shreds.size() ; i++ ) 
        if ( procid == _shreds[i]->procID ) return i;
    return -1;
}

void
CodeRevision::removeShred( int procid ) { 
    int ind = findShred ( procid );
    if ( ind >= 0 ) { 
        _shreds.erase (_shreds.begin() + ind );
    }
	else { 
		EM_log( CK_LOG_WARNING, "CodeRevision::removeShred: attempting to remove proc (%d) not found!", procid ); 
	}
}

void
CodeRevision::removeShred( ShredInstance * s ) { 
	bool removed = false;
	for ( int i = _shreds.size() - 1 ; i >= 0 ; i-- ) { 
		if ( _shreds[i] == s ) { 
			_shreds.erase ( _shreds.begin() + i ); 
			removed = true;
		}
	}

	if ( !removed ) { 
		EM_log( CK_LOG_WARNING, "CodeRevision::removeShred: attempting to remove proc (ptr) not found!" ); 
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

void dothiswhenmsgfinishes( const Chuck_Msg * msg )
{
    CodeRevision * rev = (CodeRevision *)msg->user;
    if( !msg->replyA )
        // failed
        goto error;

    switch( msg->type )
    {
    case MSG_REPLACE:
    case MSG_ADD:
    {
        ShredInstance *  shred = new ShredInstance();
        shred->procID = msg->replyA;
        shred->status = shred_RUNNING;
        shred->code = rev;
        shred->stat = Chuck_Stats::instance()->get_shred( shred->procID );
        if ( shred->stat && !shred->stat->data )  {
            AudicleFaceShredder::initialize_stat_data ( shred->stat );
        }
        rev->addShred( shred );
		if ( !shred->stat ) { 
			EM_log( CK_LOG_WARNING, "dothiswhenmsgfinishes:: stat for proc %d not found on callback...", shred->procID );
			rev->_status = rev_READY;
		}
        else rev->_status = rev_RUNNING;
        WindowManager::addAlert( EM_lasterror() );
    }
    break;
    
    case MSG_REMOVE:
    { 
        int i = rev->findShred ( msg->replyA );
        if ( i >= 0 )
        {
            rev->removeShred ( msg->replyA );
//            rev->_shreds.erase( rev->_shreds.begin() + i );
            WindowManager::addAlert( EM_lasterror() );
        }
        else
        {
            static Color4D errCol( 1.0, 0.8, 0.8, 0.8 );
            char buffer[256];
            sprintf( buffer, "cannot remove shred %i", msg->replyA );
            WindowManager::addAlert( buffer );
        }
    }
    break;

    case MSG_REMOVEALL:
        WindowManager::addAlert( EM_lasterror() );
    break;
    }

    return;
    
error:

    static Color4D errCol( 1.0, 0.8, 0.8, 0.8 );
    WindowManager::addAlert( EM_lasterror(), errCol, 0.0, 10.0 );

    return;
}

t_CKBOOL
CodeRevision::local_spork( int replace )
{
    // if ( !compile() ) return 0;

    // log
    EM_log( CK_LOG_INFO, "(audicle) performing local spork..." );
    // push
    EM_pushlog();

    _status = rev_COMPILED;
    setVersion();

    // log
    EM_log( CK_LOG_INFO, "(audicle) replace: %s", replace ? "TRUE" : "FALSE" );
    
    Net_Msg msg;
    // make the message
    if( replace )
        msg.param = replace;
    // set the type
    msg.type = replace ? MSG_REPLACE : MSG_ADD;
    strcpy ( (char*)msg.buffer, "editor->" );
    strcat ( (char*)msg.buffer, _buffer->filename().c_str() );

    // log
    EM_log( CK_LOG_INFO, "(audicle) name: %s", (char *)msg.buffer );

    // make tmp file
#ifdef __PLATFORM_WIN32__
	HANDLE hRead, hWrite;
	int fd_read, fd_write;
	
	int code_size = 0, k, code_num_lines = _buffer->nlines();
	for( k = 0; k < code_num_lines; k++ )
	{
		code_size += _buffer->line( k ).size();
	}

	code_size += code_num_lines;

	if( !CreatePipe( &hRead, &hWrite, NULL, code_size ) )
	{
		EM_log( CK_LOG_SEVERE, "(audicle): error in CreatePipe: %d", GetLastError() );
		return FALSE;
	}

	// WARNING: fails on 64 bit Windows!
	fd_read = _open_osfhandle( ( long ) hRead, _O_RDONLY );
	fd_write = _open_osfhandle( ( long ) hWrite, _O_WRONLY );

	FILE * read = _fdopen( fd_read, "r" );
	FILE * write = _fdopen( fd_write, "w" );

	_buffer->print( write );
	fclose( write );
#else
    if( _tmp_fd ) fclose( _tmp_fd );
    _tmp_fd = tmpfile();
    _buffer->print( _tmp_fd );
#endif

    // log
    EM_log( CK_LOG_INFO, "(audicle) sending SPORK request..." );
    // send it
#ifdef __PLATFORM_WIN32__
    if( !process_msg2( &msg, dothiswhenmsgfinishes, this, read ) )
#else
	if( !process_msg2( &msg, dothiswhenmsgfinishes, this, _tmp_fd ) )
#endif
    {
        // log
        EM_log( CK_LOG_INFO, "(audicle) return code: failure..." );

        // if here, then the message failed already 
        static Color4D errCol( 1.0, 0.8, 0.8, 0.8 );
        fprintf( stderr, "[audicle]: error in local spork %s\n", EM_lasterror() );
        parseErrorLocation( EM_lasterror() );
        WindowManager::addAlert( EM_lasterror(), errCol, 0.0, 10.0 );

        // pop
        EM_poplog();

        return FALSE;
    }
    // log
    // EM_log( CK_LOG_INFO, "(audicle) local spork successful..." );

    // pop
    EM_poplog();

    return TRUE;
}

void
CodeRevision::parseErrorLocation ( std::string errg ) { 
        
        //fprintf(stderr, "searching %s for error loc\n", errg.c_str() );
        int lpos = errg.find ("line(");
        if ( lpos == std::string::npos ) return;
        lpos += 5;
        int cparen = errg.find( ')', lpos );
        if ( cparen == std::string::npos || cparen == lpos ) return;

        int linenum = atoi ( errg.substr( lpos, cparen - lpos ).c_str() );
        int charnum = -1;
        int cpos = errg.find( "char(" );
        if ( cpos != std::string::npos ) {
            cpos += 5;
            cparen = errg.find( ')', cpos );
            if ( cparen != std::string::npos ) { 
                charnum = atoi ( errg.substr( cpos, cparen - cpos ).c_str() );
            }
        }
        //XXX - for some reason the type checker ( no char pos ) returns lines numbered at 0? 
        if ( charnum < 0 ) linenum ++ ;
        //fprintf(stderr, "pushing %d %d\n ", linenum, charnum );
        _errloc.push( TextLoc ( linenum-1 , charnum-1 ) );
        
}

int 
CodeRevision::spork( int replace ) {
    static int shredid = 1;
    if ( !compile() ) return 0;

    //pretend to receive results from the TCP server

    ShredInstance * nshred = new ShredInstance();

    if ( nshred->procID = sendtcp( replace ) ) {
        nshred->procID = ( replace ) ? replace : shredid++; //TEMP
        nshred->status = shred_RUNNING;
        nshred->code = this;
        _shreds.push_back( nshred );
        _status = rev_RUNNING;
        return nshred->procID;
    }
    else return 0;
}

t_CKBOOL
CodeRevision::local_remove( int id )
{
    // if ( !compile() ) return 0;
    
    Net_Msg msg;
    // set the type
    msg.type = MSG_REMOVE;
    msg.param = id ? id : 0xffffffff;
    // send it
    if( !process_msg2( &msg, dothiswhenmsgfinishes, this, NULL ) )
    {
        // if here, then the message failed already 
        static Color4D errCol( 1.0, 0.8, 0.8, 0.8 );
        WindowManager::addAlert( EM_lasterror(), errCol, 0.0, 10.0 );
        return FALSE;
    }
    
    return TRUE;
}

int
CodeRevision::removetcp(int i) { 
    fprintf(stderr, "removing shred %d\n", _shreds[i]->procID );
    connect_tcp();
    if ( _connected ) { 
        Net_Msg msg;
        msg.type = MSG_REMOVE;
//      msg.param = _shreds[i].procID;
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
        EM_log( CK_LOG_FINER, "(audicle) CodeWindow base ctor...");
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
        case KEY_CTRL_O:
        case KEY_CTRL_F:
            _current->open( NULL );
            resize ( max ( 2.2, _baseWindow.w() ) , max ( 2.4, _baseWindow.h() ) );
            _textcontent->refreshView();
            break;
        case KEY_CTRL_L:
            _current->local_spork( 0 );
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
        if ( e.state == ae_input_UP ) { 
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

    UIMenu * menu = new UIMenu();
    menu->addOption("open", KEY_CTRL_O );
    menu->addOption("save", KEY_CTRL_S );
    menu->addOption("close", KEY_CTRL_W );
    _closeButton.setMenu( menu );

    _testButton.setLabel("O");
    _testButton.setCode(KEY_CTRL_O);
    _testButton.setCols     (UI_BASE, Color4D(0.9,0.7,0.7,1.0), border );
    _testButton.setCols     (UI_HOVER, Color4D(0.8,0.1,0.1,1.0), border );
    _testButton.setCols     (UI_SEL, Color4D(0.8,0.4,0.4,1.0), border );
    
    _compileButton.setLabel("C");
    _compileButton.setCode(KEY_CTRL_K);
    _compileButton.setCols  (UI_BASE, Color4D(0.9,0.9,0.7,1.0), border );
    _compileButton.setCols  (UI_HOVER, Color4D(0.8,0.8,0.1,1.0), border );
    _compileButton.setCols  (UI_SEL, Color4D(0.8,0.8,0.4,1.0), border );
    
    _sporkButton.setLabel("S");
    _sporkButton.setCode(KEY_CTRL_L);
    _sporkButton.setCols    (UI_BASE, Color4D(0.7,0.9,0.7,1.0), border );
    _sporkButton.setCols    (UI_HOVER, Color4D(0.1,0.8,0.1,1.0), border );
    _sporkButton.setCols    (UI_SEL, Color4D(0.4,0.8,0.4,1.0), border );
    
    _revisionBox.setCols    (UI_BASE, Color4D(0.8,0.8,0.8,1.0), border );
    _revisionBox.setCols    (UI_HOVER, Color4D(0.8,0.8,0.8,1.0), border );
    _revisionBox.setCols    (UI_SEL, Color4D(0.8,0.8,0.8,1.0), border );
}

void
CodeWindow::reshape() { 

    DisplayWindow::reshape();
    
    EM_log( CK_LOG_FINE, "(audicle) CodeWindow::reshape..." );
    
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
CodeWindow::checkErrorLocations() { 
    if ( _current ) { 
        if ( _current->error_loc().size() ) { 
            TextLoc el = _current->error_loc().front();
            if ( el.chr < 0 ) { //highlight entire line
                _textcontent->setLoc( TextLoc ( el.line, 0 ) , true );
                _textcontent->setLoc( TextLoc ( el.line, _textcontent->getBuf()->line(el.line).size() ), false );
            }
            else { 
                _textcontent->setLoc( el, true );
            }
            _current->error_loc().pop();
        }
    }
}

void
CodeWindow::drawContent() { 

    checkErrorLocations();
    _textcontent->draw();
    //use parse tree to draw
}


void
CodeWindow::drawUILayerInactive() { 
    
    DisplayWindow::drawUILayerInactive();

    _testButton.drawNamed(0.7f);
    _compileButton.drawNamed(0.7f);
    _sporkButton.drawNamed(0.7f);
    
    drawRevisionList();
}

void
CodeWindow::drawUILayer() { 

    //general window functions
    DisplayWindow::drawUILayer();

    _testButton.drawNamed(0.7f);
    _compileButton.drawNamed(0.7f);
    _sporkButton.drawNamed(0.7f);

    drawRevisionList();

}

void
CodeWindow::dragAction() { 
    DragManager  * dgm = DragManager::instance();
    if ( dgm->mode() == ae_drag_is_Picking ) { 
        //dgm->setobject ( this , ae_drag_DisplayWindow );
    } else if ( dgm->mode() == ae_drag_is_Dropping ) { 
        //drag Actions             
        switch ( dgm->type() ) { 

        case ae_drag_CodeRevision: 
            fprintf ( stderr, "hey ho\n" );
            addRevision((CodeRevision*)dgm->object(), true);
            dgm->setmode( ae_drag_is_Empty);
            dgm->setobject(NULL, ae_drag_None );
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

    //fit our box to the size of the window.
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
    if ( i < 0 || i >= _revisions.size() ) return;
    if ( _current != _revisions[i] ) { 
        _current = _revisions[i];
        _textcontent->setBuf( _current->getBuffer() );
    }

}

void
CodeWindow::removeRevision( CodeRevision * r ) { 
    int ri = findRevision(r);
    if( ri < 0 ) return;
    _revisions.erase( _revisions.begin() + ri );
    if ( _revisions.size() == 0 ) setRevision( new CodeRevision() );
    else ( setRevision ( max ( 0, min(_revisions.size()-1, ri ) ) ) );
} 

DisplayWindow * 
CodeWindowHandler::new_window() { return new CodeWindow(); }

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
    DisplayWindow * dd = NULL;
    //wm set the event before this was called. 
    InputState * wimp = _wm->wimp();

    bool ret = false;

    if (wimp->ctrlDown ) {

        ret = true;
        switch ( wimp->lastKey ) { 

        case KEY_CTRL_U:
            _wm->arrangeWindows();
            break;
        case KEY_CTRL_N:
            d = (CodeWindow*)new_window();
            d->current()->setBuffer( d->getTextContent()->getBuf() );
            d->moveto(wimp->lastPos[0], wimp->lastPos[1] );
            _wm->addWindow(d);
            _wm->setTopWindow(d);
            
            break;
        case KEY_CTRL_F:
        case KEY_CTRL_O:    //create new window and pass the call down...
            if ( !_wm->top()) { 
                DirScanner dr;
                fileData * f = dr.openFileDialog();
                fileData * nf = f;
                int c = 0;
                while ( nf ) { 
                    EM_log( CK_LOG_INFO, "(audicle) opening file %d : %s", c, nf->fileName.c_str() );

                    d = (CodeWindow*)new_window(); //new codewindow, new textwindow, empty buffer
                    d->getTextContent()->open( (char*)nf->fileName.c_str());
                    d->current()->setBuffer( d->getTextContent()->getBuf() );

                    Point2D np = wimp->lastPos + Point2D(0.03, -0.03 ) * (double)c;
                    d->moveto(np[0], np[1] );
                    d->resize( max ( 2.25, d->base()->w() ) , max ( 2.5, d->base()->h() )  );
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




