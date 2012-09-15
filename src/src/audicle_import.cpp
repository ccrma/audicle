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
// name: audicle_import.cpp
// desc: audicle class library base
//
// authors: Ge Wang (gewang@cs.princeton.edu)
//          Ananya Misra (amisra@cs.princeton.edu)
//          Perry R. Cook (prc@cs.princeton.edu)
//          Philip Davidson (philipd@cs.princeton.edu)
//    date: spring 2005
//-----------------------------------------------------------------------------
#include "audicle_import.h"
#include "audicle_elcidua.h"
#include "audicle_face_groove.h"
#include "audicle_face_skot.h"
#include "audicle_face_mouse.h"
#include "chuck_type.h"
#include "chuck_errmsg.h"
#include "chuck_instr.h"
#include "coaudicle_avatar.h"


static t_CKUINT dude_offset_data = 0;
static Chuck_Type * g_dude_type = NULL;
//-----------------------------------------------------------------------------
// name: init_class_dude()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL init_class_dude( Chuck_Env * env )
{
    Chuck_DL_Func * func = NULL;
    Chuck_Value * value = NULL;

    // log
    EM_log( CK_LOG_INFO, "class 'AudicleDude'" );

    // import
    if( !type_engine_import_class_begin( env, "AudicleDude", "Object",
                                         env->global(), dude_ctor ) )
        return FALSE;

    // add member
    dude_offset_data = type_engine_import_mvar( env, "int", "@data", FALSE );
    if( dude_offset_data == CK_INVALID_OFFSET ) goto error;

    // add jump()
    func = make_new_mfun( "void", "jump", dude_jump );
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add headbang()
    func = make_new_mfun( "void", "headbang", dude_headbang );
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add leanback()
    func = make_new_mfun( "void", "leanback", dude_leanback );
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add armsup()
    func = make_new_mfun( "int", "armsup", dude_armsup );
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add setpos()
    func = make_new_mfun( "void", "pos", dude_pos );
    func->add_arg( "float", "x" );
    func->add_arg( "float", "y" );
    func->add_arg( "float", "z" );
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // end the class import
    type_engine_import_class_end( env );

    // keep the type around
    g_dude_type = type_engine_find_type( env, new_id_list("AudicleDude", 0 ) );

    return TRUE;

error:

    // end the class import
    type_engine_import_class_end( env );
    
    return FALSE;
}


//-----------------------------------------------------------------------------
// name: init_class_world()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL init_class_world( Chuck_Env * env )
{
    Chuck_DL_Func * func = NULL;
    Chuck_Value * value = NULL;

    // log
    EM_log( CK_LOG_INFO, "class 'AudicleWorld'" );

    // import
    if( !type_engine_import_class_begin( env, "AudicleWorld", "Object",
                                         env->global(), world_ctor ) )
        return FALSE;

    // add leader()
    func = make_new_sfun( "AudicleDude", "leader", world_leader );
    if( !type_engine_import_sfun( env, func ) ) goto error;

    // add add()
    func = make_new_sfun( "int", "add", world_add );
    func->add_arg( "AudicleDude", "dude" );
    if( !type_engine_import_sfun( env, func ) ) goto error;

    // add remove()
    func = make_new_sfun( "int", "remove", world_remove );
    func->add_arg( "AudicleDude", "dude" );
    if( !type_engine_import_sfun( env, func ) ) goto error;

    // wrap up
    type_engine_import_class_end( env );

    return TRUE;

error:

    // wrap up
    type_engine_import_class_end( env );

    return FALSE;
}


