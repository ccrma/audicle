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
// name: audicle_session_manager.h
// desc: ...
//
// authors: Philip Davidson (philipd@cs.princeton.edu)
//          Ge Wang (gewang@cs.princeton.edu)
//          Perry R. Cook (prc@cs.princeton.edu)
//          Ananya Misra (amisra@cs.princeton.edu)
// date: Autumn 2004
//-----------------------------------------------------------------------------
#ifndef __AUDICLE_SESSION_MANAGER_H__
#define __AUDICLE_SESSION_MANAGER_H__

#include "coaudicle.h"
#include "audicle_net.h"
#include "coaudicle_chat.h"
#include "co.h"
#include "coaudicle_avatar.h"
#include "audicle_nexus.h"
class ClientUser; 
class ClientServerImp;
// this should really be up on-level with Audicle or Co-audicle.  possibly
// encompassed by co-audicle?


class ClientSessionManagerImp : public CoSessionManager { 
protected:
    static  ClientSessionManagerImp *   _inst;
    static  CoSession *                 _local_session;
    std::vector < CoSession * >         m_sessions;
    CoSession *                         m_cur_session;
    ClientUser *                        m_local_user;
    std::string                        m_say;
public:
    ClientSessionManagerImp();
    static  ClientSessionManagerImp * instance();
    virtual CoSession* local_session();
    void init_local_session();
    virtual CoSession * new_session();
    virtual std::vector < CoSession * >& sessions();
    virtual CoSession * cur_session();
    virtual CoUser * local_user();            
    virtual CoSession * add_session ( CoSession * s ) ;
    virtual CoSession * set_cur_session ( CoSession * s ) ;
        virtual void make_cur_session ( CoSession * s);
    virtual CoSession* get_session( char * s);
    virtual CoSession* new_session ( const char * host, int port1 = CO_REQUEST_PORT, int port2 = CO_UPDATE_PORT) ;
    virtual t_CKBOOL console_request ( std::string line );
        virtual std::string say();
};

class ClientUser : public CoUser { 

protected:
    std::string  m_username;
    std::string  m_host;
    CoBindle *   m_bindle;
    CoServer *   m_server;
    unsigned int m_id;
    
    //state info
//  CoCoUserState m_state;
    Agent    * m_agent;

public:
    ClientUser();
    ClientUser( std::string name, std::string host );
    std::string username() { return m_username; }
    unsigned int id() { return 0; } // uuid? we only care about sessionid right now
    std::string host() { return m_host; }
    CoServer * server() { return m_server; }
    Agent *    agent() { return m_agent; }
    CoBindle * bindle() { return NULL; } 
    
    
    Point3D location() { return m_agent->pos; }
    Point3D direction() { return m_agent->vel; }
    Color4D color() { return m_agent->dresscol ; }
    Point3D home() { return m_agent->home; }

    void sendState ();
    void setState (AudiclePak *p);
    void sendInfo ();
    void setInfo ( std::string name, std::string host ); 
    void setInfo ( AudiclePak *p);
    double size() { return 1.0; } 
    
    virtual ~ClientUser() {}
};

class ClientServerImp;

class ClientSessionImp : public CoSession { 

protected:
    CoUser *            m_user;
    ClientWindowManager *   m_wm;
    CoServer *          m_server;
    unsigned int        m_state;
    AudicleClient *     m_client;
    t_CKUINT            m_id;
          XMutex        m_mutex;
public:
    ClientSessionImp();
    XMutex& mutex() { return m_mutex; }
    virtual CoUser   *      user();
    virtual ClientWindowManager * wm();
    virtual CoChat   *      chat();
    virtual CoServer *      server();
    virtual void setServer( CoServer * c ) { m_server = c; } 
    bool                    local();
    virtual unsigned int    start ( const char * address, t_CKUINT port1, t_CKUINT port2 );
    virtual unsigned int    start ( ClientServerImp * c );
    virtual void            disconnect();
    virtual void            handleEvent( const InputEvent &e );
    virtual unsigned int    state(); // new, starting, connected, disconnected, idle, exited 
    virtual unsigned int    request_status ( unsigned int target_status );
    virtual t_CKUINT        id();
    virtual ClientServerImp * cserver() { return (ClientServerImp*)m_server; }; 

    
    virtual void pack_header( AudiclePak * pak );
    virtual t_CKBOOL queue_packet( AudiclePak * pak );


    virtual t_CKBOOL handle_packet( AudiclePak * pak );

    virtual t_CKBOOL handle_chat_update_packet ( AudiclePak * pak );
    
    virtual t_CKBOOL handle_join_answer_packet      ( AudiclePak * pak );
    virtual t_CKBOOL handle_name_answer_packet      ( AudiclePak * pak );
    virtual t_CKBOOL handle_notify_answer_packet    ( AudiclePak * pak );
    
    virtual t_CKBOOL handle_user_add_packet         ( AudiclePak * pak );
    virtual t_CKBOOL handle_user_notify_packet      ( AudiclePak * pak );
    
    virtual t_CKBOOL handle_window_add_packet       ( AudiclePak * pak );
    virtual t_CKBOOL handle_window_mod_packet       ( AudiclePak * pak );
    virtual t_CKBOOL handle_window_remove_packet    ( AudiclePak * pak );
    
    virtual t_CKBOOL handle_buffer_add_packet       ( AudiclePak * pak );
    virtual t_CKBOOL handle_buffer_edit_packet      ( AudiclePak * pak );
    
    virtual t_CKBOOL handle_shred_add_packet        ( AudiclePak * pak );
    virtual t_CKBOOL handle_shred_remove_packet     ( AudiclePak * pak );
    
    virtual t_CKBOOL handle_vm_answer_packet        ( AudiclePak * pak );
    virtual t_CKBOOL handle_update_dude_packet      ( AudiclePak * pak );


};

void client_packet_callback();

//view of the server from the client's perspective. 
class ClientServerImp : public CoServer { 
public:
  CoChat *  chat();
  std::string name();
  
  std::string host();
    t_CKUINT  port1();
    t_CKUINT  port2();

    void initialize_client_session ( ClientSessionImp * s, t_CKUINT i );
    void initialize_chat( ClientSessionImp * s );
    void add_session ( CoSession * , t_CKUINT i );
    void remove_session ( t_CKUINT i );
    CoSession * client_session;
    CoSession * session( t_CKUINT i );
    std::vector < CoSession * >& sessions();
    ClientServerImp( std::string host, t_CKUINT port1, t_CKUINT port2 );
    ClientSessionImp * csession( t_CKUINT i) { return (ClientSessionImp*)session(i); }
protected:
        
    std::string       m_name;
    std::string       m_host;
    t_CKUINT          m_port1;
    t_CKUINT          m_port2;
    CoChatImp *       m_chat; 
    std::vector < CoSession * > m_sessions;
    std::map < t_CKUINT , CoSession * > m_id_sessions;
    CoSession *       m_client_session;
};

#endif //header ifdef
