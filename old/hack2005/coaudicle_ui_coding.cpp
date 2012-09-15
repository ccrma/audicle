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
// name: coaudicle_ui_coding.cpp
// desc: ...
//
// authors: Philip Davidson (philipd@cs.princeton.edu)
//          Ge Wang (gewang@cs.princeton.edu)
//          Perry R. Cook (prc@cs.princeton.edu)
//          Ananya Misra (amisra@cs.princeton.edu)
// date: Autumn 2005
//-----------------------------------------------------------------------------
#include "coaudicle_ui_coding.h"
#include "coaudicle_net_session.h"

ClientWindowManager::ClientWindowManager() { 
    //we're cool for now, i think
    setHandler( new ClientWindowHandler() );
    next_buffer_id = 1;
}

ClientCodingWindow*
ClientWindowManager::newClientWindow() { 
    ClientCodingWindow * win = new ClientCodingWindow();
    win->window_id = next_window_id++;
    addClientWindow( win, win->window_id );
    return win;
}

void 
ClientWindowManager::addClientWindow( ClientCodingWindow * c, t_CKUINT i ) { 
//  assert ( c != NULL && i !=0 );  PUT THIS BACK WHEN YOU"RE DONE FUCKING AROUND....
    WindowManager::addWindow( c );
    window_id_map[i] = c;
}

void
ClientWindowManager::removeClientWindow ( t_CKUINT i ) {
    WindowManager::removeWindow( window_id_map[i] );
    window_id_map.erase(i);
}

void
ClientWindowManager::recv_update ( t_CKUINT type ) { 
    //entry point for update threads.

}

ClientWindowHandler::ClientWindowHandler() :
CodeWindowHandler() { 
    
    EM_log( CK_LOG_FINE, "starting client window handler constructor..." );
}

DisplayWindow *
ClientWindowHandler::new_window() { 

    return ClientSessionManagerImp::instance()->cur_session()->wm()->newClientWindow();
//  return new CodeWindow();
}

bool
ClientWindowHandler::winFromRev( CodeRevision * ropaque ) { 
    
    EM_log( CK_LOG_SYSTEM, "client win from rev" );
    ClientCodingWindow * d = NULL;
    ClientCodeBuffer * r = (ClientCodeBuffer*)ropaque;
    InputState * wimp = _wm->wimp();
    if ( _wm->top() )
        ((ClientCodingWindow *)_wm->top())->removeClientCodeBuffer(r);
    
    d = new ClientCodingWindow(r);
    
    _wm->addWindow(d);
    _wm->setTopWindow(d);   
    d->moveto(wimp->lastPos[0], wimp->lastPos[1] );
    
    return true;    
}


ClientCodingWindow* 
ClientWindowManager::clientWindow( t_CKUINT i ) { 
    if ( window_id_map.find(i) == window_id_map.end() ) return NULL;
    return window_id_map[i]; 
}


//CodingWindow

ClientCodingWindow::ClientCodingWindow( ) :
window_id(0) 
{ 
    initShapes();
    EM_log(CK_LOG_SYSTEM, "client code win constructor" );
    _content = _textcontent = new ClientTextContent();
    _content->containerDirty() = true;
}

ClientCodingWindow::ClientCodingWindow ( ClientCodeBuffer * b ) : 
CodeWindow(b),
window_id(0) 
{
    EM_log(CK_LOG_SYSTEM, "client win w buffer constructor" );
    initShapes();
    //need a TextContent constructor with buf arguments
    _content = _textcontent = new ClientTextContent();
    _content->containerDirty() = true;
    //create for existing revision
    setClientCodeBuffer(b);
}

void
ClientCodingWindow::setClientCodeBuffer( ClientCodeBuffer * b ) { 
    t_CKUINT bufid = b->buffer_id;
    if ( buffer_id_map.find ( bufid ) == buffer_id_map.end() ) { 
        addClientCodeBuffer(b,bufid);
    }
    _current = buffer_id_map[bufid];
	_textcontent->setBuf( _current->getBuffer() );
}


void
ClientCodingWindow::addClientCodeBuffer( ClientCodeBuffer * b, t_CKUINT i ) { 
    CodeWindow::addRevision( b );
    buffer_id_map[i] = b;   
}

void
ClientCodingWindow::removeClientCodeBuffer( ClientCodeBuffer * b) { 
    CodeWindow::removeRevision( b );
    buffer_id_map.erase(b->buffer_id);
}

void
ClientCodingWindow::recv_update( t_CKUINT type ) { 
    // stuffity stuff
}

