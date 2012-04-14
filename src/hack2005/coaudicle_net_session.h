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
// name: coaudicle_network.h
// desc: network structs for audicle
//
// authors: Ge Wang (gewang@cs.princeton.edu)
//          Perry R. Cook (prc@cs.princeton.edu)
//          Philip Davidson (philipd@cs.princeton.edu)
//          Ananya Misra (amisra@cs.princeton.edu)
// date: Autumn 2004
//-----------------------------------------------------------------------------
#ifndef __COAUDICLE_NET_SESSION_H__
#define __COAUDICLE_NET_SESSION_H__

#include "coaudicle.h"

#include "audicle_text_buffer.h"
#include "coaudicle_protocol.h"

/*
enum chuck_packet_type { 
        
};

struct 
coaudicle_packet { 

};
*/

//maybe we do need a net_session.cpp
//with pack / unpack functions for the
//higher level data?

void swap4byteregion( t_CKUINT * data, t_CKUINT size );


enum user_mesg { 
    USER_ADD, USER_NOTIFY, USER_REMOVE 
};


struct
session_data { 
    t_CKUINT        serverid; 
    t_CKUINT        state;
    char            name[32];   
};

struct chat_mesg_data { 
    char user[32];
    t_CKTIME timestamp;
    t_CKUINT num;
    char data[256];
};

enum win_mod_op { 
    NOOP = 0,
    MSG_WIN_TOP, MSG_WIN_SHAPE, 
    MSG_BUFFER_MAKE_CURRENT, MSG_BUF_SCROLL
};

struct 
window_mod_data { 
    win_mod_op      op; 
    t_CKFLOAT       width;
    t_CKFLOAT       height;
    t_CKFLOAT       posx;
    t_CKFLOAT       posy;
    t_CKUINT        cur_buffer;
    t_CKUINT        buffer_id; //buffer
};


struct
buffer_edit_data_header {
    t_CKUINT data_size; //total packet size ( including header ) 
    t_CKUINT buffer_id; //unique buffer id per user per server 
    
    //XXX WARNING ASSUMPTION
    //t_CKUINT window_id; //we don't need this if buffer id's are unique-to-user
    
    //t_CKUINT seq_num; //no, we don't need these ( i don't think )
    // span of original text;
    TextLoc from_begin;
    TextLoc from_end;
    t_CKUINT from_span_length; //including NULL term
    //XXX   --these could be offsets, y'know, since data size will verify the rest
    // span of replacement text;
    TextLoc to_begin;
    TextLoc to_end;
    t_CKUINT to_span_length; //including NULL term
    void swap_me();
};




struct
shred_data { 
    t_CKUINT id; // process id
    t_CKUINT buffer_id; // buffer that spawned
    t_CKTIME shred_start;
    t_CKUINT shred_state;
};

#endif // __COAUDICLE_NET_SESSION_H__
