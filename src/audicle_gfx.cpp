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
// name: audicle_gfx.cpp
// desc: audicle graphics
//
// authors: Ge Wang (gewang@cs.princeton.edu)
//          Perry R. Cook (prc@cs.princeton.edu)
//          Philip Davidson (philipd@cs.princeton.edu)
//          Ananya Misra (amisra@cs.princeton.edu)
// date: Autumn 2004
//-----------------------------------------------------------------------------
#include "audicle_gfx.h"
#include "audicle_nexus.h"
#include "audicle.h"
#include "audicle_ui_console.h"
#include "audicle_font.h"
#include <memory.h>
#ifndef __PLATFORM_WIN32__
#include <sys/time.h>
#endif

// global data
t_CKBOOL AudicleGfx::init_on = FALSE;
t_CKBOOL AudicleGfx::cursor_on = TRUE;

AudicleWindow * AudicleWindow::our_main = NULL;
t_CKBOOL AudicleWindow::our_fullscreen = FALSE;


// callback
void g_main_draw( );
void g_main_reshape( int x, int y );
void g_main_keyboard( unsigned char c, int x, int y );
void g_main_special_keys( int c, int x, int y );
void g_main_mouse( int button, int state, int x, int y );
void g_main_depressed_motion( int x, int y );
void g_main_motion( int x, int y );
void g_main_idle( );
void g_main_die( );
//void g_main_check_gl_err (); //move ( check_gl_err() ) into .h so everyone can use 



