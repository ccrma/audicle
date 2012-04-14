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
// file: audicle_session_manager.cpp
// desc: ...
//
// author: Ge Wang (gewang@cs.princeton.edu)
//         Perry R. Cook (prc@cs.princeton.edu)
//         Ananya Misra (amisra@cs.princeton.edu)
//         Philip Davidson (philipd@cs.princeton.edu)
// date: 2005
//-----------------------------------------------------------------------------
#include <algorithm>
#include "audicle_session_manager.h" 
#include "coaudicle_ui_coding.h"
#include "coaudicle_protocol.h" //routing message types
#include "coaudicle_chat.h"
#include "audicle_nexus.h"
#include "coaudicle_net_session.h"
#include "co.h"

using namespace std;

//Session Manager that runs on the client ( GUI )
ClientSessionManagerImp* ClientSessionManagerImp::_inst = NULL;
CoSession * ClientSessionManagerImp::_local_session = NULL;

ClientSessionManagerImp*
ClientSessionManagerImp::instance() { 
    if ( _inst != NULL ) return _inst; 
    _inst = new ClientSessionManagerImp(); 
    return _inst;
}


ClientSessionManagerImp::ClientSessionManagerImp() { 
    m_cur_session = NULL;    
    m_local_user = NULL;
    m_say = "";
}

std::string
ClientSessionManagerImp::say() { 
    return m_say;
}

CoSession * 
ClientSessionManagerImp::cur_session() { 
    return m_cur_session; 
}

std::vector < CoSession * > & 
ClientSessionManagerImp::sessions() { 
    return m_sessions;
}

CoSession * 
ClientSessionManagerImp::new_session() { 
    return new ClientSessionImp();    
}

CoSession * 
ClientSessionManagerImp::new_session ( const char * host, int port1, int port2 ) { 
    ClientSessionImp * n = (ClientSessionImp*) new_session();
    if ( !n ) { 
        EM_log ( CK_LOG_SYSTEM, "session creation failed!" );
        return NULL;
    }
    n->setServer( new ClientServerImp( host, port1, port2 ) );
    add_session( n );
    return n;
}

CoSession * 
ClientSessionManagerImp::add_session ( CoSession * s ) { 
    if ( find ( m_sessions.begin(), m_sessions.end(), s ) == m_sessions.end() ) 
      m_sessions.push_back(s);
    return s;
}

CoSession * 
ClientSessionManagerImp::set_cur_session ( CoSession * s ) { 

    if ( m_cur_session == s ) return s;
    else if ( m_cur_session && m_cur_session->state() != SESSION_DISCONNECTED ) {

      EM_log ( CK_LOG_INFO, "exiting current session..." );
      m_cur_session->disconnect();
    }
    //WAIT?

    //s should not be connected yet, 
    //and it should have an disconnected server set inside
    if ( s->state() == SESSION_DISCONNECTED ) { 
        if ( s->start ( s->server()->host().c_str() , s->server()->port1(), s->server()->port2() ) == 0 ) { 
            EM_log ( CK_LOG_SYSTEM, "session creation failed!" );       
            return NULL;
        }
    }

    //right now just leave the old session as it was
    m_cur_session = s;
    //update system
    Audicle::instance()->set_session(s);
    //SHOULD now safe to delete previous now.
    return m_cur_session;
}

void
ClientSessionManagerImp::make_cur_session( CoSession *s ) { 
    m_cur_session = s; 
    Audicle::instance()->set_session(s);
}


CoUser * 
ClientSessionManagerImp::local_user() { 
    return m_local_user;
}

CoSession *
ClientSessionManagerImp::local_session() { 
    if ( _local_session != NULL ) init_local_session();
    return _local_session; 
}

void
ClientSessionManagerImp::init_local_session() { 
    instance();
    /*
     XXX SOMETHING HAPPENS HERE...
     _local_session = new ClientSessionImp();
     _local_session->start ("localhost", UP_PORT, RECV_PORT); 
     */
}


CoSession * 
ClientSessionManagerImp::get_session (char * servername ) { 
    if ( strcmp ( "localhost", servername ) == 0 ) return local_session();
    for ( int i=0 ; i < m_sessions.size() ; i++ ) { 
        if ( strcmp ( m_sessions[i]->server()->name().c_str(), servername ) == 0 )
            return m_sessions[i];
    }
    return NULL;
}


