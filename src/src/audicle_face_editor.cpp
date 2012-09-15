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
// name: audicle_face_editor.cpp
// desc: interface for audicle face vmspace
//
// authors: Philip Davidson (philipd@cs.princeton.edu)
//          Ge Wang (gewang@cs.princeton.edu)
//          Perry R. Cook (prc@cs.princeton.edu)
//          Ananya Misra (amisra@cs.princeton.edu)
// date: Autumn 2004
//-----------------------------------------------------------------------------
#include "audicle_face_editor.h"
#include "audicle_ui_coding.h"
#include "audicle_ui_console.h"
#include "audicle_gfx.h"
#include "audicle.h"




//-----------------------------------------------------------------------------
// name: AudicleFaceEditor()
// desc: ...
//-----------------------------------------------------------------------------
AudicleFaceEditor::AudicleFaceEditor( )
{
    if( !this->init( ) )
    {
        fprintf( stderr, "[audicle]: cannot start face...\n" );
        return;
    }
}




//-----------------------------------------------------------------------------
// name: ~AudicleFaceEditor()
// desc: ...
//-----------------------------------------------------------------------------
AudicleFaceEditor::~AudicleFaceEditor( ) 
{ 

}




//-----------------------------------------------------------------------------
// name: init()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL AudicleFaceEditor::init( )
{
    EM_log( CK_LOG_INFO, "AudicleFaceEditor: initializing..." );

    if( !AudicleFace::init() )
        return FALSE;

    wm = new WindowManager();
    wh = new CodeWindowHandler();
    wm->setHandler( wh );
    m_name = "ShrEDITOR";
    m_bg[0] = 1.0; m_bg[1] = 1.0; m_bg[2] = 1.0; m_bg[3] = 1.0;
    sleep_time = 30000;

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: destroy()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL AudicleFaceEditor::destroy( )
{
    EM_log( CK_LOG_INFO, "AudicleFaceEditor: cleaning up..." );

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: render()
// desc: ...
//-----------------------------------------------------------------------------
t_CKUINT AudicleFaceEditor::render( void * data )
{
    wm->draw();

    return 0;
}




//-----------------------------------------------------------------------------
// name: render_pre()
// desc: ...
//-----------------------------------------------------------------------------
void AudicleFaceEditor::render_pre()
{
    AudicleFace::render_pre();

    t_CKUINT pflag =  GL_LIGHTING_BIT | GL_COLOR_BUFFER_BIT
                      | GL_DEPTH_BUFFER_BIT ;

    pflag |= GL_POINT_BIT | GL_LINE_BIT; 
    glPushAttrib( pflag );

    glColorMask(true, true, true, true);

    glEnable (GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    if ( AudicleWindow::main()->m_antialiased ) { 
        glHint   (GL_POINT_SMOOTH_HINT, GL_NICEST );
        glHint   (GL_LINE_SMOOTH_HINT, GL_NICEST);
        glEnable (GL_POINT_SMOOTH);
        glEnable (GL_LINE_SMOOTH);
    }
    else { 
        glDisable (GL_POINT_SMOOTH);
        glDisable (GL_LINE_SMOOTH);
    }
    glPolygonOffset(  1.0, 1.0 );
    glLineWidth (1);
   
      //glEnable (GL_POLYGON_SMOOTH);
    glDisable (GL_DEPTH_TEST);
    
    glClearDepth(1900.0);
    glClearStencil(0);
}




//-----------------------------------------------------------------------------
// name: render_view()
// desc: ...
//-----------------------------------------------------------------------------
void AudicleFaceEditor::render_view( )
{
    glMatrixMode( GL_PROJECTION );

    AudicleWindow * aW = AudicleWindow::main();    
    wm->root()->setx(-aW->m_hsize );
    wm->root()->sety( aW->m_vsize );
    wm->root()->setw( aW->m_hsize * 2.0 );
    wm->root()->seth( aW->m_vsize * 2.0 );
    glOrtho(-aW->m_hsize, aW->m_hsize, -aW->m_vsize, aW->m_vsize, -10, 10 );

    glMatrixMode ( GL_MODELVIEW );
    glLoadIdentity();
}




//-----------------------------------------------------------------------------
// name: render_post()
// desc: ...
//-----------------------------------------------------------------------------
void AudicleFaceEditor::render_post()
{
    glPopAttrib();
    AudicleFace::render_post();
}




//-----------------------------------------------------------------------------
// name: on_activate()
// desc: ...
//-----------------------------------------------------------------------------
t_CKUINT AudicleFaceEditor::on_activate()
{
    return AudicleFace::on_activate();
}




//-----------------------------------------------------------------------------
// name: on_deactivate()
// desc: ...
//-----------------------------------------------------------------------------
t_CKUINT AudicleFaceEditor::on_deactivate( t_CKDUR dur )
{
    return AudicleFace::on_deactivate( dur );
}




//-----------------------------------------------------------------------------
// name: on_event()
// desc: ...
//-----------------------------------------------------------------------------
t_CKUINT AudicleFaceEditor::on_event( const AudicleEvent & event )
{
    if ( event.type == ae_event_INPUT )
    { 
        InputEvent * e = (InputEvent * ) event.data;
    
        DragManager *  d = DragManager::instance();
        if ( e->type == ae_input_MOUSE ) { 
            d->setmode( ( e->state == ae_input_DOWN ) ? ae_drag_is_Picking : ae_drag_is_Dropping );
        }

        wm->handleEvent( *e );

        if ( e->type == ae_input_MOUSE ) { 
            if ( d->mode() == ae_drag_is_Holding ) wm->setPick(e->pos) ;
            if ( e->state == ae_input_UP ) { //release an unhandled drag
        
                if ( d->object() != NULL && d->mode() != ae_drag_is_Empty ) {
                    switch ( d->type() ) { 
                    case ae_drag_CodeRevision:
                        wh->winFromRev ( (CodeRevision*) d->object() );                
                        break;
                    }
                    d->setmode ( ae_drag_is_Empty );
                    d->setobject  ( NULL, ae_drag_None );
                }
            }
        }
    }

    return AudicleFace::on_event( event );
}

void AudicleFaceEditor::set_session ( CoSession * s ) { 
    wm = (WindowManager*)s->wm();
    wh = (CodeWindowHandler*)wm->getHandler();
}