//-----------------------------------------------------------------------------
// name: init()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL AudicleGfx::init()
{
    if( init_on )
        return TRUE;
    
    // log
    EM_log( CK_LOG_SYSTEM, "(audicle) initializing graphics engine..." );
    // push log
    EM_pushlog();
    
    // set cursor
    cursor_on = true;
    
    // log
    EM_log( CK_LOG_SYSTEM, "(audicle) cursor state: %s", cursor_on ? "ON" : "OFF" );

#ifdef __MACOSX_CORE__ 
    char * cwd = getcwd( NULL, 0 );
#endif
    
    // glut
    int foo = 1; const char * bar[] = { "audicle" };
    EM_log( CK_LOG_SYSTEM, "(audicle) initializing opengl/glut..." );
    glutInit( &foo, (char **)bar);
    
#ifdef __MACOSX_CORE__
    //correct the directory..
    chdir( cwd );
    free( cwd );
#endif

    AudicleWindow::main()->get_current_time( TRUE );

    init_on = TRUE;

    //hmm. need to get the fonts in order around here somewhere. 

    // pop log
    EM_poplog();

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: loop()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL AudicleGfx::loop()
{
    if( !init_on )
        return FALSE;
    
    // loop
    glutMainLoop();
    
    return TRUE;
}




//-----------------------------------------------------------------------------
// name: main()
// desc: ...
//-----------------------------------------------------------------------------
AudicleWindow * AudicleWindow::main( )
{
    if( !our_main )
        our_main = new AudicleWindow;

    assert( our_main != NULL );
    return our_main;
}


//-----------------------------------------------------------------------------
// name: shutdown()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL AudicleGfx::shutdown()
{
    if( !init_on )
        return TRUE;

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: AudicleWindow()
// desc: ...
//-----------------------------------------------------------------------------
AudicleWindow::AudicleWindow()
{
    m_windowID = 0;
    memset( m_pick_buffer, 0, sizeof(m_pick_buffer) );
    memset( m_cur_vp, 0, sizeof(m_cur_vp) );
    m_hsize = 800;
    m_vsize = 600;
    m_w = 800;
    m_h = 600;
    m_render_mode = GL_RENDER;
    m_antialiased = true;
    m_console = NULL;
    m_bg = Color4D( 0.0f, 0.0f, 0.0f, 1.0f );
    m_bg_alpha = 1.0;
    m_frame_stamp = 0;
	m_syntax_query = NULL;
}





//-----------------------------------------------------------------------------
// name: init()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL AudicleWindow::init( t_CKUINT w, t_CKUINT h, t_CKINT xpos, t_CKINT ypos,
                              const char * name, t_CKBOOL fullscreen )
{
    if( m_windowID ) return TRUE;

    // log
    EM_log( CK_LOG_SYSTEM, "(audicle) initializing windowing system..." );
    // push log
    EM_pushlog();

    // log
    EM_log( CK_LOG_SYSTEM, "(audicle) window size: %d x %d", w, h );
    EM_log( CK_LOG_SYSTEM, "(audicle) window position: %d, %d", xpos, ypos );
    
    // set modes
    glutInitDisplayMode( GLUT_RGBA | GLUT_ALPHA | GLUT_DOUBLE | GLUT_DEPTH | GLUT_STENCIL );
    glutInitWindowSize( w, h );
    glutInitWindowPosition( xpos, ypos );
    
    m_windowID = glutCreateWindow( name );
    
    // set callback functions
    glutDisplayFunc ( g_main_draw );
    glutReshapeFunc ( g_main_reshape );
    glutKeyboardFunc( g_main_keyboard );
    glutSpecialFunc ( g_main_special_keys );
    glutMouseFunc   ( g_main_mouse );
    glutMotionFunc  ( g_main_depressed_motion );
    glutPassiveMotionFunc ( g_main_motion );
    glutVisibilityFunc ( NULL );
    glutIdleFunc( g_main_idle );
    
    // log
    EM_log( CK_LOG_SYSTEM, "(audicle) fullscreen mode: %s", fullscreen ? "ON" : "OFF" );
	
	m_w = w;
	m_h = h;
	main()->fullscreen( fullscreen );

#if (GLUT_MACOSX_IMPLEMENTATION >= 2 ) 
    glutWMCloseFunc ( g_main_die );
#endif
    
//    glEnable( GL_POINT_SMOOTH );
//    glEnable( GL_LINE_SMOOTH );
//    glEnable( GL_BLEND );
//    glColorMask( true, true, true, true );
//    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
//    glHint (GL_LINE_SMOOTH_HINT, GL_DONT_CARE);

    glPolygonOffset( 1.0, 1.0 );
    glLineWidth( 1 );
    
    // glEnable( GL_POLYGON_SMOOTH );
    glEnable (GL_DEPTH_TEST);
    
    glClearColor( 1.0, 1.0, 1.0, 1.0f );
//    glClearDepth( 1900 );
//    glClearStencil( 0 );
    
    glSelectBuffer( AG_PICK_BUFFER_SIZE, m_pick_buffer );

    glViewport( 0, 0, w, h );
    glGetIntegerv( GL_VIEWPORT, m_cur_vp );
    
    m_hsize = 1.00;
    m_vsize = 1.33;
    
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity( );

    //perspective?  
    glOrtho( -m_hsize, m_hsize, -m_vsize, m_vsize, -10, 10 );
    
    setupFonts();
    //this is silly as all hell
#if defined(WIN32)
//    ShowCursor( FALSE );
#endif

    m_console = new ConsoleWindow();

	m_syntax_query = new SyntaxQuery();
    // pop log
    EM_poplog();

    return ( m_windowID != 0 );
}




//-----------------------------------------------------------------------------
// name: set_mouse_coords()
// desc: ...
//-----------------------------------------------------------------------------
void AudicleWindow::set_mouse_coords( int x, int y )
{
     m_mousex = x;
     m_mousey = y;
     pix_to_ndc ( x, y );
}




//-----------------------------------------------------------------------------
// name: set_projection()
// desc: ...
//-----------------------------------------------------------------------------
void AudicleWindow::set_projection()
{
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity( );

    if ( m_render_mode == GL_SELECT )
    {
        gluPickMatrix( m_mousex, m_cur_vp[3] - m_mousey, AG_PICK_TOLERANCE, AG_PICK_TOLERANCE, m_cur_vp );
        glInitNames( );
        glPushName( 0xffffffff ); /* a strange value */
    }

    glMatrixMode( GL_MODELVIEW );
}




//-----------------------------------------------------------------------------
// name: pix_to_ndc()
// desc: ...
//-----------------------------------------------------------------------------
void AudicleWindow::pix_to_ndc( int x, int y )
{
    m_cur_pt[0] = m_hsize * 2.0 * ( (double)x / (double)m_cur_vp[2] - 0.5 );
    m_cur_pt[1] = m_vsize * 2.0 * ( (double)(m_cur_vp[3]-y) / (double)m_cur_vp[3] - 0.5 );
}


//-----------------------------------------------------------------------------
// name: pix_to_ndc()
// desc: ...
//-----------------------------------------------------------------------------

#ifdef __PLATFORM_WIN32__
#include <sys/timeb.h>
#else 
#ifdef __PLATFORM_LINUX__
#include <sys/time.h>
#endif
#endif 

double AudicleWindow::get_current_time( t_CKBOOL fresh )
{
#ifdef __PLATFORM_WIN32__
    struct _timeb t;
    _ftime(&t);
    return t.time+t.millitm/1000.0;
#else
    static struct timeval t;
    if( fresh ) gettimeofday(&t,NULL);
    return t.tv_sec+(double)t.tv_usec/1000000;
#endif
}

t_CKBOOL 
AudicleWindow::check_stamp ( t_CKUINT * stamp ) { 
    assert ( stamp );
    if ( *stamp == m_frame_stamp ) return false;
    else *stamp = m_frame_stamp;
    return true;
}



//-----------------------------------------------------------------------------
// name: reshape()
// desc: ...
//-----------------------------------------------------------------------------
void AudicleWindow::main_reshape( int w, int h )
{
    assert( this == AudicleWindow::main() );
    
    glViewport( 0, 0, w, h );
    glGetIntegerv( GL_VIEWPORT, m_cur_vp );

    m_vsize = ( m_cur_vp[3] == 0 ) ? 1 : (double)m_cur_vp[3]/480.0;
    double aspect = ( m_cur_vp[3] == 0 ) ? 1 : (double)m_cur_vp[2] / (double)m_cur_vp[3]; 
    m_hsize = aspect * m_vsize;

    // project
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity( ); 
//    glOrtho( -m_hsize, m_hsize, -m_vsize, m_vsize, -10, 10 );

    // view
    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity( );
    
    m_w = w;
    m_h = h;


    //XXX haaaaaaaaack!
    //we need an OSD::class instead of using that WindowManager...
    //that will take care of console and messages...
    //and can track current face to determine mesg visibility.

    //reshape
    m_console->fit_to_window ( -m_hsize, m_hsize, -m_vsize, m_vsize ) ;

}




//-----------------------------------------------------------------------------
// name: main_draw()
// desc: ...
//-----------------------------------------------------------------------------
void AudicleWindow::main_draw( )
{
    assert( this == AudicleWindow::main() );
    m_frame_stamp++;

    AudicleWindow::main()->get_current_time( TRUE );
    
    // clear buffers
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );

    // set projection ( may be picking )
    set_projection();
    AudicleFace * face = Audicle::instance()->face();

    // push state
    face->render_pre();

    // set view matrices 
    face->render_view();     

    // do yer buzinezz;
    face->render( NULL );


/*
    glEnable(GL_TEXTURE_2D);
    glColor4d ( 0,0,0,1.0);
    glPushMatrix();
    scaleFont( 1.0, 1.0 );

    kernedFont->Render( "hi!" );
    monospacedFont->Render( "hello!" );

    glPopMatrix();    
    glDisable(GL_TEXTURE_2D);
*/
    // restore state
    face->render_post();

    // rendering for constant overlay...
    // we may place a windowmanager here?
    // move messages to a non-static setup....
    // for now put a console at least in here..

    ui_render_console ();
    
    // flush
    glFlush();
    // swap
    if( m_render_mode == GL_RENDER )
        glutSwapBuffers();
}



