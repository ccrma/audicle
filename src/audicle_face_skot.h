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
// name: audicle_face_skot.h
// desc: interface for audicle face slot machine
//
// authors: Ge Wang (gewang@cs.princeton.edu)
//          Perry R. Cook (prc@cs.princeton.edu)
//          Ananya Misra (amisra@cs.princeton.edu)
//          Philip Davidson (philipd@cs.princeton.edu)
//          Scott Smallwood (skot@princeton.edu)
// date: 2/16/2004
//-----------------------------------------------------------------------------
#ifndef __AUDICLE_FACE_SKOT_H__
#define __AUDICLE_FACE_SKOT_H__

#include "audicle_def.h"
#include "audicle_face.h"
#include "audicle_gfx.h"


// forward declaration
struct Chuck_Event;


// class for Ball
class SlotBall
{
public:
    SlotBall() { number = 0; xid = 0; size = 1.0f; size2 = 1.0f; radius = .5f; color = NULL; angle = 0.0f; satellites = NULL; num_satellites = 0; }
    virtual ~SlotBall() { }

public:
    t_CKUINT number;
    t_CKUINT xid;
    GLfloat size;
    GLfloat size2;
    GLfloat radius;
    GLfloat * color;
    GLfloat angle;
    GLfloat velocity;

    SlotBall ** satellites;
    t_CKUINT num_satellites;
};



//-----------------------------------------------------------------------------
// name: class AudicleFaceFloor
// desc: ...
//-----------------------------------------------------------------------------
class AudicleFaceFloor : public AudicleFace
{
public:
    AudicleFaceFloor( t_CKUINT num = 3 );
    virtual ~AudicleFaceFloor( );

public:
    virtual t_CKBOOL init();
    virtual t_CKBOOL destroy();

public:
    Chuck_Event * event() { return m_event; }
    void set_credits( t_CKINT n );
    void set_message( const string & msg );
    void set_bgcolor( t_CKFLOAT r, t_CKFLOAT g, t_CKFLOAT b );
    void set_angle( t_CKFLOAT r, t_CKFLOAT inc );
    void set_rotate( t_CKFLOAT r );
    void set_translate( t_CKFLOAT r, t_CKFLOAT inc );
    virtual void render_pre( );
    virtual void render_post( );
    void render_ball( SlotBall * ball );
    void render_panel( );
    void render_credits( );
    void render_message( );
    virtual t_CKUINT render( void * data );
    virtual void render_view( );
    virtual t_CKUINT on_activate( );
    virtual t_CKUINT on_deactivate( t_CKDUR duration = 0.0 );
    virtual t_CKUINT on_event( const AudicleEvent & event );

public:
    void set_view( int w, int h );

public:
    t_CKBOOL m_init;
    
    // number of credits
    t_CKINT m_credits;
    // update credits;
    t_CKBOOL m_update_credits;
    // trigger
    Chuck_Event * m_event;
    // trigger
    SlotBall ** m_triggers;
    // size
    t_CKUINT m_num_triggers;
    // state
    t_CKBOOL m_state;
    // last trigger hit
    t_CKINT m_last_hit;
    // color
    GLfloat m_bgcolor[4];
    // glu
    GLUquadricObj * m_quadric;
    // message
    char m_message[1024];
    // rotate
    GLfloat m_r;
    // angle
    GLfloat m_a;
    GLfloat m_a_target;
    GLfloat m_a_inc;
    // translate
    GLfloat m_t;
    GLfloat m_t_target;
    GLfloat m_t_inc;
};




#endif
