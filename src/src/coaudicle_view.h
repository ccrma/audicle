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
// name: coaudicle_view.h
// desc: ...
//
// authors: Philip Davidson (philipd@cs.princeton.edu)
//          Ge Wang (gewang@cs.princeton.edu)
//          Perry R. Cook (prc@cs.princeton.edu)
//          Ananya Misra (amisra@cs.princeton.edu)
// date: Autumn 2005
//-----------------------------------------------------------------------------
#ifndef __COAUDICLE_VIEW_H_INCLUDED__
#define __COAUDICLE_VIEW_H_INCLUDED__

#include "coaudicle.h"
#include "coaudicle_avatar.h"
#include "audicle_ui_console.h"
#include "coaudicle_chat.h"



class CoCamera
{
public: 
    CoCamera();
    
    void setProjection();
    void setModelView();
    void follow( CoAvatar * co );
    
protected: 
    
    CoAvatar * m_following;

    double following_hang;
    double following_vang;
    double following_dist; 
    
    Point3D pos;
    Point3D at;
    Point3D up;

    double angle;
    double aspect;
    double near_limit;
    double far_limit;

    double m_scale;
};


class ChatContent;

class ChatWindow : public DisplayWindow
{
public:
    ChatWindow();
    void set_chat( CoChat * chat );
    void initShapes();
    void reshape(); //setup the window elements
    void drawUILayer(); 
    void drawScrollBars();
    void clean();
};


class ChatContent : public ShellContent
{
public:
    ChatContent();
    CoChat* chat();
    void setChat(CoChat* chat );
    void send_msg( CoChatMesg * msg );
    void processLine();
    void drawPromptLine ( TextLine & s );
    void sync_chat();
    virtual const char * title();  
    virtual const char * mesg();   
    virtual const char * posMesg();

protected:
    CoChat * m_chat; 
    t_CKUINT m_chat_last_size;
};


class CoChatRenderer 
{
public:
    CoChatRenderer();
    ~CoChatRenderer();
    void render();
    
    virtual void handleEvent ( const InputEvent &e );
    virtual void handleKey ( const InputEvent &e ) ;
    virtual void handleSpec( const InputEvent &e ) ;
    virtual void handleMouse( const InputEvent &e ) ;
    virtual void handleMotion( const InputEvent &e ) ;  
    
    void set_chat( CoChat * chat );
    void move(Point3D pt);
    void setdir( Vec3D dir );
    void resize(double w, double h);
    void setAngle( double angle ) ;

protected:
    Point3D  m_pos;
    Vec3D    m_dir; 
    double   m_scale;
    ChatWindow * window;
    ChatContent * content;    
};


class CoSessionRendererImp : public CoSessionRenderer\
{
public: 
    CoSessionRendererImp();

    CoSession*  session() { return m_session; }
    void set_session( CoSession * session ); 
    
    virtual void handleEvent ( const InputEvent &e );
    virtual void handleKey ( const InputEvent &e );
    virtual void handleSpec( const InputEvent &e );
    virtual void handleMouse( const InputEvent &e );
    virtual void handleMotion( const InputEvent &e );   

    virtual void render();

    void render_pre();
    void render_post();

    virtual void render_session();
    void render_default();
    
    //global
    virtual void render_room();
    void render_chat();
    void render_shreds();
    void render_resources();
    void render_control();
    
    //per user(session)
    virtual void render_user(CoSession* s);
    void render_character(CoSession* s);
    void render_windows(CoSession* s);
    void render_portal(CoSession* s);
    void render_bindle(CoSession* s);
    void render_halo(CoSession* s);

    void set_current_time( t_CKFLOAT time ) { last = time; dtime = 0; }
    CoAvatar * avatar() { return m_usr_avatar; }

protected: 

    CoSession * m_session;
    std::vector<CoAvatar *> m_avatars;

    CoChatRenderer * m_chat_render;
    CoCamera * m_camera;

    CoChat * m_chat;
    CoAvatar * m_usr_avatar;
    double dtime; 
    double last;
};


class DemoRenderImp : public CoSessionRendererImp
{
public:
    DemoRenderImp();
    std::vector < Agent * > agents; 
    t_CKBOOL add_agent();
    t_CKBOOL add_agent( Agent * );
    t_CKBOOL rem_agent();
    t_CKBOOL rem_agent( Agent * );
    void do_move( char a);
    void copy_move ( float prob );
    void copy_some ( float prob );
    Agent * get_leader();
    Agent * m_leader;

public:
    t_CKBOOL show_army;
    t_CKBOOL show_character;
    t_CKBOOL show_windows;
    t_CKBOOL show_bindle;
    t_CKBOOL show_portal;
    t_CKBOOL show_chat;
    t_CKBOOL show_shreds;
    t_CKBOOL show_resources;
    t_CKBOOL show_controls;
    t_CKBOOL show_halo;
    virtual void render_session();
};  


#endif // __COAUDICLE_VIEW_H_INCLUDED__
