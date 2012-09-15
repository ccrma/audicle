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
// name: coaudicle_protocol.cpp
// desc: ...
//
// authors: Ge Wang (gewang@cs.princeton.edu)
//          Perry R. Cook (prc@cs.princeton.edu)
//          Ananya Misra (amisra@cs.princeton.edu)
//          Philip Davidson (philipd@cs.princeton.edu)
// date: Autumn 2005
//-----------------------------------------------------------------------------
#include "coaudicle.h"

#include "audicle_net.h"
#include "chuck_errmsg.h"

#include "coaudicle_protocol.h"
#include "audicle_session_manager.h"
#include "audicle_nexus.h"

// for coaudicle.h
CoAudicle * CoAudicle::our_instance = NULL;


t_CKBOOL server_handle_join_request( AudiclePak * packit );
t_CKBOOL server_handle_notify_request( AudiclePak * packit );

//-----------------------------------------------------------------------------
// name: server_handle_packet()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL server_handle_packet( AudiclePak * packet )
{
    // switch on type
    switch( packet->type )
    {
        // connection
    case JOIN_REQUEST: server_handle_join_request( packet ); break;
    case NAME_REQUEST: break;
    case NOTIFY_REQUEST: server_handle_notify_request( packet ); break;

    // chat
    case CHAT_SEND: server_handle_chat_request( packet ); break;
    
    // session
    case USR_WINDOW_ADD: break;
    case USR_WINDOW_MOD: break;
    case USR_WINDOW_REMOVE: break;
    case USR_BUFFER_ADD: break;
    case USR_BUFFER_EDIT: break;
    case USR_BUFFER_REMOVE: break;
    
    // vm
    case VM_EXECUTE: break;

    default: break;
    }

    return !!!!!!TRUE;
}




//-----------------------------------------------------------------------------
// name: client_handle_packet()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL client_handle_packet( AudicleClient * client, AudiclePak * packet )
{    
    //each client belongs to a session
    ClientSessionImp * imp = client->m_session;
    EM_log ( CK_LOG_INFO, "######CLIENT loop received packet %d : length %d ", packet->type, packet->length );
    return imp->handle_packet( packet );
    
    // switch on type
    switch( packet->type )
    {
        // connection
    case JOIN_ANSWER: break;
    case NAME_ANSWER: break;
    case NOTIFY_ANSWER: break;

    // chat
    case CHAT_UPDATE: break;
    
    // session
    case HEY_USER_ADD: break;
    case HEY_USER_NOTIFY: break;
    case HEY_WINDOW_ADD: break;
    case HEY_WINDOW_MOD: break;
    case HEY_WINDOW_REMOVE: break;
    case HEY_BUFFER_ADD: break;
    case HEY_BUFFER_EDIT: break;
    case HEY_SHRED_ADD: break;
    case HEY_SHRED_REMOVE: break;

    // vm
    case VM_ANSWER: break;

    // dude
    case UPDATE_DUDE: break;

    default: break;
    }
    return !!TRUE;
}


t_CKBOOL server_handle_join_request( AudiclePak * packit )
{
    // log
    EM_log( CK_LOG_INFO, "server discarding JOIN_REQUEST..." );
    // push indent
    EM_pushlog();

    // log
    EM_log( CK_LOG_INFO, "return address: %s", packit->data );
    EM_log( CK_LOG_INFO, "return UDP port: %d", *(t_CKINT *)(packit->data + 256) );
    EM_log( CK_LOG_INFO, "return ip: %s", packit->data + 260 );

    // pop indent
    EM_poplog();
    
    // id
    static t_CKUINT id = 0;

    // set answer
    AudiclePak answer;
    answer.type = JOIN_ANSWER;
    answer.user = ++id; // create_new_user();
    strcpy( answer.server, "an arbitrary string" );
    answer.alloc( 84 );
    answer.length = 84;
    strcpy( answer.data, "accept" );
    (*(t_CKUINT *)(answer.data + 80)) = CO_BUFFER_SIZE;
    
    // create session
    CoCoSession * session = new CoCoSession;
    session->m_request_socket = packit->sender; // check
    session->m_user_id = answer.user; 
    session->m_ip = packit->data + 260;
    session->m_user_name = packit->data + 276; 
    session->m_host_name = packit->data + 308;
    // session->m_wm = ;
    // session->m_state = ;
    // session->m_bindle = ;
    
    // send join answer
    packit->sender->send( &answer );

    // notify other users of new user
    AudiclePak new_user;
    new_user.type = HEY_USER_ADD; 
    new_user.alloc( 100 );
    new_user.length = 100;
    EM_log (CK_LOG_SYSTEM, "BROADCAST new user %d", session->m_user_id );

    // copy user id and user name and host name
    *((t_CKUINT *)new_user.data) = htonl( session->m_user_id ); 

    strcpy( new_user.data + 4, session->m_user_name.c_str() );
    strcpy( new_user.data + 36, session->m_host_name.c_str() );
    // broadcast packet

    CoAudicle::instance()->coco_server.broadcast( &new_user );
    
    // add the new session
    CoAudicle::instance()->coco_server.add( session );
    
    return TRUE;
}

