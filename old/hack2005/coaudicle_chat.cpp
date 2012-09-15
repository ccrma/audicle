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
// name: coaudicle_chat.cpp
// desc: ...
//
// authors: Ge Wang (gewang@cs.princeton.edu)
//          Perry R. Cook (prc@cs.princeton.edu)
//          Ananya Misra (amisra@cs.princeton.edu)
//          Philip Davidson (philipd@cs.princeton.edu)
// date: Autumn 2005
//-----------------------------------------------------------------------------
#include "coaudicle_chat.h"
#include "audicle_net.h"
#include "audicle_session_manager.h"
#include "coaudicle_net_session.h"
#include "coaudicle_protocol.h"

//CoChat imp
CoChatImp::CoChatImp() :
m_client_session(NULL)
{ 
    
}

CoChatImp::CoChatImp( ClientSessionImp * session ) { 
    set_session ( session );
}

void 
CoChatImp::send_mesg( CoChatMesg * m ) {
    //send a messages to the packet queue;
    if ( !queue_chat_send_packet(m) ) 
        EM_log( CK_LOG_SYSTEM, "error - message \"%s\" could not be queued",  m->data().c_str() );
}

void
CoChatImp::recv_mesg( CoChatMesg * m ) { 
    //receive a message generated from a parsed packet
    m_messages.push_back( m );
}

void 
CoChatImp::send_mesg_local( CoChatMesg * m ) {
    m_messages.push_back( m );
}

CoServer * 
CoChatImp::server() { 
    return NULL;
}

std::vector < CoChatMesg* >&
CoChatImp::messages() { 
    return m_messages;
}

void 
CoChatImp::set_session ( ClientSessionImp * session ) { 
    m_client_session = session;
    m_messages.clear();
}

t_CKBOOL
CoChatImp::queue_chat_send_packet( CoChatMesg * mesg ) { 
    
    if ( !m_client_session ) { 
        EM_log ( CK_LOG_SYSTEM, "ChatImp: no session found, dropping message" );
        return false;
    }
    
    AudiclePak * p = new AudiclePak();  
    
    //fill in chat info
    p->type = CHAT_SEND;
    
    //session identifier
    m_client_session->pack_header(p);

    pack_chat_mesg(mesg, p);

    return m_client_session->queue_packet( p ) ;
    
}

void
pack_chat_mesg( CoChatMesg * mesg, AudiclePak * p ) { 
    //alloc data segment with our new fun data
    p->length = sizeof(chat_mesg_data);
    p->alloc(p->length);
    
    //dangerous cast!
    chat_mesg_data * chat_data = (chat_mesg_data*) p->data;
    
    //fill in data members
    chat_data->num = mesg->num(); //filled in by server
    chat_data->timestamp = mesg->time(); //filled in by server
    strncpy ( chat_data->user, mesg->user().c_str(), 32 );
    chat_data->user[31] = '\0';
    strncpy ( chat_data->data, mesg->data().c_str(), 256 );
    chat_data->user[255] = '\0';
    
    //hton relevant data // CHECK
    chat_data->num = htonl( chat_data->num );
    chat_data->timestamp = htonl( (unsigned long)chat_data->timestamp );  
}

t_CKBOOL
CoChatImp::queue_packet( AudiclePak * p ) { 
    if ( !m_client_session ) { 
        EM_log ( CK_LOG_SYSTEM, "ChatImp: no session found, dropping message" );
        return false;
    }
    return m_client_session->queue_packet ( p );
    
}


CoChatMesg * 
CoChatImp::message( int i ) { 
    return m_messages[i];
}

//CoChatMesg implementation

CoChatMesgImp::CoChatMesgImp ( std::string user, 
                               std::string data, 
                               t_CKTIME time, 
                               t_CKUINT num ) 
{ 
    m_user = user;
    m_data = data;
    m_time = time;
    m_num  = num;
}

CoChatMesgImp::CoChatMesgImp ( AudiclePak * p ) { 
    
    chat_mesg_data chat_data = *((chat_mesg_data *)p->data);
    
    chat_data.num = ntohl( chat_data.num );
    chat_data.timestamp = ntohl( (unsigned long)chat_data.timestamp );
    
    m_data = chat_data.data;
    m_user = chat_data.user;
    m_num  = chat_data.num;
    m_time = chat_data.timestamp;
    
}

void 
CoChatMesgImp::pack( AudiclePak * p ) { 
    
    //alloc data segment with our new fun data
    p->length = sizeof(chat_mesg_data);
    p->alloc(p->length);
    
    //dangerous cast!
    chat_mesg_data * chat_data = (chat_mesg_data*) p->data;
    
    //fill in data members
    chat_data->num = 0; //filled in by server
    chat_data->timestamp = 0.0; //filled in by server
    strncpy ( chat_data->user, m_user.c_str(), 32 );
    chat_data->user[31] = '\0';
    strncpy ( chat_data->data, m_data.c_str(), 256 );
    chat_data->user[255] = '\0';
    
    //hton relevant data CHECK
    chat_data->num = htonl( chat_data->num );
    chat_data->timestamp = htonl( (unsigned long)chat_data->timestamp );
    
}


        
t_CKUINT 
CoChatMesgImp::num() { 
    return m_num; 
}

t_CKTIME
CoChatMesgImp::time() { 
    return m_time; 
}

std::string 
CoChatMesgImp::user() { 
    return m_user; 
}

std::string
CoChatMesgImp::data(){ 
    return m_data; 
}