static t_CKUINT pane_offset_data = 0;
static Chuck_Type * g_pane_type = NULL;
//-----------------------------------------------------------------------------
// name: init_class_pane()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL init_class_pane( Chuck_Env * env )
{
    Chuck_DL_Func * func = NULL;
    Chuck_Value * value = NULL;

    // log
    EM_log( CK_LOG_INFO, "class 'AudiclePane'" );

    // import
    if( !type_engine_import_class_begin( env, "AudiclePane", "Object",
                                         env->global(), pane_ctor ) )
        return FALSE;

    // add member
    pane_offset_data = type_engine_import_mvar( env, "int", "@data", FALSE );
    if( pane_offset_data == CK_INVALID_OFFSET ) goto error;

    // add width()
    func = make_new_mfun( "int", "width", pane_get_width );
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add height()
    func = make_new_mfun( "int", "height", pane_get_height );
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add setglow()
    func = make_new_mfun( "void", "setglow", pane_set_glow );
    func->add_arg( "int", "x" );
    func->add_arg( "int", "y" );
    func->add_arg( "float", "val" );
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add getvalue()
    func = make_new_mfun( "int", "getvalue", pane_get_value );
    func->add_arg( "int", "x" );
    func->add_arg( "int", "y" );
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // end the class import
    type_engine_import_class_end( env );

    // keep the type around
    g_pane_type = type_engine_find_type( env, new_id_list("AudiclePane", 0 ) );

    return TRUE;

error:

    // end the class import
    type_engine_import_class_end( env );
    
    return FALSE;
}


//-----------------------------------------------------------------------------
// name: init_class_groove()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL init_class_groove( Chuck_Env * env )
{
    Chuck_DL_Func * func = NULL;
    Chuck_Value * value = NULL;

    // log
    EM_log( CK_LOG_INFO, "class 'AudicleGroove'" );

    // import
    if( !type_engine_import_class_begin( env, "AudicleGroove", "Object",
                                         env->global(), groove_ctor ) )
        return FALSE;

    // add pane()
    func = make_new_sfun( "AudiclePane", "pane", groove_pane );
    if( !type_engine_import_sfun( env, func ) ) goto error;

    // wrap up
    type_engine_import_class_end( env );

    return TRUE;

error:

    // wrap up
    type_engine_import_class_end( env );

    return FALSE;
}


//-----------------------------------------------------------------------------
// name: init_class_floor()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL init_class_floor( Chuck_Env * env )
{
    Chuck_DL_Func * func = NULL;
    Chuck_Value * value = NULL;

    // log
    EM_log( CK_LOG_INFO, "class 'AudicleFloor'" );

    // import
    if( !type_engine_import_class_begin( env, "AudicleFloor", "Object",
                                         env->global(), floor_ctor ) )
        return FALSE;

    // add event()
    func = make_new_sfun( "Event", "event", floor_event );
    if( !type_engine_import_sfun( env, func ) ) goto error;

    // add which()
    func = make_new_sfun( "int", "which", floor_which );
    if( !type_engine_import_sfun( env, func ) ) goto error;

    // add credits()
    func = make_new_sfun( "int", "credits", floor_set_credits );
    func->add_arg( "int", "val" );
    if( !type_engine_import_sfun( env, func ) ) goto error;
    // add credits()
    func = make_new_sfun( "int", "credits", floor_get_credits );
    if( !type_engine_import_sfun( env, func ) ) goto error;
    // add addCredits;
    func = make_new_sfun( "int", "addCredits", floor_add_credits );
    func->add_arg( "int", "val" );
    if( !type_engine_import_sfun( env, func ) ) goto error;

    // add message()
    func = make_new_sfun( "void", "message", floor_message );
    func->add_arg( "string", "msg" );
    if( !type_engine_import_sfun( env, func ) ) goto error;

    // add bgcolor()
    func = make_new_sfun( "int", "bgcolor", floor_bgcolor );
    func->add_arg( "float", "r" );
    func->add_arg( "float", "g" );
    func->add_arg( "float", "b" );
    if( !type_engine_import_sfun( env, func ) ) goto error;

    // add rotate()
    func = make_new_sfun( "void", "rotate", floor_rotate );
    func->add_arg( "float", "r" );
    if( !type_engine_import_sfun( env, func ) ) goto error;

    // add angle()
    func = make_new_sfun( "void", "angle", floor_angle );
    func->add_arg( "float", "r" );
    func->add_arg( "float", "inc" );
    if( !type_engine_import_sfun( env, func ) ) goto error;

    // add translate()
    func = make_new_sfun( "void", "translate", floor_translate );
    func->add_arg( "float", "r" );
    func->add_arg( "float", "inc" );
    if( !type_engine_import_sfun( env, func ) ) goto error;

    // wrap up
    type_engine_import_class_end( env );

    return TRUE;

error:

    // wrap up
    type_engine_import_class_end( env );

    return FALSE;
}


