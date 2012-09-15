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
// name: audicle_face_groove.h
// desc: interface for audicle face groove
//
// authors: Ge Wang (gewang@cs.princeton.edu)
//          Perry R. Cook (prc@cs.princeton.edu)
//          Ananya Misra (amisra@cs.princeton.edu)
//          Philip Davidson (philipd@cs.princeton.edu)
// date: 2/16/2004
//-----------------------------------------------------------------------------
#ifndef __AUDICLE_FACE_GROOVE_H__
#define __AUDICLE_FACE_GROOVE_H__

#include "audicle_def.h"
#include "audicle_face.h"
#include "audicle_gfx.h"




//-----------------------------------------------------------------------------
// name: struct AntiBox
// desc: ...
//-----------------------------------------------------------------------------
struct AntiBox
{
    // opengl id
    t_CKUINT xid;
    // holds
    t_CKINT value;
    // mode ( 0 normal, 1 selected )
    t_CKUINT mode;
    // glow state
    t_CKFLOAT glow;

    // constructor
    AntiBox( t_CKUINT _id = 0, t_CKUINT _value = 0,
             t_CKUINT _mode = 0, t_CKFLOAT _glow = 0.0 )
    { xid = _id; value = _value; mode = _mode; glow = _glow; }
};




//-----------------------------------------------------------------------------
// name: struct AntiPane
// desc: ...
//-----------------------------------------------------------------------------
struct AntiPane
{
    // width
    t_CKUINT width;
    // height
    t_CKUINT height;
    // uh
    t_CKUINT which;
    // matrix of boxes
    AntiBox ** matrix;
    // for chuck
    t_CKINT SELF;

    // constructor
    AntiPane() { width = 0; height = 0; which = 0; matrix = NULL; SELF = 0; }
    // destructor
    ~AntiPane() { cleanup(); }
    // initialize
    t_CKBOOL init( t_CKUINT w, t_CKUINT h );
    // cleanup
    void cleanup( );
    // clear
    void clear( ) { if( matrix ) {
        for( t_CKUINT i = 0; i < width*height; i++ )
            matrix[i]->mode = 0;
        }
    }
    // set
    void reset( t_CKINT x ) { if( matrix ) {
        for( t_CKUINT i = 0; i < width*height; i++ )
            matrix[i]->value = x;
        }
    }
};




//-----------------------------------------------------------------------------
// name: class AudicleFaceGroove
// desc: ...
//-----------------------------------------------------------------------------
class AudicleFaceGroove : public AudicleFace
{
public:
    AudicleFaceGroove( );
    virtual ~AudicleFaceGroove( );

public:
    virtual t_CKBOOL init();
    virtual t_CKBOOL destroy();

public:
    virtual void render_pre( );
    virtual void render_post( );
    void render_box( AntiBox * box );
    void render_pane( );
    void render_panel( );
    virtual t_CKUINT render( void * data );
    virtual void render_view( );
    virtual t_CKUINT on_activate( );
    virtual t_CKUINT on_deactivate( t_CKDUR duration = 0.0 );
    virtual t_CKUINT on_event( const AudicleEvent & event );

public:
    void set_view( int w, int h );

public:
    t_CKBOOL m_init;

    // selected number
    t_CKUINT m_selected;
    // pane
    AntiPane * m_pane;
    // panel
    AntiPane * m_panel;
};




#endif