//ClientSessionImp

//implements the session for the local client. 

ClientSessionImp::ClientSessionImp() : 
    m_server(NULL),
    m_state(SESSION_DISCONNECTED),
    m_user(NULL),
    m_wm(NULL),
    m_client(NULL)
{  
    m_client = new AudicleClient(); 
    m_client->m_session = this; //leave a pointer in the client ( for client recv loop )
    m_wm = new ClientWindowManager();
    m_user = new ClientUser();
}


unsigned int
ClientSessionImp::start( ClientServerImp * server )  { 
    m_server = server;
    m_id = m_client->connect( m_server->host().c_str() , m_server->port1(), m_server->port2() );
    EM_log ( CK_LOG_SYSTEM, "****SERVER adding id %d", m_id );
    mutex().acquire();
    server->initialize_client_session ( this, m_id );
    mutex().release();
    m_state = 0;
    return m_id;
}

unsigned int
ClientSessionImp::start( const char * host, t_CKUINT port1, t_CKUINT port2 ) { 
    return start ( new ClientServerImp( host, port1, port2 ) );
}

void
ClientSessionImp::disconnect() {
    AudiclePak packit;
    
    // set the type
    packit.type = NOTIFY_REQUEST;
    // set the user and server
    pack_header( &packit );
    // set status
    packit.alloc( 4 );
    packit.length = 4;
    *(int *)packit.data = SESSION_DISCONNECTED;
    m_state = SESSION_DISCONNECTED;
    
    // send it
    queue_packet( &packit );
}


CoChat *
ClientSessionImp::chat()    {  
    if ( !m_server ) return NULL; 
    return m_server->chat();
}

CoServer * 
ClientSessionImp::server()  {   return m_server;  }

bool
ClientSessionImp::local()  {  return true; }

CoUser * 
ClientSessionImp::user()  {  return m_user;  }

ClientWindowManager * 
ClientSessionImp::wm()  {  return m_wm;  }

t_CKUINT 
ClientSessionImp::id() { return m_id;  } 

unsigned int
ClientSessionImp::state() {  return m_state; }

unsigned int
ClientSessionImp::request_status( unsigned int target_status  ) { 
    //send status_change request to the server
    return 0;
}


void
ClientSessionImp::handleEvent ( const InputEvent &e ) { 
    m_wm->handleEvent(e);
}

t_CKBOOL
ClientSessionManagerImp::console_request( std::string line ) { 
    EM_log ( CK_LOG_SYSTEM, "request via shell %s ", line.c_str() );
    if ( line.substr(1,4) == "chat" ) { 
        EM_log ( CK_LOG_SYSTEM, "console chat %s ", line.c_str() );
        CoChat * chat = cur_session()->server()->chat();
        chat->send_mesg ( new CoChatMesgImp ( "shell", line.substr( 6 ) ) ); 
        return TRUE;
    }
    if ( line.substr(1,4) == "join") { 
        EM_log ( CK_LOG_SYSTEM, "console join %s ", line.c_str() );
        CoSession* tojoin = new_session( line.substr(6).c_str() );
        set_cur_session( tojoin ) ;
        return TRUE;
    }

    if ( line.substr(1,3) == "say") { 
        EM_log ( CK_LOG_SYSTEM, "console say %s ", line.c_str() );
        m_say = line.substr( min(line.length(), 5) );
        return TRUE;
    }

    return FALSE;
}
//wrap AudicleClient

void
ClientSessionImp::pack_header( AudiclePak * pak ) {
    pak->user = id();
    strncpy ( pak->server, server()->name().c_str() , 32  );
    pak->server[31] = '\0';
}

t_CKBOOL
ClientSessionImp::queue_packet( AudiclePak * pak ) { 
    return m_client->queue( pak );
}