static t_CKUINT mouse_pane_offset_data = 0;
static Chuck_Type * g_mouse_pane_type = NULL;
//-----------------------------------------------------------------------------
// name: init_class_mouse_pane()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL init_class_mouse_pane( Chuck_Env * env )
{
    Chuck_DL_Func * func = NULL;
    Chuck_Value * value = NULL;

    // log
    EM_log( CK_LOG_INFO, "class 'AudicleMousePane'" );

    // import
    if( !type_engine_import_class_begin( env, "AudicleMousePane", "Object",
                                         env->global(), mouse_pane_ctor ) )
        return FALSE;

    // add member
    mouse_pane_offset_data = type_engine_import_mvar( env, "int", "@data", FALSE );
    if( mouse_pane_offset_data == CK_INVALID_OFFSET ) goto error;

    // add width()
    func = make_new_mfun( "int", "width", mouse_pane_get_width );
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add height()
    func = make_new_mfun( "int", "height", mouse_pane_get_height );
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add getvalue()
    func = make_new_mfun( "int", "getvalue", mouse_pane_get_value );
    func->add_arg( "int", "x" );
    func->add_arg( "int", "y" );
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // end the class import
    type_engine_import_class_end( env );

    // keep the type around
    g_mouse_pane_type = type_engine_find_type( env, new_id_list("AudicleMousePane", 0 ) );

    return TRUE;

error:

    // end the class import
    type_engine_import_class_end( env );
    
    return FALSE;
}


//-----------------------------------------------------------------------------
// name: init_class_mouse_mania()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL init_class_mouse_mania( Chuck_Env * env )
{
    Chuck_DL_Func * func = NULL;
    Chuck_Value * value = NULL;

    // log
    EM_log( CK_LOG_INFO, "class 'AudicleMouseMania'" );

    // import
    if( !type_engine_import_class_begin( env, "AudicleMouseMania", "Object",
                                         env->global(), mouse_mania_ctor ) )
        return FALSE;

    // add pane()
    func = make_new_sfun( "AudicleMousePane", "pane", mouse_mania_pane );
    if( !type_engine_import_sfun( env, func ) ) goto error;

    // add go()
    func = make_new_sfun( "void", "go", mouse_mania_go );
    func->add_arg( "float", "duration" );
    if( !type_engine_import_sfun( env, func ) ) goto error;

    // add get_item()
    func = make_new_sfun( "int", "get_item", mouse_mania_get );
    func->add_arg( "int", "index" );
    if( !type_engine_import_sfun( env, func ) ) goto error;

    // add get_teleport_in()
    func = make_new_sfun( "int", "get_teleport_in", mouse_mania_in );
    if( !type_engine_import_sfun( env, func ) ) goto error;

    // add get_teleport_out()
    func = make_new_sfun( "int", "get_teleport_out", mouse_mania_out );
    if( !type_engine_import_sfun( env, func ) ) goto error;

    // add add_mouse()
    func = make_new_sfun( "void", "add_mouse", mouse_mania_add );
    func->add_arg( "int", "from" );
    func->add_arg( "int", "note" );
    if( !type_engine_import_sfun( env, func ) ) goto error;
    
    // add get_num_mice()
    func = make_new_sfun( "int", "get_num_mice", mouse_mania_get_num_mice );
    if( !type_engine_import_sfun( env, func ) ) goto error;
    
    // add get_volume()
    func = make_new_sfun( "float", "get_volume", mouse_mania_get_volume );
    if( !type_engine_import_sfun( env, func ) ) goto error;
    
    // add get_volume2()
    func = make_new_sfun( "float", "get_volume2", mouse_mania_get_volume2 );
    if( !type_engine_import_sfun( env, func ) ) goto error;

    // wrap up
    type_engine_import_class_end( env );

    return TRUE;

error:

    // wrap up
    type_engine_import_class_end( env );

    return FALSE;
}


// AudicleDude implementation

// constructor
CK_DLL_CTOR( dude_ctor )
{
    Agent * agent = new Agent;
    OBJ_MEMBER_INT(SELF, dude_offset_data) = (t_CKINT)agent;
}