t_CKUINT client_send_join_request( AudicleClient * source, t_CKINT port )
{
    AudiclePak packit;

    // log
    EM_log( CK_LOG_INFO, "preparing JOIN_REQUEST..." );
    // push indent
    EM_pushlog();

    // set type
    packit.type = JOIN_REQUEST;
    // set user to 0
    packit.user = 0;
    // set server to "" 
    strcpy( packit.server, "" );

    // allocate payload
    packit.alloc( 372);
    // get the host name of this machine
    if( gethostname( packit.data, 256 ) )
    {
        EM_error3( "cannot get my own hostname!  what kind of a world do we..." );
        return FALSE;
    }

    // log
    EM_log( CK_LOG_INFO, "current hostname: %s", packit.data );
    // get port? ? ? - use UDP port of audio receiver
    memcpy( packit.data + 256, &port, 4 );
    // log
    EM_log( CK_LOG_INFO, "copying UDP port: %d", port );
    // ip (maximum 16 bytes)
    strcpy( packit.data + 260, CoAudicle::instance()->the_ip.c_str() );
    // log
    EM_log( CK_LOG_INFO, "copying ip address: %s", CoAudicle::instance()->the_ip.c_str() );
    // user name
    strcpy( packit.data + 276, source->m_session->user()->username().c_str() );
    // log
    EM_log( CK_LOG_INFO, "copying user name: %s", packit.data + 276 );
    // server
    strcpy( packit.data + 308, source->m_session->server()->host().c_str() ); // TODO (later): check size
    // log
    EM_log( CK_LOG_INFO, "copying server: %s", packit.data + 308 );

    // set length
    packit.length = 372;
    // log
    EM_log( CK_LOG_INFO, "packet length: %d", packit.length );

    // pop indent
    EM_poplog();

    EM_log( CK_LOG_INFO, "sending JOIN_REQUEST packet..." );

    // send the request
    if( !source->send_request( &packit ) )
    {
        EM_error3( "cannot send request packet! oh no!" );
        return 0;
    }

    // get the answer
    if( !source->recv_answer( &packit ) )
    {
        EM_error3( "cannot recv answer! nothing works!" );
        return 0;
    }

    // log
    EM_log( CK_LOG_INFO, "client received packet (hopefully JOIN_ANSWER)..." );
    EM_pushlog();
    EM_log( CK_LOG_INFO, "response: %s", packit.data );
    EM_log( CK_LOG_INFO, "buffer size: %d", *(t_CKINT *)(packit.data + 80) );
    EM_poplog();

    return packit.user; 
}


t_CKBOOL client_send_update_info( AudicleClient * source, t_CKUINT user )
{
    AudiclePak packit;

    // log
    EM_log( CK_LOG_INFO, "preparing update info packet (follow up to join request)..." );
    // push indent
    EM_pushlog();

    // set type
    //packit.type = ?;
    // set user to appropriate value
    packit.user = user;
    // set server to ""
    strcpy( packit.server, "" );

    // allocate payload
    packit.alloc( sizeof( t_CKUINT ) );
    
    // log
    EM_log( CK_LOG_INFO, "user id: %d", packit.user );

    // set length
    packit.length = 0;
    // log
    EM_log( CK_LOG_INFO, "packet length: %d", packit.length );

    // pop indent
    EM_poplog();

    EM_log( CK_LOG_INFO, "sending update packet..." );

    // send the request
    if( !source->m_update_socket.send( &packit ) )
    {
        EM_error3( "cannot send update packet! oh no!" );
        return FALSE;
    }

    return TRUE;    
}