t_CKBOOL
ClientSessionImp::handle_packet( AudiclePak * pak) { 

  //this presently happens on our server thread, 
  //so we need to buffer
  //packets if we're going to instantiate anything,
  //otherwise we can't delete. 

    EM_log ( CK_LOG_SYSTEM, "Session Handler: received packet %d %d", pak->type, pak->length );
    //mutex().acquire();
    //take a packet, and figure out what to do with it. 
        // switch on type
    t_CKBOOL ret = TRUE;
    switch( pak->type )
    {
        // connection
    case JOIN_ANSWER: 
        ret = handle_join_answer_packet ( pak );
        break;
    case NAME_ANSWER: 
        ret = handle_name_answer_packet ( pak );
        break;
    case NOTIFY_ANSWER: 
        ret = handle_notify_answer_packet ( pak );
        break;
    // chat
    case CHAT_UPDATE: 
        ret = handle_chat_update_packet( pak );
        break;
    
    // session
    case HEY_USER_ADD:
        ret = handle_user_add_packet( pak );
        break;
    case HEY_USER_NOTIFY: 
        ret = handle_user_notify_packet( pak );
        
        break;
    case HEY_WINDOW_ADD:
        ret = handle_window_add_packet( pak );
        break;
    case HEY_WINDOW_MOD: 
        ret = handle_window_mod_packet( pak );
        break;
    case HEY_WINDOW_REMOVE: 
        ret = handle_window_remove_packet( pak );
        break;
    case HEY_BUFFER_ADD: 
        ret = handle_buffer_add_packet( pak );
        break;
    case HEY_BUFFER_EDIT: break;
        ret = handle_buffer_edit_packet ( pak );
    case HEY_SHRED_ADD: break;
        ret = handle_shred_add_packet( pak );
    case HEY_SHRED_REMOVE: break;
        ret = handle_shred_remove_packet ( pak );

    // vm
    case VM_ANSWER: break;
      ret = handle_vm_answer_packet ( pak );
      // dude
    case UPDATE_DUDE: 
      ret = handle_update_dude_packet ( pak );
      break;

    default: break;
    }

    //    mutex().release();

    //    return ret;

    return !!TRUE;

}

t_CKBOOL
ClientSessionImp::handle_chat_update_packet( AudiclePak * pak ) { 

    CoChatMesgImp *c  = new CoChatMesgImp( pak );
    server()->chat()->recv_mesg ( c );
    return TRUE;
}

t_CKBOOL ClientSessionImp::handle_join_answer_packet ( AudiclePak * pak ) 
{ 
    return TRUE; 
}

t_CKBOOL ClientSessionImp::handle_name_answer_packet        ( AudiclePak * pak ) 
{ 

    return TRUE; 
}

t_CKBOOL ClientSessionImp::handle_notify_answer_packet  ( AudiclePak * pak ) 
{ 
    return TRUE; 
}


t_CKBOOL ClientSessionImp::handle_user_add_packet           ( AudiclePak * pak ) 
{ 
  //we need to allow for the case where we're trying to add our 
  //own session...

  //if this happens before we have our own session assigned,
  //it will create a new session with our id...

    //get new user id 
    EM_log( CK_LOG_SYSTEM, "USER ADD recieved %s", pak->data );
    //unpack data

    t_CKUINT user_id = *((t_CKUINT*)pak->data); //WHY IS THIS NOT REVERSED?

    EM_log( CK_LOG_INFO, "USER ADD ID %d?", user_id );

    if ( cserver()->session(user_id) != NULL ) { 
        EM_log ( CK_LOG_INFO, "User exists, ignoring packet\n" );
        return TRUE;
    }

    EM_log (CK_LOG_INFO, "user name %s, host %s", pak->data+4, pak->data+36 );

    ClientSessionImp * nS = new ClientSessionImp();

    cserver()->add_session( nS, user_id);

    ((ClientUser*)nS->user())->setInfo( (char*)pak->data+4, (char*)pak->data+36 );


    return TRUE; 
}

t_CKBOOL ClientSessionImp::handle_user_notify_packet        ( AudiclePak * pak ) 
{

    return TRUE;
}

t_CKBOOL ClientSessionImp::handle_window_add_packet     ( AudiclePak * pak ) 
{ 
    
    return TRUE; 
}

t_CKBOOL ClientSessionImp::handle_window_mod_packet     ( AudiclePak * pak ) 
{ 
    return TRUE; 
}

t_CKBOOL ClientSessionImp::handle_window_remove_packet  ( AudiclePak * pak ) 
{ 
    return TRUE; 
}

t_CKBOOL ClientSessionImp::handle_buffer_add_packet     ( AudiclePak * pak ) 
{ 
    return TRUE; 
}