// destructor
CK_DLL_DTOR( dude_dtor )
{
    delete (Agent *)OBJ_MEMBER_INT(SELF, dude_offset_data);
    OBJ_MEMBER_INT(SELF, dude_offset_data) = 0;
}

// jump
CK_DLL_MFUN( dude_jump )
{
    Agent * agent = (Agent *)OBJ_MEMBER_INT(SELF, dude_offset_data);
    agent->do_jump();
}

// headbang
CK_DLL_MFUN( dude_headbang )
{
    Agent * agent = (Agent *)OBJ_MEMBER_INT(SELF, dude_offset_data);
    agent->do_head_bang_once();
}

// leanback
CK_DLL_MFUN( dude_leanback )
{
    Agent * agent = (Agent *)OBJ_MEMBER_INT(SELF, dude_offset_data);
    agent->do_lean_back();
}

// armsup
CK_DLL_MFUN( dude_armsup )
{
    Agent * agent = (Agent *)OBJ_MEMBER_INT(SELF, dude_offset_data);
    agent->do_arms_up();
}

// pos
CK_DLL_MFUN( dude_pos )
{
    Agent * agent = (Agent *)OBJ_MEMBER_INT(SELF, dude_offset_data);
    t_CKFLOAT x = GET_NEXT_FLOAT(ARGS);
    t_CKFLOAT y = GET_NEXT_FLOAT(ARGS);
    t_CKFLOAT z = GET_NEXT_FLOAT(ARGS);
    agent->pos = Point3D( x, y, z );
}


// AudicleWorld implementation

extern ElDemoFace * g_el_demo_face;

// constructor
CK_DLL_CTOR( world_ctor )
{
    // nothing
}

// destructor
CK_DLL_DTOR( world_dtor )
{
    // nothing
}

// leader
CK_DLL_SFUN( world_leader )
{
    RETURN->v_int = (t_CKINT)g_el_demo_face->m_demoview->get_leader()->SELF;
    if( RETURN->v_int == 0 )
    {
        Chuck_Object * obj = instantiate_and_initialize_object(
            g_dude_type, NULL );
        Agent * agent = g_el_demo_face->m_demoview->m_leader;
        OBJ_MEMBER_INT(obj, dude_offset_data) = (t_CKINT)agent;
        RETURN->v_int = (t_CKINT)obj;
        g_el_demo_face->m_demoview->m_leader->SELF = (t_CKINT)obj;
    }
}

// add
CK_DLL_SFUN( world_add )
{
    Chuck_Object * obj = (Chuck_Object *)GET_CK_INT(ARGS);
    RETURN->v_int = g_el_demo_face->m_demoview->add_agent( (Agent *)OBJ_MEMBER_INT(obj, dude_offset_data) );
}

// remove
CK_DLL_SFUN( world_remove )
{
    Chuck_Object * obj = (Chuck_Object *)GET_CK_INT(ARGS);
    RETURN->v_int = g_el_demo_face->m_demoview->rem_agent( (Agent *)OBJ_MEMBER_INT(obj, dude_offset_data) );
}


// AudiclePane implementation
CK_DLL_CTOR( pane_ctor )
{
    AntiPane * pane = new AntiPane;
    OBJ_MEMBER_INT(SELF, pane_offset_data) = (t_CKINT)pane;
}

// destructor
CK_DLL_DTOR( pane_dtor )
{
    delete (AntiPane *)OBJ_MEMBER_INT(SELF, pane_offset_data);
    OBJ_MEMBER_INT(SELF, pane_offset_data) = 0;
}

// setglow
CK_DLL_MFUN( pane_set_glow )
{
    AntiPane * pane = (AntiPane *)OBJ_MEMBER_INT(SELF, pane_offset_data);
    t_CKINT x = GET_NEXT_INT(ARGS);
    t_CKINT y = GET_NEXT_INT(ARGS);
    t_CKFLOAT val = GET_NEXT_FLOAT(ARGS);
    if( x < 0 || y < 0 ) return;
    if( x < pane->width && y < pane->height )
        pane->matrix[x+pane->width*y]->glow = val;
}

