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
// name: co.cpp
// desc: ...
//
// authors: Ge Wang (gewang@cs.princeton.edu)
//          Perry R. Cook (prc@cs.princeton.edu)
//          Ananya Misra (amisra@cs.princeton.edu)
//          Philip Davidson (philipd@cs.princeton.edu)
// date: Autumn 2004
//-----------------------------------------------------------------------------
#include "co.h"
#include "coaudicle.h"
#include "coaudicle_net_session.h"

using namespace std;

// server
CoCoServer::CoCoServer()
{
    m_chat = NULL;
}

CoCoServer::~CoCoServer()
{
    SAFE_DELETE( m_chat ); // I know that's not allowed, but...
}

// initialize server-side
void CoCoServer::initialize()
{
    m_chat = new CoCoChat; 
}

// add a new session
t_CKBOOL CoCoServer::add( CoCoSession * session )
{
    m_sessions[session->m_user_id] = session;
    // TODO: protect with mutex
    return TRUE;
}

// remove a session
t_CKBOOL CoCoServer::remove( CoCoSession * session )
{
    m_sessions.erase( m_sessions.find(session->m_user_id) ); 
    return TRUE;
}


// get a session by id
CoCoSession * CoCoServer::get( t_CKUINT id )
{
    map <t_CKUINT, CoCoSession *>::iterator it = m_sessions.find( id );
    if( it == m_sessions.end() )
    {
        return NULL;
    }
    else
    {
        return (*it).second;
    }
}

// get the chat object
CoCoChat * CoCoServer::chat()
{
    return m_chat;
}

// get vector of sessions
void CoCoServer::get_sessions( std::vector < CoCoSession * > & out )
{
    out.clear();
    map <t_CKUINT, CoCoSession *>::iterator it;
    for( it = m_sessions.begin(); it != m_sessions.end(); it++ )
    {
        out.push_back( (*it).second );
    }
}

// get resources
// std::vector < CoCoResource * > & CoCoServer::resources( int i )
// {
//      return ?;
// }

// add correct id for each session and broadcast
void CoCoServer::broadcast( AudiclePak * packet )
{
    map <t_CKUINT, CoCoSession *>::iterator it;
    for( it = m_sessions.begin(); it != m_sessions.end(); it++ )
    {
        //packet->user = (*it).first;
        (*it).second->m_update_socket->send( packet );
    }
}




//class CoCoChat

// add a new message
void CoCoChat::add( chat_mesg_data * mesg )
{
    if( mesg != NULL )
    {
        // m_msgs.push_back( mesg );
        send( mesg );
    }
}

// send (protected)
void CoCoChat::send( chat_mesg_data * mesg )
{
    AudiclePak packet;
    packet.type = CHAT_UPDATE;
    strcpy( packet.server, "server" );
    packet.time = 0;
    packet.alloc( sizeof( chat_mesg_data ) );
    memcpy( packet.data, mesg, sizeof(chat_mesg_data) );
    packet.length = sizeof( chat_mesg_data );
    
    EM_log( CK_LOG_INFO, "broadcasting chat message %s:%s",
        mesg->user, mesg->data );
    CoAudicle::instance()->coco_server.broadcast( &packet );
}