t_CKBOOL ClientSessionImp::handle_buffer_edit_packet        ( AudiclePak * pak ) 
{ 
    t_CKUINT user_id = 0;
    t_CKUINT buffer_id = 0;
    ClientSessionImp * s = cserver()->csession(user_id);
//  s->wm()->clientWindow(
    return TRUE; 
}

t_CKBOOL ClientSessionImp::handle_shred_add_packet      ( AudiclePak * pak ) 
{ 
    return TRUE; 
}

t_CKBOOL ClientSessionImp::handle_shred_remove_packet       ( AudiclePak * pak ) 
{ 
    return TRUE; 
}

t_CKBOOL ClientSessionImp::handle_vm_answer_packet      ( AudiclePak * pak ) 
{ 
    return TRUE; 
}

t_CKBOOL ClientSessionImp::handle_update_dude_packet        ( AudiclePak * pak ) 
{ 
    
EM_log( CK_LOG_SYSTEM, "UPDATE DUDE received" );
  t_CKUINT user_id;

  user_id = pak->user;

  CoCoUserState * state = (CoCoUserState *)pak->data;
  swap4byteregion ( (t_CKUINT*) state, ( sizeof( CoCoUserState ) - 64 ) / 4 );
  ClientUser * userinf = (ClientUser*)cserver()->csession(user_id)->user();
  Agent* ap = userinf->agent();

  ap->pos = state->m_location;
  ap->vel = state->m_orientation;
  ap->dresscol = state->m_color;
  ap->message = state->m_msg;
  ap->home = state->m_home;
  
  
  return TRUE; 
}









ClientServerImp::ClientServerImp( std::string host, t_CKUINT port1, t_CKUINT port2 ) :
m_host(host),
m_port1(port1),
m_port2(port2),
m_name(""),
m_chat(NULL)
{ 

}

CoChat * 
ClientServerImp::chat() { 
    return m_chat; 
}

std::string 
ClientServerImp::host() { return m_host; }

t_CKUINT
ClientServerImp::port1() { return m_port1; }

t_CKUINT
ClientServerImp::port2() { return m_port2; }

std::string 
ClientServerImp::name() { return m_name; }

std::vector < CoSession * > & 
ClientServerImp::sessions() { return m_sessions; } 

//for add user
void
ClientServerImp::add_session( CoSession * c, t_CKUINT i ) { 
  if ( m_id_sessions.find(i) == m_id_sessions.end() ) { 
    m_sessions.push_back(c);
    m_id_sessions[i] = c;
  }
}

void
ClientServerImp::remove_session( t_CKUINT i ) { 

    m_sessions.erase(find( m_sessions.begin(), m_sessions.end(), m_id_sessions[i] ));
    m_id_sessions.erase( i );
}

CoSession*
ClientServerImp::session( t_CKUINT i ) { 
    if ( m_id_sessions.find(i) == m_id_sessions.end() ) return NULL;
    return m_id_sessions[i];
}

void
ClientServerImp::initialize_client_session( ClientSessionImp * s, t_CKUINT i) { 

  //called during the session->connect sequence

  //this did not test for the id's existence
  //because it made the assumption that broadcasts
  //that added its own id wouldn't arrive until
  //after this happens

  //i still have no idea why a weird id is assigned. 

    m_client_session = s;
    
    add_session ( s, i );

    initialize_chat ( s );
}
void
ClientServerImp::initialize_chat( ClientSessionImp * s ) { 
    m_chat = new CoChatImp( s );
}       
 
CoUser::~CoUser() {}

ClientUser::ClientUser()  { 
    m_agent = new Agent();
    m_username ="";
    m_host  ="";
    m_server = NULL;
}

ClientUser::ClientUser( std::string name, std::string host ){ 
    m_agent = new Agent();
    m_username = name;
    m_host = host;
    m_server = new ClientServerImp(host, CO_REQUEST_PORT, CO_UPDATE_PORT ); // XX MAKE REAL PORTS
}

void
ClientUser::setInfo( std::string name, std::string host ) { 
    m_username =name;
    m_host = host;
    m_server = new ClientServerImp( host,  CO_REQUEST_PORT, CO_UPDATE_PORT );
}


void
ClientUser::setState( AudiclePak * p ) { 
    
}

void 
ClientUser::sendState ( ) { 
    
}

void
ClientUser::setInfo ( AudiclePak * p ) { 
    
}

void
ClientUser::sendInfo( ) { 
    
}