//-----------------------------------------------------------------------------
// name: destroy()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL AudicleWindow::destroy()
{
    assert( this == AudicleWindow::main() );

    if( m_windowID ){ 
        glutDestroyWindow ( m_windowID );
    }
    
    return TRUE;
}




//-----------------------------------------------------------------------------
// name: main_pick()
// desc: ...
//-----------------------------------------------------------------------------

void AudicleWindow::main_pick() { 

    m_render_mode = GL_SELECT;
    glRenderMode( m_render_mode );

    main_draw();

    //return to normal
    m_render_mode = GL_RENDER;
    int nHit = glRenderMode( m_render_mode );

    t_CKUINT n  = 0 ;
    m_pick_top  = NULL;
    m_pick_size = 0;
    
    for( int i = 0; i < nHit; i++ )
    { 
        m_pick_size = m_pick_buffer[n++];
        t_CKUINT zMin = m_pick_buffer[n++];
        t_CKUINT zMax = m_pick_buffer[n++];
        m_pick_top = ( m_pick_buffer + n );
#ifdef AUDICLE_DEBUG_PICK
        fprintf(stderr, "stack %d objs, %d %d\n", m_pick_size ,zMin, zMax );
        for( int j = 0; j < m_pick_size ; j++ )
        {
         fprintf(stderr, "item %u\n", m_pick_buffer[n++]);
        }        
#else
        n += m_pick_size;
#endif
    }


}

