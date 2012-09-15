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
// name: audicle_face_shredder.h
// desc: interface for audicle face shredder
//
// authors: Ge Wang (gewang@cs.princeton.edu)
//          Perry R. Cook (prc@cs.princeton.edu)
//          Ananya Misra (amisra@cs.princeton.edu)
//          Philip Davidson (philipd@cs.princeton.edu)
// date: 2/16/2004
//-----------------------------------------------------------------------------
#ifndef __AUDICLE_FACE_SHREDDER_H__
#define __AUDICLE_FACE_SHREDDER_H__

#include "audicle_def.h"
#include "audicle_face.h"

#include <map>
using namespace std;




//-----------------------------------------------------------------------------
// name: class AudicleFaceShredder
// desc: ...
//-----------------------------------------------------------------------------
class AudicleFaceShredder : public AudicleFace
{
public:
    AudicleFaceShredder( );
    virtual ~AudicleFaceShredder( );

public:
    virtual t_CKBOOL init();
    virtual t_CKBOOL destroy();

public:
    virtual void render_pre( );
    virtual void render_post( );
    virtual t_CKUINT render( void * data );
    virtual void render_view( );
    virtual t_CKUINT on_activate( );
    virtual t_CKUINT on_deactivate( t_CKDUR duration = 0.0 );
    virtual t_CKUINT on_event( const AudicleEvent & event );

public:
    void render_1( t_CKFLOAT delta );
    void render_2( t_CKFLOAT delta );
    void render_3( t_CKFLOAT delta );
    static void initialize_stat_data ( Shred_Stat * stat );
    t_CKUINT which;
    t_CKUINT m_b1_id;
    t_CKBOOL sphere_down;
    
    t_CKUINT on_event_1( const AudicleEvent & event );
    t_CKUINT on_event_2( const AudicleEvent & event );
    t_CKUINT on_event_3( const AudicleEvent & event );

public:
    void set_view( int w, int h );

protected:
    t_CKBOOL m_init;
    t_CKFLOAT m_last_time;
    t_CKFLOAT m_time;

protected:
    t_CKUINT m_test_id;
    Point2D m_test_pt;
    
protected:
    vector<Shred_Stat *> m_stats;
    vector<Shred_Stat *> m_happy;
    map<Shred_Stat *, Shred_Stat *> m_done;
    map<t_CKUINT, Shred_Data *> m_map;
};


#endif