// getvalue
CK_DLL_MFUN( pane_get_value )
{
    AntiPane * pane = (AntiPane *)OBJ_MEMBER_INT(SELF, pane_offset_data);
    t_CKINT x = GET_NEXT_INT(ARGS);
    t_CKINT y = GET_NEXT_INT(ARGS);
    if( x < 0 || y < 0 ) return;
    if( x < pane->width && y < pane->height )
        RETURN->v_int = pane->matrix[x+pane->width*y]->value;
    else
        RETURN->v_int = 0;
}

// width
CK_DLL_MFUN( pane_get_width )
{
    AntiPane * pane = (AntiPane *)OBJ_MEMBER_INT(SELF, pane_offset_data);
    RETURN->v_int = pane->width;
}

// height
CK_DLL_MFUN( pane_get_height )
{
    AntiPane * pane = (AntiPane *)OBJ_MEMBER_INT(SELF, pane_offset_data);
    RETURN->v_int = pane->height;
}


// constructor
CK_DLL_CTOR( groove_ctor )
{
    // nothing
}

// destructor
CK_DLL_DTOR( groove_dtor )
{
    // nothing
}


// should hold groove
extern AudicleFaceGroove * g_face_groove;

// pane
CK_DLL_SFUN( groove_pane )
{
    RETURN->v_int = (t_CKINT)g_face_groove->m_pane->SELF;
    if( RETURN->v_int == 0 )
    {
        Chuck_Object * obj = instantiate_and_initialize_object(
            g_pane_type, NULL );
        AntiPane * pane = g_face_groove->m_pane;
        OBJ_MEMBER_INT(obj, pane_offset_data) = (t_CKINT)pane;
        RETURN->v_int = (t_CKINT)obj;
        g_face_groove->m_pane->SELF = (t_CKINT)obj;
    }
}



// floor
extern AudicleFaceFloor * g_face_floor;

CK_DLL_CTOR( floor_ctor )
{
}

// event
CK_DLL_SFUN( floor_event )
{
    RETURN->v_object = g_face_floor->event();
}

// which
CK_DLL_SFUN( floor_which )
{
    RETURN->v_int = g_face_floor->m_last_hit;
}

// credits
CK_DLL_SFUN( floor_set_credits )
{
    t_CKINT i = GET_NEXT_INT(ARGS);
    g_face_floor->set_credits( i );
    RETURN->v_int = g_face_floor->m_credits;
}

// credits
CK_DLL_SFUN( floor_get_credits )
{
    RETURN->v_int = g_face_floor->m_credits;
}

// addCredits
CK_DLL_SFUN( floor_add_credits )
{
    t_CKINT i = GET_NEXT_INT(ARGS);
    g_face_floor->set_credits( g_face_floor->m_credits + i );
    RETURN->v_int = g_face_floor->m_credits;
}

// message
CK_DLL_SFUN( floor_message )
{
    Chuck_String * s = (Chuck_String *)GET_NEXT_OBJECT(ARGS);
    g_face_floor->set_message( s->str );
}

// bgcolor
CK_DLL_SFUN( floor_bgcolor )
{
    t_CKFLOAT r = GET_NEXT_FLOAT(ARGS);
    t_CKFLOAT g = GET_NEXT_FLOAT(ARGS);
    t_CKFLOAT b = GET_NEXT_FLOAT(ARGS);

    g_face_floor->set_bgcolor( r, g, b );
}

// rotate
CK_DLL_SFUN( floor_rotate )
{
    t_CKFLOAT r = GET_NEXT_FLOAT(ARGS);
    g_face_floor->set_rotate( r );
}

// angle
CK_DLL_SFUN( floor_angle )
{
    t_CKFLOAT r = GET_NEXT_FLOAT(ARGS);
    t_CKFLOAT inc = GET_NEXT_FLOAT(ARGS);
    g_face_floor->set_angle( r, inc );
}

// translate
CK_DLL_SFUN( floor_translate )
{
    t_CKFLOAT r = GET_NEXT_FLOAT(ARGS);
    t_CKFLOAT inc = GET_NEXT_FLOAT(ARGS);
    g_face_floor->set_translate( r, inc );
}


// AudicleMousePane implementation
CK_DLL_CTOR( mouse_pane_ctor )
{
    MousePane * pane = new MousePane;
    OBJ_MEMBER_INT(SELF, mouse_pane_offset_data) = (t_CKINT)pane;
}