t_CKBOOL server_handle_update_info( AudiclePak * pack )
{
    EM_log( CK_LOG_INFO, "server about to handle update info for user %d", pack->user );

    CoCoServer * server = &(CoAudicle::instance()->coco_server); 
    CoCoSession * session = server->get( pack->user );
    if( session != NULL )
    {
        EM_log( CK_LOG_INFO, "server handled update info for user %d", pack->user );
        session->m_update_socket = pack->sender;
        
        // inform user of other users for that server
        AudiclePak packet;
        packet.type = HEY_USER_ADD;
        packet.alloc( 100 );
        packet.length = 100;
        packet.user = session->m_user_id;
        map <t_CKUINT, CoCoSession *>::iterator it;
        for( it = server->m_sessions.begin(); it != server->m_sessions.end(); it++ )
        {
            *(t_CKUINT *)packet.data = (*it).first;
            strcpy( packet.data + 4, (*it).second->m_user_name.c_str() );
            strcpy( packet.data + 36, (*it).second->m_host_name.c_str() );
            session->m_update_socket->send( &packet );
        }

        // set state
        packet.type = UPDATE_DUDE;
        CoCoUserState state;
        state.m_location = Point3D( ( 2.0 * ::rand() / (t_CKFLOAT)RAND_MAX - 1.0 ), 0,
                                    ( 2.0 * ::rand() / (t_CKFLOAT)RAND_MAX - 1.0 ) );
        state.m_orientation = Vec3D( 0, 0, -1 );
        state.m_color = Color4D( (::rand() / (t_CKFLOAT)RAND_MAX), (::rand() / (t_CKFLOAT)RAND_MAX), (::rand() / (t_CKFLOAT)RAND_MAX), 1.0 );
        state.m_home = Point3D( 0, 0, 0 );
        strcpy( state.m_msg, "hey" );
        packet.alloc( sizeof(state) );
        packet.length = sizeof(state);
        memcpy( packet.data, &state, sizeof(state) );
        
        CoAudicle::instance()->coco_server.broadcast( &packet );
        
        // start audio
        CoAudicle::instance()->audio_server.add( session->m_ip.c_str(), CO_AUDIO_PORT );
    }
    else
    {
        EM_log( CK_LOG_INFO, "server not coping well" );
        return FALSE;
    }

    return TRUE;
}


/*
t_CKBOOL client_send_chat( AudicleClient * source, CoChatMesg * chat )
{
    EM_log( CK_LOG_INFO, "client sending chat" );
    
    AudiclePak packet;
    chat->pack( &packet );
    return ( source->m_request_socket.send( &packet ) > 0 );
}
  */

t_CKBOOL server_handle_chat_request( AudiclePak * packet )
{
    // read packet
    chat_mesg_data * foo = (chat_mesg_data *)packet->data;
    // copy mesg struct in packet->data (foo) and add to cocoserver instance
    CoAudicle::instance()->coco_server.chat()->add( foo );

    return TRUE;
}


t_CKBOOL server_handle_notify_request( AudiclePak * packet )
{
    // get status
    int * status = (int *)packet->data;
    
    // handle request
    switch( *status )
    {
    case SESSION_CONNECTED: // normal
    case SESSION_RETURNED: // back
        break;

    case SESSION_DISCONNECTED: // gone (+ timeout)
    case SESSION_MUTE: // mute
    {
        // get session associated with this user
        CoCoSession * session = CoAudicle::instance()->coco_server.get( packet->user );
        if( session != NULL )
        {
            EM_log( CK_LOG_SYSTEM, "*********** removing ip %s", session->m_ip.c_str() );
            // mute audio
            return CoAudicle::instance()->audio_server.remove( session->m_ip.c_str() );
        }
        else
        {
            return FALSE;
        }
        break;  
    }

    case SESSION_UNMUTE: // unmute
    {
        // get session associated with this user
        CoCoSession * session = CoAudicle::instance()->coco_server.get( packet->user );
        if( session != NULL )
        {
            // unmute audio
            return CoAudicle::instance()->audio_server.add( session->m_user_name.c_str(), CO_AUDIO_PORT );
        }
        else
        {
            return FALSE;
        }
        break;  
    }

    case SESSION_IDLE: // idle
        break;
    case SESSION_CRASH: // stop!!! crashing. etc.
        break;
    default:
        break;
    }

    // send answer
    AudiclePak answer;
    answer.type = NOTIFY_ANSWER;
    answer.user = packet->user;
    strcpy( answer.server, "" );
    answer.alloc( 80 );
    answer.length = 80;
    strcpy( answer.data, "accept" );
    packet->sender->send( &answer );

    return TRUE;
}


void CoAudicle::getip()
{
    // get our hostname
    char hostname[256];
    gethostname( hostname, 256 );
    // get the hostend
    struct hostent * host = gethostbyname( hostname );
    // print it
    sprintf( hostname, "%i.%i.%i.%i", (host->h_addr_list[0][0]+256)%256, (host->h_addr_list[0][1]+256)%256, (host->h_addr_list[0][2]+256)%256, (host->h_addr_list[0][3]+256)%256 );
    // log
    EM_log( CK_LOG_SYSTEM, "ip %s", hostname );
    the_ip = hostname;
}