//-----------------------------------------------------------------------------
// name: main_mouse()
// desc: ...
//-----------------------------------------------------------------------------
void AudicleWindow::main_mouse( int button, int state, int x, int y)
{
    assert( this == AudicleWindow::main() );
    set_mouse_coords( x, y );

    //fetch pick stack
    main_pick();
    t_CKUINT * temp_stack = (t_CKUINT*) malloc ( m_pick_size * sizeof(t_CKUINT) );
    memcpy ( (void*)temp_stack, (void*)m_pick_top, m_pick_size * sizeof(t_CKUINT) );

    // create mouse input event
    InputEvent* ev = new InputEvent ( ae_input_MOUSE, m_cur_pt, button, state );
    ev->setMods( glutGetModifiers() );
    ev->setStack( temp_stack , m_pick_size );
    ev->time = get_current_time();

    InputEvent sub = *ev;

    //START AN AUDICLE EVENT 
    //make_mouse_event( m_cur_pt,  button, state );

    AudicleEvent event;
    event.type = ae_event_INPUT;
    event.message = 2;
    event.param1 = button;
    event.param2 = state;
    event.data = (void *) ev;

    Audicle::instance()->face()->on_event( event );

    sub.popStack();

    m_console->handleMouse( sub );
    m_console->handleMouseAlerts( sub ); 

    free (temp_stack);
    delete ev; //XXX assumes unqueued...

    //    if ( wm ) wm->handleMouse(button, state, cur_pt);

    //    if ( DragManager::instance()->object() ) { } 

}




//-----------------------------------------------------------------------------
// name: main_motion()
// desc: ...
//-----------------------------------------------------------------------------
void AudicleWindow::main_motion( int x, int y )
{
    assert( this == AudicleWindow::main() );
    set_mouse_coords(x,y);

//XXX
#ifdef MACOSX
    if ( 0 < x && x < m_cur_vp[2] && 0 < y && y < m_cur_vp[3] ) { 
        if ( cursorOn ) { 
            HideCursor();
            cursorOn = false;
        }
    }
    else { 
        if ( !cursorOn ) { 
            ShowCursor();
            cursorOn = true;
        }
    }
#endif

    InputEvent * ev = new InputEvent ( ae_input_MOTION, m_cur_pt );
    ev->time = get_current_time();

    AudicleEvent event;
    event.type = ae_event_INPUT;
    event.message = 2;
    event.data = (void *) ev;

    Audicle::instance()->face()->on_event( event );

    delete ev; //XXX assumes unqueued...

}




