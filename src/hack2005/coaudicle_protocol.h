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
// name: coaudicle_protocol.h
// desc: ...
//
// authors: Ge Wang (gewang@cs.princeton.edu)
//          Perry R. Cook (prc@cs.princeton.edu)
//          Ananya Misra (amisra@cs.princeton.edu)
//          Philip Davidson (philipd@cs.princeton.edu)
// date: Autumn 2005
//-----------------------------------------------------------------------------
#ifndef __COAUDICLE_PROTOCOL_H__
#define __COAUDICLE_PROTOCOL_H__

#include "audicle_net.h"


t_CKBOOL server_handle_packet( AudiclePak * packet );
t_CKBOOL client_handle_packet( AudicleClient * client , AudiclePak * packet );

enum 
{
    SESSION_CONNECTED =0,
    SESSION_MUTE,
    SESSION_UNMUTE,
    SESSION_IDLE,
    SESSION_RETURNED,
    SESSION_DISCONNECTED,
    SESSION_CRASH
};

// message types
enum
{
    // connection
    JOIN_REQUEST,
    JOIN_ANSWER,
    NAME_REQUEST,
    NAME_ANSWER,
    NOTIFY_REQUEST,
    NOTIFY_ANSWER,

    // chat
    CHAT_SEND,
    CHAT_UPDATE,

    // audio
    AUDIO_SEND,

    // session
    HEY_USER_ADD,
    HEY_USER_NOTIFY,
    HEY_WINDOW_ADD,
    HEY_WINDOW_MOD,
    HEY_WINDOW_REMOVE,
    HEY_BUFFER_ADD,
    HEY_BUFFER_EDIT,
    USR_WINDOW_ADD,
    USR_WINDOW_MOD,
    USR_WINDOW_REMOVE,
    USR_BUFFER_ADD,
    USR_BUFFER_EDIT,
    USR_BUFFER_REMOVE,
    
    HEY_SHRED_ADD,
    HEY_SHRED_MOD,
    HEY_SHRED_REMOVE,

    HEY_BINDLE_LIST,
    HEY_BINDLE_ADD,
    HEY_BINDLE_REMOVE,
    HEY_BINDLE_DATA,
    
    // vm
    VM_EXECUTE,
    VM_ANSWER,

    // dude
    REQUEST_DUDE,
    UPDATE_DUDE,
};
/*
t_CKBOOL client_send_join_request( AudicleClient * source, t_CKINT port );
t_CKBOOL client_send_update_info( AudicleClient * source, t_CKUINT user );
t_CKBOOL server_handle_update_info( AudiclePak * pack );
//t_CKBOOL client_send_chat( AudicleClient * source, CoChatMesg * chat );
*/

t_CKBOOL client_send_join_request( AudicleClient * source, t_CKINT port );
t_CKBOOL client_send_update_info( AudicleClient * source, t_CKUINT user );
t_CKBOOL server_handle_update_info( AudiclePak * pack );
t_CKBOOL server_handle_chat_request( AudiclePak * packet );

#endif
