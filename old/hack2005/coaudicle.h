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
// name: co-audicle.h
// desc: interface for co-audicle
//
// authors: Ge Wang (gewang@cs.princeton.edu)
//          Ananya Misra (amisra@cs.princeton.edu)
//          Philip Davidson (philipd@cs.princeton.edu)
//          Perry R. Cook (prc@cs.princeton.edu)
// date: Summer 2005
//-----------------------------------------------------------------------------
#ifndef __COAUDICLE_H__
#define __COAUDICLE_H__

// threads
#include "audicle_def.h"
#include "audicle_net.h"

#include "audicle.h"
#include "audicle_geometry.h"

#include "chuck_vm.h"
#include "chuck_compile.h"

#include "co.h"


//-----------------------------------------------------------------------------
// name: class CoAudicle
// desc: the monolith of convenience
//-----------------------------------------------------------------------------
class CoAudicle
{
public:
    Chuck_VM * vm() { return m_vm; }
    void set_vm( Chuck_VM * vm ) { m_vm = vm; }
    Chuck_Compiler * compiler() { return m_compiler; }
    void set_compiler( Chuck_Compiler * compiler ) { m_compiler = compiler; }
    void getip();
    static CoAudicle * instance()
    {
        if( !our_instance )
            our_instance = new CoAudicle;

        return our_instance;
    }

protected:
    static CoAudicle * our_instance;

protected:
    Chuck_VM * m_vm;
    Chuck_Compiler * m_compiler;

public:
    AudicleServer server;
    AudicleClient client;
    AudicleAudioSend audio_server;
    AudicleAudioRecv audio_client;
    CoCoServer coco_server;

    std::string the_ip;
};





// server can point to your own session on your own Audicle,
// or to another server entirely . 


/*

CoAudicle -- ( Thread IPC ) -- Chuck 
CoAudicle -- Session -( Thread IPC )- Server -- Chuck
CoAudicle -- Session -(NETWORK)- -- Server -- Chuck 

CoAudicle - user interaction & control
          - creates/manages different sessions ( asks tracker ) 
          - renders a view of the session(s)
          - interacts with user buffers
          - render buffers of other users

Session   - represents a connection to a particular Server
          - sends updated info about session to server
          - 'owns' buffers being edited by user
          - 'caches' buffers edited by other users
          - maintains local cache of the other user-sessions on the server
          - maintains local chat logs
          * thread synchronizes to server
      
          USER SIDE ( AUDICLE )
          ------------------------

          ( NETWORK ? )

          ------------------------
          SERVER SIDE ( CHUCK )

Server    - accepts connections from user sessions
          - accepts & broadcasts user info to sessions
          - accepts & broadcasts buffer info to sessions
          - send code from sessions to chuck 
          - streams audio from chuck to sessions
          - 'advertises' via tracker

Chuck     - audio computation, thread management, 
          - receives buffers from the session to code
          - report compile/syntax errors on buffers
          - report VM state
          - report VM namespace/classes ?

          - machine control messages
          - send statistics to server 
          - stream audio to server
*/

// defines for pre-link 
class CoSession;
class CoUser;
class CoChat;
class CoWindow; 
class CoWindowManager;
class CoShred;
class CoBindle;
class CoResource; 


class CoSessionManager

{
public:
//    static CoSessionManager * instance();
    virtual CoSession * new_session() = 0;
    virtual std::vector < CoSession * >& sessions() = 0;
    virtual CoSession * cur_session() = 0;
    virtual CoSession * add_session ( CoSession * s ) = 0;
    virtual CoSession * set_cur_session ( CoSession * s ) = 0;
    virtual CoSession * get_session( char * servername ) = 0; //get session by name
    virtual CoSession * local_session() = 0; // get the local session ( starting the local server if necessary )...
};

// Session needs to run a sync thread to server

// interface only - server runs on this machine or remote. 
// CoAudicle manages between local/remote experience 

// on the server itself, should we have yet another version? 
 
class CoServer

