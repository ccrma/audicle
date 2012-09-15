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
// name: audicle_elcidua.cpp
// desc: the elcidua
//
// authors: Ge Wang (gewang@cs.princeton.edu)
//          Perry R. Cook (prc@cs.princeton.edu)
//          Philip Davidson (philipd@cs.princeton.edu)
//          Ananya Misra (amisra@cs.princeton.edu)
// date: Autumn 2004
//-----------------------------------------------------------------------------
#include "audicle_elcidua.h"
#include "audicle_gfx.h"
#include "audicle.h"

#include "audicle_session_manager.h"


//-----------------------------------------------------------------------------
// name: ElciduaFace()
// desc: ...
//-----------------------------------------------------------------------------
ElciduaFace::ElciduaFace( ) : AudicleFace( ) {
    if( !this->init( ) )
    {
        fprintf( stderr, "[audicle]: cannot start ElCiduaFace...\n" );
        return;
    }

}




//-----------------------------------------------------------------------------
// name: ~ElciduaFace()
// desc: ...
//-----------------------------------------------------------------------------
ElciduaFace::~ElciduaFace( ) { }




//-----------------------------------------------------------------------------
// name: init()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL ElciduaFace::init( )
{
    if( !AudicleFace::init() )
        return FALSE;

    m_bg[0] = 1.0; m_bg[1] = 1.0; m_bg[2] = 1.0; m_bg[3] = 1.0;
    m_name = "Co-Audicle";

    m_view = new CoSessionRendererImp();

    return TRUE;
}


//-----------------------------------------------------------------------------
// name: destroy()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL ElciduaFace::destroy( )
{
    this->on_deactivate( 0.0 );
    m_id = Audicle::NO_FACE;
    m_state = INACTIVE;

    return TRUE;
}


void ElciduaFace::render_pre() { 
    AudicleFace::render_pre();
    //your stuff here. 
}

void ElciduaFace::render_post() {
    //your stuff here
    AudicleFace::render_post();
}

//-----------------------------------------------------------------------------
// name: render_view()
// desc: ...
//-----------------------------------------------------------------------------
void ElciduaFace::render_view( )
{

    glMatrixMode( GL_PROJECTION );
    //ortho for now, but we'll be doing some view management here. 
    AudicleWindow * aW = AudicleWindow::main();    
    glOrtho( 0.66 * -aW->m_hsize ,0.66 *  aW->m_hsize, 0.66 *  -aW->m_vsize, 0.66 *  aW->m_vsize, 10, -10 );

    glMatrixMode ( GL_MODELVIEW );
    glLoadIdentity();
}



//-----------------------------------------------------------------------------
// name: render()
// desc: ...
//-----------------------------------------------------------------------------
t_CKUINT ElciduaFace::render( void * data )
{

    //AudicleSession * session = SessionManager::cur_session();
    //m_view->set_session(session);

    m_view->render();

    //end view->render();
    return TRUE;
}

void
ElciduaFace::set_session( CoSession * w )
{
    EM_log( CK_LOG_INFO, "setting Elcidua-session..." );
    m_session = w;
    m_view->set_session( m_session );
    m_view=m_view;
}

//-----------------------------------------------------------------------------
// name: on_activate()
// desc: ...
//-----------------------------------------------------------------------------
t_CKUINT ElciduaFace::on_activate()
{
    m_view->set_current_time( AudicleWindow::main()->get_current_time() );
    return AudicleFace::on_activate();
}




//-----------------------------------------------------------------------------
// name: on_deactivate()
// desc: ...
//-----------------------------------------------------------------------------
t_CKUINT ElciduaFace::on_deactivate( t_CKDUR dur )
{
    return AudicleFace::on_deactivate( dur );
}




//-----------------------------------------------------------------------------
// name: on_event()
// desc: ...
//-----------------------------------------------------------------------------
t_CKUINT ElciduaFace::on_event( const AudicleEvent & event )
{
 //   Audicle::instance()->move_to( (t_CKUINT)0 );
    if ( event.type == ae_event_INPUT )
    { 
        
        InputEvent * e = (InputEvent * ) event.data;
        ((ClientSessionImp*)m_session)->handleEvent( *e );
        
    }
    return AudicleFace::on_event( event );
}


ElDemoFace::ElDemoFace() :
ElciduaFace()
{ 
    m_view = m_demoview = new DemoRenderImp();
}


t_CKUINT ElDemoFace::on_event( const AudicleEvent & event ) { 
    
    t_CKUINT ret = 0;
    
    if ( event.type == ae_event_INPUT )
    { 
        InputEvent * e = (InputEvent * ) event.data;
        if ( e->type == ae_input_KEY ) { 
            InputState inp;
            inp.setEvent (*e, e->pos );
            switch ( inp.lastKey ) { 
            case '1':
                m_demoview->show_character = !m_demoview->show_character;
                break;
            case '2':
                m_demoview->show_windows = !m_demoview->show_windows;
                break;
            case '3':
                m_demoview->show_chat = !m_demoview->show_chat;
                break;
            case '4':
                m_demoview->show_portal = !m_demoview->show_portal;
                break;
            case '5':
                m_demoview->show_bindle = !m_demoview->show_bindle;
                break;
            case '6':
                m_demoview->show_shreds = !m_demoview->show_shreds;
                break;
            case '7':
                m_demoview->show_resources = !m_demoview->show_resources;
                break;
            case '8':
                m_demoview->show_controls = !m_demoview->show_controls;
                break;
            case '9':
                m_demoview->show_halo = !m_demoview->show_halo;
                break;
                        case '0':
                            m_demoview->show_army = !m_demoview->show_army;
            case '-':
              m_demoview->rem_agent();
              break;
            case '=':
              m_demoview->add_agent();
              break;
                        case 'e':
                        case 'r':
                        case 't':
                        case 'y':
						case 'u':
						case 'i':
                        case ' ':
                            m_demoview->do_move ( inp.lastKey );
                            break;
                        case 'a':
                            m_demoview->copy_move ( 0.1f );
                            break;
                        case 's':
                            m_demoview->copy_move ( 0.3f );
                            break;
                        case 'd':
                            m_demoview->copy_move ( 0.5f );
                            break;
                        case 'f':
                            m_demoview->copy_move ( 0.9f );
                            break;
                        case 'g':
                            m_demoview->copy_move ( 1.0f );
                            break;

                        case 'h':
                            m_demoview->copy_some ( 0.8f );
                            break;
                        case 'j':
                            m_demoview->copy_some ( 0.5f );
                            break;
                        case 'k':
                            m_demoview->copy_some ( 0.2f );
                            break;
                        case 'l':
                            m_demoview->copy_some ( 0.1f );
                            break;
            default:
                ret = 1;
                break;
                
            }
            
            
        }

    }
    if ( ret ) return AudicleFace::on_event( event );
    else return 0;
}