//-----------------------------------------------------------------------------
// name: main_depressed_motion()
// desc: ...
//-----------------------------------------------------------------------------
void AudicleWindow::main_depressed_motion( int x, int y )
{
    assert( this == AudicleWindow::main() );

    set_mouse_coords(x,y);
    // if ( wm ) wm->handleMotion(cur_pt);

    InputEvent * ev = new InputEvent ( ae_input_MOTION, m_cur_pt );
    ev->time = get_current_time();

    AudicleEvent event;
    event.type = ae_event_INPUT;
    event.message = 2;
    event.data = (void *) ev;

    Audicle::instance()->face()->on_event( event );

    delete ev; //XXX assumes unqueued...
}




//-----------------------------------------------------------------------------
// name: fullscreen()
// desc: ...
//-----------------------------------------------------------------------------
void AudicleWindow::fullscreen( t_CKBOOL fs )
{
	static int w, h;

	our_fullscreen = fs;
	if( our_fullscreen )
	{
		w = m_w;
		h = m_h;
		glutFullScreen();
	}
	else
	{
		if( !w || !h )
		{
			w = m_w;
			h = m_h;
		}
		glutReshapeWindow( w, h );
	}
}




//-----------------------------------------------------------------------------
// name: main_keyboard()
// desc: ...
//-----------------------------------------------------------------------------
void AudicleWindow::main_keyboard( unsigned char c, int x, int y )
{
    assert( this == AudicleWindow::main() );
    set_mouse_coords(x,y);

    InputEvent * ev = new InputEvent ( ae_input_KEY, m_cur_pt, c ); 
    ev->setMods( glutGetModifiers() );
    ev->time = get_current_time();

    AudicleEvent event;
    event.type = ae_event_INPUT;
    event.message = 2;
    event.data = (void *) ev;
    
    bool handled = false;

    if( ev->mods & ae_input_CTRL ) 
    {
        switch( c )
        {   
        case KEY_CTRL_G:
            our_fullscreen = !our_fullscreen;
			fullscreen( our_fullscreen );
            handled = true;
            break;
        case KEY_CTRL_Q:
            handled = true;
            // stop
            the()->stop();
            // detach
            all_detach();
            // done
            exit( 0 );
            break;
        }
    }
    else { 
        switch ( c ) { 
        case '`':
            if ( ev->mods & ae_input_ALT ) {
                m_antialiased = !m_antialiased;
            }
            else { 
                if ( m_console->active() ) m_console->deactivate();
                else m_console->activate();
            }
            handled = true;
            break;
        }

    }
    
    if ( !handled ) { 

        if ( m_console->active() && m_console->selected() ) { 
            m_console->handleKey( *ev );
        }
        else { 
            Audicle::instance()->face()->on_event( event );
        }


    }
    delete ev; //XXX assumes unqueued...


//   if ( wm ) wm->handleKey( c, cur_pt );
}




//-----------------------------------------------------------------------------
// name: main_special_keys()
// desc: ...
//-----------------------------------------------------------------------------
void AudicleWindow::main_special_keys( int key, int x, int y )
{
    assert( this == AudicleWindow::main() );
    set_mouse_coords(x,y);
    
    InputEvent * ev = new InputEvent ( ae_input_SPEC, m_cur_pt, key ); 
    ev->setMods( glutGetModifiers() );
    ev->time = get_current_time();

    AudicleEvent event;
    event.type = ae_event_INPUT;
    event.message = 2;
    event.data = (void *) ev;

    bool handled = false;
    if ( ev->mods & ae_input_CTRL ) { 
        handled = true;
        switch ( key ) { 
        case KEY_LEFTARROW:
            Audicle::instance()->move( Audicle::LEFT );
            m_console->setCubes();
            break;    
        case KEY_RIGHTARROW:
            Audicle::instance()->move( Audicle::RIGHT );
            m_console->setCubes();
            break;
        case KEY_UPARROW:
            Audicle::instance()->move( Audicle::UP );
            m_console->setCubes();
            break;
        case KEY_DOWNARROW:
            Audicle::instance()->move( Audicle::DOWN );
            m_console->setCubes();
            break;
        default:
            handled = false; 
            break;
        }
    }
    if ( !handled ) { 
        if ( m_console->active()  && m_console->selected() ) 
        { 
            m_console->handleSpec ( *ev ) ;
        }
        else 
        { 
            Audicle::instance()->face()->on_event( event );
        }
    }

    delete ev; //XXX assumes unqueued...


//   if ( wm ) wm->handleSpec( key, cur_pt);  
}



