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
// name: audicle_face_editor.h
// desc: interface for audicle text editor face
//
// authors: Philip Davidson (philipd@cs.princeton.edu)
//          Ge Wang (gewang@cs.princeton.edu)
//          Perry R. Cook (prc@cs.princeton.edu)
//          Ananya Misra (amisra@cs.princeton.edu)
// date: Autumn 2004
//-----------------------------------------------------------------------------
#ifndef __AUDICLE_FACE_EDITOR_H__
#define __AUDICLE_FACE_EDITOR_H__

#include "audicle_face.h"
#include "audicle_ui_coding.h"


//-----------------------------------------------------------------------------
// name: class AudicleFaceVMSpace
// desc: ...
//-----------------------------------------------------------------------------
class AudicleFaceEditor: public AudicleFace
{

public:
    AudicleFaceEditor ( );
    virtual ~AudicleFaceEditor( );

public:
    WindowManager * wm;
    CodeWindowHandler * wh;

public:
    virtual t_CKBOOL init();
    virtual t_CKBOOL destroy();

public:
    virtual void render_pre( );
    virtual t_CKUINT render( void * data );
    virtual void render_post( );
    virtual void render_view( );
    virtual t_CKUINT on_activate( );
    virtual t_CKUINT on_deactivate( t_CKDUR duration = 0.0 );
    virtual t_CKUINT on_event( const AudicleEvent & event );
    virtual void set_session( CoSession* session );

public:
    void set_view( int w, int h );

protected:
    t_CKBOOL m_init;
};




#endif

