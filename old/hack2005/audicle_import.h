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
// name: audicle_import.h
// desc: audicle class library base
//
// authors: Ge Wang (gewang@cs.princeton.edu)
//          Ananya Misra (amisra@cs.princeton.edu)
//          Perry R. Cook (prc@cs.princeton.edu)
//          Philip Davidson (philipd@cs.princeton.edu)
//    date: spring 2005
//-----------------------------------------------------------------------------
#ifndef __AUDICLE_IMPORT_H__
#define __AUDICLE_IMPORT_H__

#include "chuck_def.h"
#include "chuck_dl.h"


// class initialization
t_CKBOOL init_class_dude( Chuck_Env * env );
t_CKBOOL init_class_camera( Chuck_Env * env );
t_CKBOOL init_class_world( Chuck_Env * env );
t_CKBOOL init_class_pane( Chuck_Env * env );
t_CKBOOL init_class_groove( Chuck_Env * env );
t_CKBOOL init_class_floor( Chuck_Env * env );
t_CKBOOL init_class_mouse_pane( Chuck_Env * env );
t_CKBOOL init_class_mouse_mania( Chuck_Env * env );


//-----------------------------------------------------------------------------
// AudicleDude API
//-----------------------------------------------------------------------------
CK_DLL_CTOR( dude_ctor );
CK_DLL_DTOR( dude_dtor );
CK_DLL_MFUN( dude_jump );
CK_DLL_MFUN( dude_headbang );
CK_DLL_MFUN( dude_leanback );
CK_DLL_MFUN( dude_armsup );
CK_DLL_MFUN( dude_pos );


//-----------------------------------------------------------------------------
// AudicleWorld API
//-----------------------------------------------------------------------------
CK_DLL_CTOR( world_ctor );
CK_DLL_DTOR( world_dtor );
CK_DLL_SFUN( world_leader );
CK_DLL_SFUN( world_camera );
CK_DLL_SFUN( world_add );
CK_DLL_SFUN( world_remove );


//-----------------------------------------------------------------------------
// AudicleCamera API
//-----------------------------------------------------------------------------
CK_DLL_CTOR( camera_ctor );
CK_DLL_DTOR( camera_dtor );
CK_DLL_MFUN( camera_eye );
CK_DLL_MFUN( camera_at );
CK_DLL_MFUN( camera_up );


//-----------------------------------------------------------------------------
// AudiclePane API
//-----------------------------------------------------------------------------
CK_DLL_CTOR( pane_ctor );
CK_DLL_DTOR( pane_dtor );
CK_DLL_MFUN( pane_init );
CK_DLL_MFUN( pane_set_glow );
CK_DLL_MFUN( pane_get_value );
CK_DLL_MFUN( pane_get_width );
CK_DLL_MFUN( pane_get_height );


//-----------------------------------------------------------------------------
// AudicleGroove API
//-----------------------------------------------------------------------------
CK_DLL_CTOR( groove_ctor );
CK_DLL_DTOR( groove_dtor );
CK_DLL_SFUN( groove_pane );


//-----------------------------------------------------------------------------
// AudicleFloor API
//-----------------------------------------------------------------------------
CK_DLL_CTOR( floor_ctor );
CK_DLL_DTOR( floor_dtor );
CK_DLL_SFUN( floor_event );
CK_DLL_SFUN( floor_which );
CK_DLL_SFUN( floor_set_credits );
CK_DLL_SFUN( floor_get_credits );
CK_DLL_SFUN( floor_add_credits );
CK_DLL_SFUN( floor_message );
CK_DLL_SFUN( floor_bgcolor );
CK_DLL_SFUN( floor_angle );
CK_DLL_SFUN( floor_rotate );
CK_DLL_SFUN( floor_translate );


//-----------------------------------------------------------------------------
// AudicleMousePane API
//-----------------------------------------------------------------------------
CK_DLL_CTOR( mouse_pane_ctor );
CK_DLL_DTOR( mouse_pane_dtor );
CK_DLL_MFUN( mouse_pane_init );
CK_DLL_MFUN( mouse_pane_set_glow );
CK_DLL_MFUN( mouse_pane_get_value );
CK_DLL_MFUN( mouse_pane_get_width );
CK_DLL_MFUN( mouse_pane_get_height );


//-----------------------------------------------------------------------------
// AudicleMouseMania API
//-----------------------------------------------------------------------------
CK_DLL_CTOR( mouse_mania_ctor );
CK_DLL_DTOR( mouse_mania_dtor );
CK_DLL_SFUN( mouse_mania_pane );
CK_DLL_SFUN( mouse_mania_go );
CK_DLL_SFUN( mouse_mania_get );
CK_DLL_SFUN( mouse_mania_in );
CK_DLL_SFUN( mouse_mania_out );
CK_DLL_SFUN( mouse_mania_add );
CK_DLL_SFUN( mouse_mania_get_num_mice );
CK_DLL_SFUN( mouse_mania_get_volume );
CK_DLL_SFUN( mouse_mania_get_volume2 );




#endif