TextBuffer * 
ClientTextContent::new_buffer() { return new ClientTextBuffer(); }

ClientTextContent::ClientTextContent() :
TextContent() 
{ 
    _buf = new ClientTextBuffer();
    EM_log(CK_LOG_SYSTEM, "client text constructor" );
}


ClientCodeBuffer * 
ClientCodingWindow::clientCodeBuffer ( t_CKUINT i ) { 
    return buffer_id_map[i];
}


ClientCodeBuffer::ClientCodeBuffer() :
CodeRevision(),
buffer_id(0)
{
    EM_log(CK_LOG_SYSTEM, "client code buffer" );
}

ClientCodeBuffer::ClientCodeBuffer(TextBuffer *b) :
CodeRevision(b),
buffer_id(0)
{
    EM_log(CK_LOG_SYSTEM, "client code ( from buffer ) " );
}

ClientCodeBuffer::ClientCodeBuffer(CodeRevision * c) :
CodeRevision(c),
buffer_id(0)
{
    EM_log(CK_LOG_SYSTEM, "client code ( from codebuffer ) " );
}





void
ClientCodeBuffer::addClientShred( ClientShred * s, t_CKUINT i ) { 
    shred_id_map[i] = s; 
}

void
ClientCodeBuffer::removeClientShred ( t_CKUINT i ) { 
    CodeRevision::removeShred( shred_id_map[i]->procID );
    shred_id_map.erase(i);
}

ClientShred * 
ClientCodeBuffer::clientShred ( t_CKUINT i ) { 
    return shred_id_map[i];
}

void
ClientCodeBuffer::recv_update( t_CKUINT type ) { 
    //something
}



ClientTextBuffer::ClientTextBuffer() : 
TextBuffer()
{  
    EM_log(CK_LOG_SYSTEM, "client text buffer constructor" );

}

void
ClientTextBuffer::pushEdit(TextEdit &e) { 
    TextBuffer::pushEdit(e);
    queue_text_edit_packet(e);
}

void
ClientTextBuffer::makeEdit(TextEdit &e) { 
    EM_log (CK_LOG_SYSTEM, "netbuffer-makeEdit");
    TextBuffer::makeEdit(e);
}

t_CKBOOL
ClientTextBuffer::queue_text_edit_packet( TextEdit &e ) { 
    
    //ASSUMPTION
    //cur_session HERE is the same as cur_session
    //at the end?
    
    //well, the session is only changed through
    //this interface, so yeeeeas..
    
    AudiclePak * pak = new AudiclePak();
    
    pak->type = USR_BUFFER_EDIT;
    
    buffer_edit_data_header head;   
    
    // has buffer ID been set by the revision???
    
    head.buffer_id = buffer_id; //unique to user(session), so don't need window id. 
    
    head.from_begin = e.from.begin;
    head.from_end   = e.from.end;
    head.from_span_length = e.from.content.length()+1; // nul
    
    head.to_begin = e.to.begin;
    head.to_end   = e.to.end;
    head.to_span_length = e.to.content.length()+1; // nul
    
    t_CKUINT header_size = sizeof(buffer_edit_data_header);
    head.data_size = header_size + head.from_span_length + head.to_span_length;
    
    pak->alloc(head.data_size);
    
    //copy in string contents in space after header position
    memcpy( pak->data + header_size, 
            e.from.content.c_str(), 
            head.from_span_length );
    memcpy( pak->data + header_size + head.from_span_length, 
            e.to.content.c_str(), 
            head.to_span_length );
    
    //htonl the header
    head.swap_me();         
    
    ClientSessionImp * session = (ClientSessionImp*)ClientSessionManagerImp::instance()->cur_session();
    session->pack_header(pak);
    return session->queue_packet ( pak );
    
}

void
ClientTextBuffer::recvEdit ( AudiclePak * p ) { 
    TextEdit nE = unpackEdit(p);
    makeEdit(nE);
}

TextEdit
ClientTextBuffer::unpackEdit ( AudiclePak * p ) { 
    
    TextEdit nE; 
    
    t_CKUINT header_size = sizeof(buffer_edit_data_header);
    buffer_edit_data_header head = *((buffer_edit_data_header*)p->data);
    head.swap_me();
    
    nE.from.begin = head.from_begin;
    nE.from.end   = head.from_end;
    nE.to.begin   = head.to_begin;
    nE.to.end     = head.to_end;
    
    nE.from.content = (char*) ( p->data + header_size );
    nE.to.content   = (char*) ( p->data + header_size + head.from_span_length );
    
    return nE; 
}


ClientShred::ClientShred() { 

}
