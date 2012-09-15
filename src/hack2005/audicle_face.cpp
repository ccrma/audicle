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
// name: audicle_face.cpp
// desc: interface for audicle face
//
// authors: Ge Wang (gewang@cs.princeton.edu)
//          Perry R. Cook (prc@cs.princeton.edu)
// date: 2/16/2004
//-----------------------------------------------------------------------------
#include "audicle_face.h"
#include "audicle_gfx.h"
#include "audicle.h"

#include "digiio_rtaudio.h"




//-----------------------------------------------------------------------------
// name: AudicleFace()
// desc: ...
//-----------------------------------------------------------------------------
AudicleFace::AudicleFace( )
{
    if( !this->init( ) )
    {
        fprintf( stderr, "[audicle]: cannot start face...\n" );
        return;
    }
}




//-----------------------------------------------------------------------------
// name: ~AudicleFace()
// desc: ...
//-----------------------------------------------------------------------------
AudicleFace::~AudicleFace( )
{
    this->destroy( );
}




//-----------------------------------------------------------------------------
// name: init()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL AudicleFace::init( )
{
    m_id = Audicle::NO_FACE;
    m_state = INACTIVE;

    // light 0 position
    t_CKSINGLE light0_pos[4] = { 2.0f, 1.2f, 4.0f, 1.0f };

    // light 1 parameters
    t_CKSINGLE light1_ambient[] = { .2f, .2f, .2f, 1.0f };
    t_CKSINGLE light1_diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    t_CKSINGLE light1_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    t_CKSINGLE light1_pos[4] = { -2.0f, 0.0f, -4.0f, 1.0f };

    // copy
    memcpy( m_light0_pos, light0_pos, sizeof(m_light0_pos) );
    memcpy( m_light1_ambient, light1_ambient, sizeof(m_light1_ambient) );
    memcpy( m_light1_diffuse, light1_diffuse, sizeof(m_light1_diffuse) );
    memcpy( m_light1_specular, light1_specular, sizeof(m_light1_specular) );
    memcpy( m_light1_pos, light1_pos, sizeof(m_light1_pos) );

    m_angle_y = 0.0f;
    m_eye_y = 0.0f;
    m_bg_alpha = 0.0;
    m_bg_speed = .075;
    sleep_time = 0;

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: destroy()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL AudicleFace::destroy( )
{
    this->on_deactivate( 0.0 );
    m_id = Audicle::NO_FACE;
    m_state = INACTIVE;

    return TRUE;
}


//-----------------------------------------------------------------------------
// name: render_pre()
// desc: ...
//-----------------------------------------------------------------------------
void AudicleFace::render_pre( ) 
{
    glMatrixMode ( GL_PROJECTION ) ;
    glPushMatrix();
    glMatrixMode ( GL_MODELVIEW );
    glPushMatrix();
    
    xfade_bg();
}




//-----------------------------------------------------------------------------
// name: render()
// desc: ...
//-----------------------------------------------------------------------------
t_CKUINT AudicleFace::render( void * data )
{
    fprintf( stderr, "[audicle]: face '%i' ('%s') rendering...\n", m_id,
             m_name.c_str() );

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: render_post()
// desc: ...
//-----------------------------------------------------------------------------
void AudicleFace::render_post()
{
    glMatrixMode ( GL_PROJECTION ) ;
    glPopMatrix();
    glMatrixMode ( GL_MODELVIEW );
    glPopMatrix();
}



//-----------------------------------------------------------------------------
// name: on_activate()
// desc: ...
//-----------------------------------------------------------------------------
t_CKUINT AudicleFace::on_activate()
{
    switch( m_state )
    {
        case INACTIVE:
        case DEACTIVATING:
        {
            //fprintf( stderr, "[audicle]: face '%i' ('%s') activating...\n", m_id,
            //         m_name.c_str() );
            m_state = ACTIVATING;
            AudicleWindow::main()->m_bg_alpha = 1.0 - AudicleWindow::main()->m_bg_alpha;
            m_bg2 = AudicleWindow::main()->m_bg;
            m_bg_speed = -1.0;
            break;
        }

        case ACTIVE:
        {
            fprintf( stderr, "[audicle]: face '%i' ('%s') already active...\n",
                     m_id, m_name.c_str() );
            return FALSE;
        }

        case ACTIVATING:
        {
            fprintf( stderr, "[audicle]: face '%i' ('%s') already activating...\n",
                     m_id, m_name.c_str() );
            return FALSE;
        }

        default:
            fprintf( stderr, "[audicle]: face '%i' ('%s') in illegal state '%i'...\n",
                     m_id, m_name.c_str(), m_state );
            assert( FALSE );
    }

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: on_deactivate()
// desc: ...
//-----------------------------------------------------------------------------
t_CKUINT AudicleFace::on_deactivate( t_CKDUR dur )
{
    // gotta deactivate in <= dur (ChucK time)

    switch( m_state )
    {
        case ACTIVE:
        case ACTIVATING:
        {
            //fprintf( stderr, "[audicle]: face '%i' ('%s') deactivating...\n",
            //         m_id, m_name.c_str() );
            m_state = DEACTIVATING;
            break;
        }

        case INACTIVE:
        {
            fprintf( stderr, "[audicle]: face '%i' ('%s') already inactive...\n",
                     m_id, m_name.c_str() );
            return FALSE;
        }

        case DEACTIVATING:
        {
            fprintf( stderr, "[audicle]: face '%i' ('%s') already deactivating...\n",
                     m_id, m_name.c_str() );
            return FALSE;
        }

        default:
        {
            fprintf( stderr, "[audicle]: face '%i' ('%s') in illegal state '%i'...\n",
                     m_id, m_name.c_str() );
            assert( FALSE );
        }
    }

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: on_event()
// desc: ...
//-----------------------------------------------------------------------------
t_CKUINT AudicleFace::on_event( const AudicleEvent & event )
{
/*
    fprintf( stderr, "[audicle]: input event received...\n" );
    fprintf( stderr, "     type: %i\n", event.type );
    fprintf( stderr, "     mesg: %i\n", event.message );
    fprintf( stderr, "     arg1: %i\n", event.param1 );
    fprintf( stderr, "     arg2: %i\n", event.param2 );
    fprintf( stderr, "     arg3: %i\n", event.param3 );
    fprintf( stderr, "     time: %.4f sec\n", event.timestamp / Digitalio::sampling_rate() );
    fprintf( stderr, "     data: %i\n", event.data );
*/
    return TRUE;
}




//-----------------------------------------------------------------------------
// name: xfade_bg()
// desc: ...
//-----------------------------------------------------------------------------
void AudicleFace::xfade_bg( )
{
    m_bg_alpha = AudicleWindow::main()->m_bg_alpha;
    if( m_bg_alpha > 1.0 )
    {
        AudicleWindow::main()->m_bg_alpha = 1.0;
        glClearColor( m_bg[0], m_bg[1], m_bg[2], m_bg[3] );
        return;
    }

    Color4D c = m_bg2 * ( 1.0 - m_bg_alpha );
    c += m_bg * m_bg_alpha;
    glClearColor( c[0], c[1], c[2], c[3] );

    if( m_bg_speed < 0.0 ) m_bg_speed = AudicleWindow::main()->get_current_time();
    t_CKFLOAT delta = AudicleWindow::main()->get_current_time() - m_bg_speed;
    AudicleWindow::main()->m_bg = c;
    AudicleWindow::main()->m_bg_alpha += 3.0 * delta;
    m_bg_speed = AudicleWindow::main()->get_current_time();
}
