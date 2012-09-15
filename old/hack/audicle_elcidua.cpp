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
    user = new Agent();
    user->skin->set_walk( AGENT_LEGS_SIMPLE );
    dtime = 0;
    last = AudicleWindow::main()->get_current_time();

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


//-----------------------------------------------------------------------------
// name: render_view()
// desc: ...
//-----------------------------------------------------------------------------
void ElciduaFace::render_view( )
{
    glMatrixMode( GL_PROJECTION );

    //ortho for now, but we'll be doing some view management here. 

    AudicleWindow * aW = AudicleWindow::main();    
    glOrtho( 0.66 * -aW->m_hsize ,0.66 *  aW->m_hsize, 0.66 *  -aW->m_vsize, 0.66 *  aW->m_vsize, -5, 10 );

    glMatrixMode ( GL_MODELVIEW );
    glLoadIdentity();
}



//-----------------------------------------------------------------------------
// name: render()
// desc: ...
//-----------------------------------------------------------------------------
t_CKUINT ElciduaFace::render( void * data )
{

    dtime = AudicleWindow::main()->get_current_time() - last;
    last += dtime;


    //AudicleSession * session = SessionManager::cur_session();
    //SessionView * view = Audicle::cur_viewer();
    //view->render ( session );  

    //text ( window-oriented );
    glPushMatrix();  //session title
        glTranslated ( 0, 0.4 , 0);
        scaleFont( 0.12, 1.0 );
        drawString_centered("introducing elCidua");
    glPopMatrix();   //session title


    glEnable(GL_NORMALIZE);
    //glNormalize();

    bool dLighting = true; 

    if ( dLighting) { 
    float width = 100.0;
		glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT0);
		float ambient[4] = {0.2f, 0.2f, 0.2f, 0.2f};
		float diffuse[4] = {0.9f, 0.9f, 0.9f, 0.2f};
		float specular[4] = {0.2f, 0.2f, 0.2f, 0.1f};
		float position[4] = {40, 80, 200.0 , 0.0f};
		glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
		glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
		glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
		glLightfv(GL_LIGHT0, GL_POSITION, position);
		glEnable(GL_COLOR_MATERIAL);
		glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 1.0f);
		glColorMaterial(GL_FRONT_AND_BACK, GL_SPECULAR);
		glColor3f(0.8f, 0.8f, 0.8f);
		glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    }

    static int xx= 0;
    static float yy= 0;
    xx += 5;
    yy += 0.2;

//    glEnable(GL_LIGHTING);
    glColor3d( 1.0, 1.0 ,1.0);

    //semi-isometric world view ( this should use more of a camera model ) 

    glPushMatrix(); //stage

    glScaled ( 0.05 , 0.05 , 0.05 );
    glTranslated ( 0, -7, 0 );


    glRotated ( 20, 1, 0, 0 );
    glRotated ( yy, 0, 1, 0 );
 
    // walkin' doin' his thing.
    user->update( dtime );

    //center him for now...
    glTranslated ( -user->pos[0], -user->pos[1], -user->pos[2] ) ;
    // render using skin
    user->draw();


    if ( dLighting ) glDisable(GL_LIGHTING); //no lighting on our windows

//    user->draw_env(); 

    //get a view out front where we'll project
    //the user's windows, gfx environment. 

    //this will lock to the body-center
    //for fanciness, we'll allow the user's view-point
    //to follow his cursor position? 

    float env_forward_offset = 10.0; 
    float env_height = 10.0 ; //center of our view.
    float env_scale  = 0.5 * env_forward_offset ; //scaling of view


    //show orientation of o

    glPushMatrix(); //user frame


    GLdouble vmtx[16];
    Vec3D up( 0, 1, 0 );
    up.fillMatrix( vmtx, user->vel, user->pos);
    glMultMatrixd(vmtx);


    glBegin(GL_LINES);


    glColor3d ( 1,0,0 );
    glVertex3d( 0, 0, 0 );
    glVertex3d( 0, 0, env_forward_offset ); 
    glColor3d ( 0,1,0 );
    glVertex3d( 0, 0, env_forward_offset );
    glVertex3d( 0, env_height, env_forward_offset );
    glEnd(); //GL_LINES


    glPushMatrix(); //window space
    glTranslated ( 0, env_height, env_forward_offset );
    glRotated ( 180, 0, 1, 0 );

    glColor3d( 0, 1, 1 );
    glBegin(GL_LINES);
    glVertex3d( 0, 0, 0 );
    glVertex3d( 1, 0, 0 );

    glColor3d( 1, 0, 1 );
    glVertex3d( 0, 0, 0 );
    glVertex3d( 0, 1, 0 );
    glEnd();

    glPushMatrix(); //font space
//    scaleFont ( 2.0, 1.0 );
//    drawString_centered ("your text here");
    glPopMatrix(); //font space
    glScaled( env_scale, env_scale, env_scale );

    // we'll need to figure something out here. 
//    glEnable(GL_BLEND);
//    glDisable(GL_DEPTH_TEST);

    window_render_mode defmode = WindowManager::getRenderMode();
    //windows should know to change rendering method

    glPushAttrib(GL_POLYGON_BIT);
    glPolygonOffset( 10.0, 2.0 );
    glEnable(GL_POLYGON_OFFSET_FILL);
    WindowManager::setRenderMode( WINDOW_RENDER_DEPTH );
    user_wm->draw();
    WindowManager::setRenderMode( defmode );
    user_wm->drawCursor();
    glDisable(GL_POLYGON_OFFSET_FILL);
    glPopAttrib();



//    glEnable(GL_DEPTH_TEST);
//    glDisable ( GL_BLEND );
    // hmm.. 

    glPopMatrix(); //window space

    glPopMatrix(); //user frame 

    glPopMatrix(); //stage 
    
    //end view->render();
    return TRUE;
}


void
ElciduaFace::set_user_wm( WindowManager * w ) { 
    user_wm = w; 
}

//-----------------------------------------------------------------------------
// name: on_activate()
// desc: ...
//-----------------------------------------------------------------------------
t_CKUINT ElciduaFace::on_activate()
{
    dtime = 0;
    last = AudicleWindow::main()->get_current_time();
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
    return AudicleFace::on_event( event );
}