//-----------------------------
// name: ui_render_console
// desc: ...
//-----------------------------
void
AudicleWindow::ui_render_console() 
{ 
    //push Attribs

//    if ( m_console->active() ) { 

    t_CKUINT pflag =  GL_LIGHTING_BIT | GL_COLOR_BUFFER_BIT \
        | GL_DEPTH_BUFFER_BIT ;
    
    pflag |= GL_POINT_BIT | GL_LINE_BIT; 
    
    glPushAttrib( pflag ) ;


    glColorMask(true, true, true, true);
    
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable (GL_BLEND);

    if ( m_antialiased ) { 
        glEnable (GL_POINT_SMOOTH);
        glEnable (GL_LINE_SMOOTH);
//        glHint   (GL_LINE_SMOOTH_HINT, GL_NICEST);
    }
    else { 
        glDisable (GL_POINT_SMOOTH);
        glDisable (GL_LINE_SMOOTH);
    }

    glDisable (GL_DEPTH_TEST);
    
    glClearStencil(0);

    //set view
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();

    glOrtho( -m_hsize, m_hsize, -m_vsize, m_vsize, -10, 10 );

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();


    //draw!

    m_console->drawWindow();

    m_console->drawAlerts();


    //pop view
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    
    //pop attrib
    
    glPopAttrib();
//    }
}


IDManager * IDManager::m_inst = NULL;


//-----------------------------------------------------------------------------
// name: IDManager()
// desc: ...
//-----------------------------------------------------------------------------

IDManager::IDManager( ) { 

    GLint bits = 8;
    glGetIntegerv( GL_STENCIL_BITS, &bits );
    m_sidnum = 1 << bits;

    m_sids = ( t_CKBOOL *) malloc( m_sidnum * sizeof( t_CKBOOL ) );

    memset( m_sids, 0, m_sidnum * sizeof( t_CKBOOL ) ) ;
    m_free = NULL;
    m_pickID = m_sidnum; //pickIDs are separate from stencil ids
}





//-----------------------------------------------------------------------------
// name: instance()
// desc: ...
//-----------------------------------------------------------------------------
IDManager * 
IDManager::instance( ) { 
    if ( !m_inst ) 
    { 
        m_inst = new IDManager( );
        if ( !m_inst ) 
        { 
            fprintf( stderr,"IDManager::could not create _instance;\n");
        }
    }
    return m_inst;
}





//-----------------------------------------------------------------------------
// name: getStencilID()
// desc: ...
//-----------------------------------------------------------------------------
t_CKUINT
IDManager::getStencilID( ) { 
    for ( int i = 1 ; i < m_sidnum ; i++ ) 
    { 
        if ( m_sids[i] == false ) 
        {
            m_sids[i] = true;
            //fprintf( stderr, "assigned stencil %d\n", i ); 
            return i;
        }
    }
    fprintf( stderr, "IDManager::getStencilID - error - ID array is full!\n");
    return 0;
}




//-----------------------------------------------------------------------------
// name: freeStenciID()
// desc: ...
//-----------------------------------------------------------------------------
void
IDManager::freeStencilID( t_CKUINT i ) {
    if ( i > 0 && i < m_sidnum ) 
    { 
        if ( !m_sids[i] ) 
            fprintf( stderr, "IDManager::_freeID - error: freeing unbound ID %d\n", i );
        m_sids[i] = false;
    }
    else fprintf( stderr,"IDManager::freeStencilID - ID %d out of range!\n", i);    
}





//-----------------------------------------------------------------------------
// name: getPickID()
// desc: ...
//-----------------------------------------------------------------------------
t_CKUINT 
IDManager::getPickID( ) {
    return ++m_pickID;
}


//-----
// name: freePickID ( t_CKUINT id );
// desc: ...
//-----
void
IDManager::freePickID ( t_CKUINT id ) { 
    freeID * nw = new freeID();
    nw->id = id;
    nw->next = m_free;
    m_free = nw;

}




