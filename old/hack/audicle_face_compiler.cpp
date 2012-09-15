//-----------------------------------------------------------------------------
// name: audicle_face_compiler.cpp
// desc: interface for audicle face compiler
//
// authors: Ge Wang (gewang@cs.princeton.edu)
//          Perry R. Cook (prc@cs.princeton.edu)
//          Philip Davidson (philipd@cs.princeton.edu)
//          Ananya Misra (amisra@cs.princeton.edu)
// date: Autumn 2004
//-----------------------------------------------------------------------------
#include "audicle_face_compiler.h"
#include "audicle_gfx.h"
#include "audicle.h"




//-----------------------------------------------------------------------------
// name: AudicleFaceCompiler()
// desc: ...
//-----------------------------------------------------------------------------
AudicleFaceCompiler::AudicleFaceCompiler( )
{
    if( !this->init( ) )
    {
        fprintf( stderr, "[audicle]: cannot start compiler space face...\n" );
        return;
    }
}




//-----------------------------------------------------------------------------
// name: ~AudicleFaceCompiler()
// desc: ...
//-----------------------------------------------------------------------------
AudicleFaceCompiler::~AudicleFaceCompiler( ) { }




//-----------------------------------------------------------------------------
// name: init()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL AudicleFaceCompiler::init( )
{
    if( !AudicleFace::init() )
        return FALSE;

    m_name = "Compiler-Space";

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: destroy()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL AudicleFaceCompiler::destroy( )
{
    this->on_deactivate( 0.0 );
    m_id = Audicle::NO_FACE;
    m_state = INACTIVE;

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: render()
// desc: ...
//-----------------------------------------------------------------------------
t_CKUINT AudicleFaceCompiler::render( void * data )
{
    return 0;
}




//-----------------------------------------------------------------------------
// name: render_pre()
// desc: ...
//-----------------------------------------------------------------------------
void AudicleFaceCompiler::render_pre()
{
    glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );

    AudicleFace::render_pre();

    glPushAttrib( GL_LIGHTING_BIT | GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT ); 

    // enable depth
    glEnable( GL_DEPTH_TEST );
    // enable lighting
    glEnable( GL_LIGHTING );

    // enable light 0
    glEnable( GL_LIGHT0 );

    // material have diffuse and ambient lighting 
    glColorMaterial( GL_FRONT, GL_AMBIENT_AND_DIFFUSE );
    // enable color
    glEnable( GL_COLOR_MATERIAL );
    
    // setup and enable light 1
    glLightfv( GL_LIGHT1, GL_AMBIENT, m_light1_ambient );
    glLightfv( GL_LIGHT1, GL_DIFFUSE, m_light1_diffuse );
    glLightfv( GL_LIGHT1, GL_SPECULAR, m_light1_specular );
    glEnable( GL_LIGHT1 );
}




//-----------------------------------------------------------------------------
// name: render_post()
// desc: ...
//-----------------------------------------------------------------------------
void AudicleFaceCompiler::render_post()
{
    glPopAttrib();

    AudicleFace::render_post();
}




//-----------------------------------------------------------------------------
// name: render_view()
// desc: ...
//-----------------------------------------------------------------------------
void AudicleFaceCompiler::render_view( )
{
    // set the matrix mode to project
    glMatrixMode( GL_PROJECTION );
    // load the identity matrix
    // load the identity matrix XXX
    // this is handled by AudicleWindow, in order to set up pick matrices...
    // you can assume that LoadIdentity has been called already
    //glLoadIdentity( ); 
    // create the viewing frustum
    gluPerspective( 45.0, (GLfloat) AudicleWindow::main()->m_w / 
        (GLfloat) AudicleWindow::main()->m_h, 1.0, 300.0 );
    // set the matrix mode to modelview
    glMatrixMode( GL_MODELVIEW );
    // load the identity matrix
    glLoadIdentity( );
    // position the view point
    gluLookAt( 0.0f, 3.5f * sin( m_eye_y ), 3.5f * cos( m_eye_y ), 
               0.0f, 0.0f, 0.0f, 
               0.0f, ( cos( m_eye_y ) < 0 ? -1.0f : 1.0f ), 0.0f );

    // set the position of the lights
    glLightfv( GL_LIGHT0, GL_POSITION, m_light0_pos );
    glLightfv( GL_LIGHT1, GL_POSITION, m_light1_pos );
}




//-----------------------------------------------------------------------------
// name: on_activate()
// desc: ...
//-----------------------------------------------------------------------------
t_CKUINT AudicleFaceCompiler::on_activate()
{
    return AudicleFace::on_activate();
}




//-----------------------------------------------------------------------------
// name: on_deactivate()
// desc: ...
//-----------------------------------------------------------------------------
t_CKUINT AudicleFaceCompiler::on_deactivate( t_CKDUR dur )
{
    return AudicleFace::on_deactivate( dur );
}




//-----------------------------------------------------------------------------
// name: on_event()
// desc: ...
//-----------------------------------------------------------------------------
t_CKUINT AudicleFaceCompiler::on_event( const AudicleEvent & event )
{
    return AudicleFace::on_event( event );
}
