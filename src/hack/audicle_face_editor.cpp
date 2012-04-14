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

    t_CKUINT pflag =  GL_LIGHTING_BIT | GL_COLOR_BUFFER_BIT \
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
