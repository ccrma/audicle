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
// name: coaudicle_ui_coding.h
// desc: ...
//
// authors: Philip Davidson (philipd@cs.princeton.edu)
//          Ge Wang (gewang@cs.princeton.edu)
//          Perry R. Cook (prc@cs.princeton.edu)
//          Ananya Misra (amisra@cs.princeton.edu)
// date: Autumn 2005
//-----------------------------------------------------------------------------
#ifndef __COAUDICLE_UI_CODING_H_DEFINED__
#define __COAUDICLE_UI_CODING_H_DEFINED__

#include "audicle_ui_coding.h"
#include "audicle_session_manager.h"

class ClientWindowManager;
class ClientCodingWindow;
class ClientTextContext; 
class ClientCodeBuffer;
class ClientTextBuffer;
class ClientShred;

class ClientWindowManager : public WindowManager { 
    //belong to session
public:
    ClientWindowManager();
    
public:
    void addClientWindow( ClientCodingWindow * c, t_CKUINT i );
    ClientCodingWindow* newClientWindow();
    void removeClientWindow ( t_CKUINT );
    void recv_update ( t_CKUINT type );
    ClientCodingWindow * clientWindow( t_CKUINT );  
protected:
    map < t_CKUINT , ClientCodingWindow * > window_id_map;
    t_CKUINT next_buffer_id;
    t_CKUINT next_window_id;
    
};

class ClientCodingWindow : public CodeWindow { 
    //contains ClientCodeBuffer
public:
    ClientCodingWindow();
    ClientCodingWindow( ClientCodeBuffer* r );
    ~ClientCodingWindow() {}
public:
    t_CKUINT window_id;
    
    void recv_update ( t_CKUINT type ); // for callbacks
    
    void addClientCodeBuffer( ClientCodeBuffer * b, t_CKUINT i );
    void removeClientCodeBuffer ( ClientCodeBuffer * b);
    void setClientCodeBuffer( ClientCodeBuffer *b );

    ClientCodeBuffer * clientCodeBuffer( t_CKUINT i );
    
protected:
        
    map < t_CKUINT, ClientCodeBuffer * > buffer_id_map;
};



class ClientTextContent : public TextContent { 
    //belongs 1 - 1 to a Window
    //might put in some tracers here for override.
public:
    ClientTextContent();
    virtual TextBuffer * new_buffer();
};

class ClientWindowHandler : public CodeWindowHandler { 
public:
    ClientWindowHandler();
    bool winFromRev ( CodeRevision * r );
    virtual DisplayWindow * new_window();
protected:

        
};


class ClientCodeBuffer : public CodeRevision { 
public: // constructors
    ClientCodeBuffer();
    ClientCodeBuffer( TextBuffer * b );
    ClientCodeBuffer( CodeRevision * c );
    
public: // overrides
    CodeRevision * dup();
        
public: // client-level management 
    t_CKUINT buffer_id; 
    void addClientShred ( ClientShred * s , t_CKUINT i );
    void removeClientShred ( t_CKUINT i );
    ClientShred * clientShred ( t_CKUINT i );
    void recv_update( t_CKUINT type );
protected:
    map < t_CKUINT , ClientShred * > shred_id_map;
};

//this should be a nettextbuffer,and it should be common
//to client and server...but not for now, because of the 
//coderevision+textbuffer representation we're using

//but steal the functions!

class ClientTextBuffer : public TextBuffer {    
public:
    ClientTextBuffer();
public:
    t_CKUINT buffer_id;
    virtual void pushEdit(TextEdit &e);
    virtual void makeEdit(TextEdit &e);
    
    virtual t_CKBOOL queue_text_edit_packet(TextEdit &e);
    
    virtual void recvEdit(AudiclePak *p);
    virtual TextEdit unpackEdit(AudiclePak*p);
};

class ClientShred : public ShredInstance { 
public:
    ClientShred();
public:
    t_CKUINT shred_id;
};

#endif // __COAUDICLE_UI_CODING_H_DEFINED__