// destructor
CK_DLL_DTOR( mouse_pane_dtor )
{
    delete (MousePane *)OBJ_MEMBER_INT(SELF, mouse_pane_offset_data);
    OBJ_MEMBER_INT(SELF, mouse_pane_offset_data) = 0;
}

// setglow
CK_DLL_MFUN( mouse_pane_set_glow )
{
    MousePane * pane = (MousePane *)OBJ_MEMBER_INT(SELF, mouse_pane_offset_data);
    t_CKINT x = GET_NEXT_INT(ARGS);
    t_CKINT y = GET_NEXT_INT(ARGS);
    t_CKFLOAT val = GET_NEXT_FLOAT(ARGS);
    if( x < 0 || y < 0 ) return;
    if( x < pane->width && y < pane->height )
        pane->matrix[x+pane->width*y]->glow = val;
}

// getvalue
CK_DLL_MFUN( mouse_pane_get_value )
{
    MousePane * pane = (MousePane *)OBJ_MEMBER_INT(SELF, mouse_pane_offset_data);
    t_CKINT x = GET_NEXT_INT(ARGS);
    t_CKINT y = GET_NEXT_INT(ARGS);
    if( x < 0 || y < 0 ) return;
    if( x < pane->width && y < pane->height )
        RETURN->v_int = pane->matrix[x+pane->width*y]->value;
    else
        RETURN->v_int = 0;
}

// width
CK_DLL_MFUN( mouse_pane_get_width )
{
    MousePane * pane = (MousePane *)OBJ_MEMBER_INT(SELF, mouse_pane_offset_data);
    RETURN->v_int = pane->width;
}

// height
CK_DLL_MFUN( mouse_pane_get_height )
{
    MousePane * pane = (MousePane *)OBJ_MEMBER_INT(SELF, mouse_pane_offset_data);
    RETURN->v_int = pane->height;
}


// constructor
CK_DLL_CTOR( mouse_mania_ctor )
{
    // nothing
}

// destructor
CK_DLL_DTOR( mouse_mania_dtor )
{
    // nothing
}


// should hold groove
extern AudicleFaceMouse * g_face_mouse;

// pane
CK_DLL_SFUN( mouse_mania_pane )
{
    RETURN->v_int = (t_CKINT)g_face_mouse->m_pane->SELF;
    if( RETURN->v_int == 0 )
    {
        Chuck_Object * obj = instantiate_and_initialize_object(
            g_mouse_pane_type, NULL );
        MousePane * pane = g_face_mouse->m_pane;
        OBJ_MEMBER_INT(obj, mouse_pane_offset_data) = (t_CKINT)pane;
        RETURN->v_int = (t_CKINT)obj;
        g_face_mouse->m_pane->SELF = (t_CKINT)obj;
    }
}

// go
CK_DLL_SFUN( mouse_mania_go )
{
    t_CKFLOAT duration = GET_NEXT_FLOAT(ARGS);
    g_face_mouse->go( duration );
}

// get_item
CK_DLL_SFUN( mouse_mania_get )
{
    t_CKINT index = GET_NEXT_INT(ARGS);
    RETURN->v_int = g_face_mouse->get_item( index );
}

// get_teleport_in
CK_DLL_SFUN( mouse_mania_in )
{
    RETURN->v_int = g_face_mouse->get_teleport_in();
}

// get_teleport_out
CK_DLL_SFUN( mouse_mania_out )
{
    RETURN->v_int = g_face_mouse->get_teleport_out();
}

// add_mouse
CK_DLL_SFUN( mouse_mania_add )
{
    t_CKINT from = GET_NEXT_INT(ARGS);
    t_CKINT note = GET_NEXT_INT(ARGS);
    g_face_mouse->add_mouse( from, note );
}

// get_num_mice
CK_DLL_SFUN( mouse_mania_get_num_mice )
{
    RETURN->v_int = g_face_mouse->get_num_mice();
}

// get_volume
CK_DLL_SFUN( mouse_mania_get_volume )
{
    RETURN->v_float = g_face_mouse->get_volume();
}

// get_volume
CK_DLL_SFUN( mouse_mania_get_volume2 )
{
    RETURN->v_float = g_face_mouse->get_volume2();
}