DragManager * DragManager::m_instance = NULL;

DragManager::DragManager( )  
{ 
    m_type       = ae_drag_None;
    m_object     = NULL;
}

DragManager* 
DragManager::instance( ) 
{ 
    if ( !m_instance ) 
    { 
        m_instance = new DragManager( );
        if ( !m_instance ) { 
            fprintf( stderr, "[chuck] error: DragManager instance creation failed\n" );
        }   
    }
    return m_instance;
}



//-----------------------------------------------------------------------------
// name: g_main_draw()
// desc: ...
//-----------------------------------------------------------------------------
void g_main_draw( void )
{
    AudicleWindow::main()->main_draw();
    check_gl_err();
}




//-----------------------------------------------------------------------------
// name: g_main_check_gl_err()
// desc: ...
//-----------------------------------------------------------------------------
void check_gl_err()
{
    GLenum errCode;
    const GLubyte *errString;

    while( (errCode = glGetError()) != GL_NO_ERROR )
    { 
        errString = gluErrorString( errCode );
        fprintf( stderr, "[audicle]: OpenGL error: %d\t%s\n", errCode, errString ); 
    }
}


void render_unit_axes() {
    glBegin(GL_LINES);
    glColor3d ( 1, 0, 0 );
    glVertex3d( 1, 0, 0 );
    glVertex3d( 0, 0, 0 );
    glColor3d ( 0, 1, 0 );
    glVertex3d( 0, 1, 0 );
    glVertex3d( 0, 0, 0 );
    glColor3d ( 0, 0, 1 );
    glVertex3d( 0, 0, 1 );
    glVertex3d( 0, 0, 0 );
    glEnd();    
}

//-----------------------------------------------------------------------------
// name: g_main_idle()
// desc: ...
//-----------------------------------------------------------------------------
void  g_main_idle( )
{
//    static double ltime = 0; 

    glutPostRedisplay();
    int sleeps = Audicle::instance()->face() ? Audicle::instance()->face()->sleep_time : 20000; 
    usleep( sleeps );

//    double dtime = AudicleWindow::main()->get_current_time() ; 
//    fprintf(stderr, "time %f\n", dtime - ltime ) ;
//    ltime = dtime;
}




//-----------------------------------------------------------------------------
// name: g_main_reshape()
// desc: ...
//-----------------------------------------------------------------------------
void g_main_reshape( int width, int height )
{
    AudicleWindow::main()->main_reshape( width, height );
}




//-----------------------------------------------------------------------------
// name: g_main_mouse()
// desc: ...
//-----------------------------------------------------------------------------
void g_main_mouse( int button, int state, int x, int y )
{
    AudicleWindow::main()->main_mouse( button, state, x, y );
}




//-----------------------------------------------------------------------------
// name: g_main_motion()
// desc: ...
//-----------------------------------------------------------------------------
void g_main_motion( int x, int y)
{
    AudicleWindow::main()->main_motion( x, y );
}




//-----------------------------------------------------------------------------
// name: g_main_depressed_motion()
// desc: ...
//-----------------------------------------------------------------------------
void g_main_depressed_motion( int x, int y )
{
    AudicleWindow::main()->main_depressed_motion(x,y);
}




//-----------------------------------------------------------------------------
// name: g_main_keyboard()
// desc: ...
//-----------------------------------------------------------------------------
void g_main_keyboard( unsigned char c, int x, int y )
{
    AudicleWindow::main()->main_keyboard( c, x, y );
}




//-----------------------------------------------------------------------------
// name: g_main_special_keys()
// desc: ...
//-----------------------------------------------------------------------------
void g_main_special_keys( int key, int x, int y )
{
    AudicleWindow::main()->main_special_keys( key, x, y );
}




//-----------------------------------------------------------------------------
// name: g_main_die()
// desc: ...
//-----------------------------------------------------------------------------
void g_main_die( )
{
    fprintf( stderr, "[audicle]: exiting...\n" );
    exit( 0 );
}
