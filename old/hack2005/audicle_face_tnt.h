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
// name: audicle_face_tnt.h
// desc: interface for audicle face TnT
//
// authors: Ge Wang (gewang@cs.princeton.edu)
//          Perry R. Cook (prc@cs.princeton.edu)
//          Ananya Misra (amisra@cs.princeton.edu)
//          Philip Davidson (philipd@cs.princeton.edu)
// date: 2/16/2004
//-----------------------------------------------------------------------------
#ifndef __AUDICLE_FACE_TNT_H__
#define __AUDICLE_FACE_TNT_H__

#include "audicle_def.h"
#include "audicle_face.h"
#include "chuck_stats.h"




//-----------------------------------------------------------------------------
// name: class AudicleFaceTnT
// desc: ...
//-----------------------------------------------------------------------------
class AudicleFaceTnT : public AudicleFace
{
public:
    AudicleFaceTnT( );
    virtual ~AudicleFaceTnT( );

public:
    virtual t_CKBOOL init();
    virtual t_CKBOOL destroy();

public:
    virtual void render_pre( );
    virtual void render_post( );
    virtual t_CKUINT render( void * data );
    virtual t_CKUINT render_timing_history();
    virtual t_CKUINT render_timing_grid();
    virtual t_CKUINT render_shred( Shred_Stat * stat );
    virtual t_CKUINT render_shred_nested( Shred_Stat * stat, int deep );

    virtual t_CKUINT render_cycle_heatmap( );
    virtual t_CKUINT render_cycle_shred( Shred_Stat * stat, float w, float h );
    virtual t_CKUINT render_cycle_shred_group( vector< Shred_Stat * >& shreds, int begin, int end, float w, float h  );

    virtual t_CKUINT fetch_shred_activations( Shred_Stat * stat );
    virtual t_CKUINT fetch_group_cycles ( Shred_Stat * stat );
    virtual t_CKUINT check_active_stat( Shred_Time * time );

    virtual t_CKUINT recalculate_scaling_factors();

    virtual void render_view( );
    virtual t_CKUINT on_activate( );
    virtual t_CKUINT on_deactivate( t_CKDUR duration = 0.0 );
    virtual t_CKUINT on_event( const AudicleEvent & event );

public:
    void set_view( int w, int h );

protected:

    t_CKBOOL   m_init;
    t_CKTIME   m_now;
    t_CKFLOAT  m_srate;
    t_CKFLOAT  m_aspt;
    t_CKFLOAT  m_rotx;
    t_CKFLOAT  m_roty;
    InputState inp;

//time window
protected:
    t_CKDUR    m_window;
    t_CKDUR    m_window_target;
    t_CKTIME   m_earliest;
    t_CKUINT   m_wrapmode;

//op scales
protected:
    t_CKUINT   m_max;
    t_CKFLOAT  m_max_drift;
    t_CKFLOAT  m_mheight; 
    t_CKBOOL   m_log_scale;

//keeping everything on screen 
protected:

    enum { FIT_ACTIVE=0, LIST_ACTIVE, LIST_ALL, FIT_ALL, FIT_NESTED, FIT_NTYPES };
    enum { SHOW_TIMING_HISTORY=0, SHOW_CYCLE_HEATMAP_CUMULATIVE, SHOW_CYCLE_HEATMAP_RUNNING , SHOW_NTYPES };

    t_CKFLOAT  m_item_height;
    t_CKFLOAT  m_item_padding;
    t_CKFLOAT  m_label_height;
    t_CKFLOAT  m_item_spacing;
    t_CKBOOL   m_drawing_active_set;
    t_CKFLOAT  m_drawing_scale;

    t_CKUINT   m_fit_mode;
    t_CKUINT   m_show_mode;
    t_CKFLOAT  m_active_accum;
    t_CKFLOAT  m_done_accum;

    t_CKFLOAT  m_active_scale;
    t_CKFLOAT  m_done_scale;

protected:
    Shred_Stat * m_current_shred;
    vector<Shred_Stat *> m_shreds;
    map<Shred_Stat *, Shred_Stat *> m_done;
    vector<Shred_Activation> m_activations;
};




#endif
