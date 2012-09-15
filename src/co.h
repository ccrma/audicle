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
// name: co.h
// desc: ...
//
// authors: Ge Wang (gewang@cs.princeton.edu)
//          Perry R. Cook (prc@cs.princeton.edu)
//          Ananya Misra (amisra@cs.princeton.edu)
//          Philip Davidson (philipd@cs.princeton.edu)
// date: Autumn 2004
//-----------------------------------------------------------------------------
#ifndef __CO_H__
#define __CO_H__

#include "audicle_text_buffer.h"
#include "audicle_net.h"
#include "audicle_geometry.h"
#include "coaudicle_protocol.h"

class CoCoSession;
class CoCoChat;
struct chat_mesg_data;


// server
class CoCoServer
{
public:
    CoCoServer();
    ~CoCoServer();

    // initialize server-side
    void initialize();

    // add a new session
    t_CKBOOL add( CoCoSession * session );
    // remove a session
    t_CKBOOL remove( CoCoSession * session );
    // get a session by id
    CoCoSession * get( t_CKUINT id );

    // get the chat object
    CoCoChat * chat();
    // get vector of sessions
    void get_sessions( std::vector < CoCoSession * > & out );
    // get resources
    // std::vector < CoCoResource * > & resources( int i );

    // broadcast
    void broadcast( AudiclePak * packet );

public:
    std::map<t_CKUINT, CoCoSession *> m_sessions;
    CoCoChat * m_chat;
};


class CoCoBuffer
{
    // text buffer
    TextBuffer * m_text_buffer;
    // text edits
    std::vector<TextEdit *> m_text_edits;
};


class CoCoWindow
{
public:
    std::vector<CoCoBuffer *> m_buffers;
    t_CKUINT m_top_buffer;

    // position
    Point3D m_location;
    // size
    Point3D m_dimension;
    // cursor position
    TextLoc m_cursor_location;
    // highlighted region
    TextSpan m_selection;
};


class CoCoWindowManager
{
public:
    std::vector<CoCoWindow *> m_windows;
    t_CKUINT m_top_window;
};


class CoCoBindle
{
public:
    std::vector<t_CKUINT> m_buffers;
};


class CoCoSession
{
public:
    // user id on the server
    t_CKUINT m_user_id;
    // user name
    std::string m_user_name;
        // host name
        std::string m_host_name;

    // window manager
    CoCoWindowManager m_wm;
    // state
    t_CKUINT m_state;
    // bindle
    CoCoBindle m_bindle;
    //client ip address
    std::string m_ip;
public:
    // client request socket (not used)
    SockPack * m_request_socket;
    // client update socket
    SockPack * m_update_socket;
};


class CoCoUserState
{
public:
    // location
    Point3D m_location;
    // orientation
    Vec3D m_orientation;
    // color
    Color4D m_color;
    // portal location
    Point3D m_home;
    // message
    char m_msg[64];
};


class CoCoChat
{
public:
    // add a new message
    void add( chat_mesg_data * mesg );

public:
    std::vector<chat_mesg_data *> m_msgs;

protected:
    // send
    void send( chat_mesg_data * mesg );
};

#endif