{
public:
    virtual std::string     name() = 0; //id
    virtual std::string     host() =0 ;
    virtual t_CKUINT        port1() =0;
    virtual t_CKUINT        port2() =0;
    virtual std::vector < CoSession * >&  sessions() = 0; // connected sessions 
    virtual CoSession *     session(t_CKUINT i) = 0;
    virtual CoChat *        chat() = 0;
    
//    virtual void            restart() = 0;
//    virtual void            connect( CoSession * ) = 0;
//    virtual std::vector < CoResource * >& resources( int i ) = 0;

};

// Either the local user, or someone on a different server. 

class ClientWindowManager; 

class CoSession

{
public:
    virtual t_CKUINT id() = 0; //my ID on the server
    virtual CoUser   * user() = 0;
    virtual ClientWindowManager * wm() = 0;
//  virtual CoVM     * vm() = 0; //in server
    virtual CoServer * server() = 0;
//    virtual CoChat   * chat() = 0; belongs to server

    virtual bool local() = 0;

//    virtual unsigned int start ( CoServer * ) = 0;
    virtual unsigned int start ( const char * address, t_CKUINT port1, t_CKUINT port2 ) = 0;
    virtual unsigned int state() = 0; // new, starting, connected, disconnected, idle, exited
    virtual void disconnect() = 0; // leave server
};


class CoVM { 
//    virtual std::vector < CoShred * > shreds() = 0;
//  virtual void execute ( CoBuffer * buffer ) = 0;
};

class CoWindowManager

{
public:
    virtual CoWindow * co_current() { return NULL; }
    virtual std::vector < CoWindow * > co_windows() = 0;
    virtual void co_draw() {}
    virtual void co_draw_cursor() {}
};

class CoShred
{
public:
    virtual int server() = 0;
    virtual int id() = 0;
};

//information about session-user
// their avatar . 
class CoUserState;
class CoUser
{
public:
    virtual std::string username() = 0; 
    virtual unsigned int id() = 0; // uuid? 
    virtual std::string host() = 0;
    virtual CoServer * server() =0;
    virtual CoBindle * bindle() =0;
    virtual ~CoUser() =0;
//  virtual CoUserState * state();
};

class CoUserState { 
public:
    virtual Point3D& location() = 0;
    virtual Point3D& direction() = 0;
    virtual double size() = 0;
    virtual Color4D& color() = 0;
    virtual Point3D& home() =0;
};

class CoBuffer; 

class CoWindow

{
public:
    virtual Point2D location() = 0;
    virtual Point2D size() = 0;
    virtual CoBuffer * front() = 0;
    virtual std::vector < CoBuffer * > buffers() = 0; //aka revisions
    virtual void render() = 0;
};

class CoBuffer

{
public:
    virtual std::string name() = 0;
    virtual std::string path() = 0;
    virtual std::string content() = 0;
    virtual std::string line() = 0;
//  virtual void edit ( TextEdit * t ) = 0;
};


class CoResource

{
public:
    virtual std::string name() = 0;
    virtual std::string location() = 0;
    virtual std::string type() = 0;
    virtual unsigned int   size() = 0;
    virtual char * data() = 0;
};

class CoBindle

{
public: 
    virtual std::vector < CoResource * >& resources() = 0;
};

class CoChatMesg

{

public:
    virtual t_CKUINT num() = 0;
    virtual t_CKTIME time() = 0;
    virtual std::string user() = 0;
    virtual std::string data() = 0;

};

class CoChat
{
public: 
    virtual CoServer * server() = 0;
    virtual std::vector < CoChatMesg * >& messages() = 0;
    virtual CoChatMesg* message ( int i ) = 0;
    virtual void send_mesg( CoChatMesg * mesg ) = 0;
    virtual void recv_mesg( CoChatMesg * mesg ) = 0;
};



// renderer for session ( perhaps this gets #defined elsewhere. 

class CoSessionRenderer

{
public:
    virtual CoSession* session() = 0;
    virtual void set_session( CoSession * session ) = 0;
    virtual void render() = 0;
    virtual void set_current_time( t_CKFLOAT time ) = 0;
};



/*
//session to server 

//a server actively manages the machine. 
single user - session - no public server - direct connect

user - directsession
     - localsession 
     - remotesession 


server - localuser
       - remoteuser
        //
       
single user - shared server - develops cross link 

single user - remote server - session w/ connection 

single user - view remote server .  

server is transparent to the 
*/

#endif
