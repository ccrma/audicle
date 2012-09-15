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
// name: coaudicle_chat.h
// desc: ...
//
// authors: Ge Wang (gewang@cs.princeton.edu)
//          Perry R. Cook (prc@cs.princeton.edu)
//          Ananya Misra (amisra@cs.princeton.edu)
//          Philip Davidson (philipd@cs.princeton.edu)
// date: Autumn 2005
//-----------------------------------------------------------------------------
#ifndef __COAUDICLE_CHAT_H_DEFINED__
#define __COAUDICLE_CHAT_H_DEFINED__

#include "coaudicle.h"

class ClientSessionImp; 

class CoChatImp : public CoChat { 
public:
    CoChatImp();
    CoChatImp( ClientSessionImp * session );
    void send_mesg( CoChatMesg *);
    void recv_mesg( CoChatMesg * );
    void send_mesg_local( CoChatMesg *);
    CoServer * server(); 
    std::vector < CoChatMesg* >& messages(); 
    CoChatMesg* message ( int i );
    void set_session( ClientSessionImp * session );
    
    t_CKBOOL queue_chat_send_packet( CoChatMesg * m );
    CoChatMesg* unpack ( AudiclePak *p );
    //interaction with the client
public:
    t_CKBOOL queue_packet( AudiclePak *p );
    
protected:
    std::vector < CoChatMesg* > m_messages; //messages from server
    ClientSessionImp * m_client_session;
};

class CoChatMesgImp : public CoChatMesg { 
protected:
    std::string m_data;
    std::string m_user;
    t_CKUINT m_num;
    t_CKTIME m_time;
    
public:
        
        CoChatMesgImp ( std::string user, std::string data, t_CKTIME time=0.0, t_CKUINT num=0 );
    CoChatMesgImp ( AudiclePak * pack );
    virtual t_CKUINT num();
    virtual t_CKTIME time();
    virtual std::string user();
    virtual std::string data();
    virtual void pack(  AudiclePak * );
};

void pack_chat_mesg ( CoChatMesg * mesg, AudiclePak * p );

#endif // __COAUDICLE_CHAT_H_DEFINED__
