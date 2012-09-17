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
// name: audicle_face_skot.cpp
// desc: interface for audicle face slot machine
//
// authors: Ge Wang (gewang@cs.princeton.edu)
//          Perry R. Cook (prc@cs.princeton.edu)
//          Philip Davidson (philipd@cs.princeton.edu)
//          Ananya Misra (amisra@cs.princeton.edu)
//          Scott Smallwood (skot@princeton.edu)
// date: Spring 2006
//-----------------------------------------------------------------------------
#include "audicle_face_skot.h"
#include "audicle_gfx.h"
#include "audicle.h"

#include "chuck_lang.h"
#include "chuck_instr.h"

#include <vector>
using namespace std;


//-----------------------------------------------------------------------------
// name: AudicleFaceFloor()
// desc: ...
//-----------------------------------------------------------------------------
AudicleFaceFloor::AudicleFaceFloor( t_CKUINT num )
{
    m_num_triggers = num;

    if( !this->init( ) )
    {
        fprintf( stderr, "[audicle]: cannot start face...\n" );
        return;
    }
}


//-----------------------------------------------------------------------------
// name: ~AudicleFaceFloor()
// desc: ...
//-----------------------------------------------------------------------------
AudicleFaceFloor::~AudicleFaceFloor( ) { }




// color
static GLfloat g_grey[3] = { .5f, .5f, .5f };
static GLfloat g_red[3] = { 1.0f, .5f, .5f };
static GLfloat g_green[3] = { .5f, 1.0f, .5f };
static GLfloat g_blue[3] = { .4f, .4f, 1.0f };
static GLfloat g_yellow[3] = { 1.0f, 1.0f, .5f };
static GLfloat g_orange[3] = { 1.0f, .65f, .3f };
static GLfloat g_magenta[3] = { 1.0f, .5f, 1.0f };
static GLfloat g_uh[3] = { .5f, .8f, 1.0f };
static GLfloat g_cyan[3] = { .5f, 1.0f, 1.0f };

static GLfloat * g_colors[27] = { 
    g_grey, g_red, g_yellow, g_green, g_blue, g_orange, g_magenta, g_uh, g_cyan,
    g_grey, g_red, g_yellow, g_green, g_blue, g_orange, g_magenta, g_uh, g_cyan,
    g_grey, g_red, g_yellow, g_green, g_blue, g_orange, g_magenta, g_uh, g_cyan };
static t_CKUINT g_num_colors = sizeof(g_colors) / sizeof(GLfloat *);

// box size
static GLfloat g_size = .15f;


// randi
static t_CKINT randi( t_CKINT min, t_CKINT max )
{
    int range = max - min; 
    if ( range == 0 )
    {
        return min;
    }
    else
    {
        if( range > 0 )
        { 
            return min + (int) ( (1.0 + range) * ( ::rand()/(RAND_MAX+1.0) ) );
        }
        else
        { 
            return min - (int) ( (-range + 1.0) * ( ::rand()/(RAND_MAX+1.0) ) );
        }
    }
}

// randf
static t_CKFLOAT randf( t_CKFLOAT min, t_CKFLOAT max )
{
    return min + (max-min)*(::rand()/(t_CKFLOAT)RAND_MAX);
}



//-----------------------------------------------------------------------------
// name: init()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL AudicleFaceFloor::init( )
{
    if( !AudicleFace::init() )
        return FALSE;

    m_name = "skot machine";
    m_bg_speed = .2;
    m_credits = 0;
    m_update_credits = FALSE;
    assert( m_num_triggers > 0 );
    m_event = new Chuck_Event;
    initialize_object( m_event, &t_event );
    m_triggers = new SlotBall *[m_num_triggers];
    for( t_CKUINT i = 0; i < m_num_triggers; i++ )
    {
        m_triggers[i] = new SlotBall;
        m_triggers[i]->number = i + 1;
        m_triggers[i]->xid = IDManager::instance()->getPickID();
        m_triggers[i]->color = g_colors[i+1];

        m_triggers[i]->num_satellites = randi( g_num_colors, g_num_colors );
        m_triggers[i]->satellites = new SlotBall *[m_triggers[i]->num_satellites];
        for( t_CKUINT j = 0; j < m_triggers[i]->num_satellites; j++ )
        {
            m_triggers[i]->satellites[j] = new SlotBall;
            SlotBall * b = m_triggers[i]->satellites[j];
            b->angle = randf( 0, 360 );
            b->color = g_colors[j];
            b->radius = randf( .2f, 1.0f );
            b->size = randf( .01f, .1f );
            b->velocity = 1/::pow( b->radius * b->radius, 1.0/3 );
        }
    }
    m_last_hit = 0;
    m_bgcolor[0] = 1.0f;
    m_bgcolor[1] = 1.0f;
    m_bgcolor[2] = 1.0f;
    m_bgcolor[3] = 1.0f;

    m_quadric = gluNewQuadric();
    gluQuadricNormals( m_quadric, GLU_FLAT );
    // gluQuadricDrawStyle( m_quadric, GLU_FILL );

    m_message[0] = '\0';
    m_a = 0.0f;
    m_r = 0.0f;
    m_t = 0.0f;
    m_a_target = 0.0f;
    m_a_inc = 0.0f;
    m_t_target = 0.0f;
    m_t_inc = 0.0f;

    return TRUE;
}


//-----------------------------------------------------------------------------
// name: destroy()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL AudicleFaceFloor::destroy( )
{
    this->on_deactivate( 0.0 );
    m_id = Audicle::NO_FACE;
    m_state = INACTIVE;

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: set_credits()
// desc: ...
//-----------------------------------------------------------------------------
void AudicleFaceFloor::set_credits( t_CKINT n )
{
    m_credits = n;
    m_update_credits = TRUE;
}




//-----------------------------------------------------------------------------
// name: set_message()
// desc: ...
//-----------------------------------------------------------------------------
void AudicleFaceFloor::set_message( const string & msg )
{
    strcpy( m_message, msg.c_str() );
}




//-----------------------------------------------------------------------------
// name: set_bgcolor()
// desc: ...
//-----------------------------------------------------------------------------
void AudicleFaceFloor::set_bgcolor( t_CKFLOAT r, t_CKFLOAT g, t_CKFLOAT b )
{
    m_bg[0] = r;
    m_bg[1] = g;
    m_bg[2] = b;
}




//-----------------------------------------------------------------------------
// name: set_angle()
// desc: ...
//-----------------------------------------------------------------------------
void AudicleFaceFloor::set_angle( t_CKFLOAT r, t_CKFLOAT inc )
{
    m_a_target = (GLfloat)r;
    m_a_inc = (GLfloat)inc;
    set_rotate( 0.0 );
}




//-----------------------------------------------------------------------------
// name: set_rotate()
// desc: ...
//-----------------------------------------------------------------------------
void AudicleFaceFloor::set_rotate( t_CKFLOAT r )
{ m_r = (GLfloat)r; }




//-----------------------------------------------------------------------------
// name: set_translate()
// desc: ...
//-----------------------------------------------------------------------------
void AudicleFaceFloor::set_translate( t_CKFLOAT r, t_CKFLOAT inc )
{
    m_t_target = (GLfloat)r;
    m_t_inc = (GLfloat)inc;
}




//-----------------------------------------------------------------------------
// name: render_ball()
// desc: ...
//-----------------------------------------------------------------------------
void AudicleFaceFloor::render_ball( SlotBall * ball )
{
    t_CKUINT i;
    // set ID
    glPushName( ball->xid );

    // color
    glColor3fv( ball->color );
    // draw the thing
    glutSolidSphere( .3 * ball->size, ball->number ? 20 : 8, ball->number ? 20 : 8 );

    // satellites
    for( i = 0; i < ball->num_satellites; i++ )
    {
        SlotBall * b = ball->satellites[i];
        glPushMatrix();
        b->angle += b->velocity;
        glRotatef( b->angle, 1.0f, 0.0f, 0.0f );
        glTranslatef( 0.0, b->radius * ::pow(ball->size2, 3), 0.0f );
        render_ball( b );
        glPopMatrix();
    }

    // down
    if( ball->size > 1.0f ) ball->size *= .99f;
    if( ball->size2 > 1.0f ) ball->size2 *= .999f;

    // unset ID
    glPopName();
}




//-----------------------------------------------------------------------------
// name: render_panel()
// desc: ...
//-----------------------------------------------------------------------------
void AudicleFaceFloor::render_panel( )
{
    float x = 0;
    float y = 0;

    for( t_CKUINT i = 0; i < m_num_triggers; i++ )
    {
        glPushMatrix();
        glTranslatef( x, y, 0 );
        render_ball( m_triggers[i] );
        glPopMatrix();
        x += 2.0f / (m_num_triggers-1);
    }
}




//-----------------------------------------------------------------------------
// name: render_credits()
// desc: ...
//-----------------------------------------------------------------------------
void AudicleFaceFloor::render_credits( )
{
    char buffer[1024];
    sprintf( buffer, "[ %ld ]", m_credits );

    glPushMatrix();
    glDisable( GL_LIGHTING );
    glLineWidth( 4.0 );
    glColor4f( 1.0f-m_bg[0], 1.0f-m_bg[1], 1.0f-m_bg[2], 1.0f );
    scaleFont( .152 );
    drawString( buffer );
    glLineWidth( 1.0 );
    glEnable( GL_LIGHTING );
    glPopMatrix();

    // draw coins
    GLfloat y = 0.0f;
    GLfloat thickness = .03f;

    glColor3f( .8f, .8f, .4f );
    if( m_credits > 0 )
    {
        for( t_CKINT i = 0; i < m_credits; i++ )
        {
            glPushMatrix();
            glTranslatef( 1.5f, y, 0.0 );
            glRotatef( 90.0f, 1.0f, 0.0f, 0.0f );
            glutSolidTorus( .02f, .05f, 6, 8 );
            glPopMatrix();
            y += thickness;
        }
    }
}




//-----------------------------------------------------------------------------
// name: render_message()
// desc: ...
//-----------------------------------------------------------------------------
void AudicleFaceFloor::render_message( )
{
    glPushMatrix();
    glDisable( GL_LIGHTING );
    glLineWidth( 3.0 );
    glColor4f( 1.0f-m_bg[0], 1.0f-m_bg[1], 1.0f-m_bg[2], 1.0f );
    scaleFont( .125 );
    drawString( m_message );
    glLineWidth( 1.0 );
    glEnable( GL_LIGHTING );
    glPopMatrix();
}




//-----------------------------------------------------------------------------
// name: render()
// desc: ...
//-----------------------------------------------------------------------------
t_CKUINT AudicleFaceFloor::render( void * data )
{
    glPushMatrix();

    if( fabs( m_a_target - m_a ) > 1 )
    {
        if( m_a_target > m_a && m_a_inc > 1.0 ) m_a += m_a_target-m_a - (m_a_target-m_a) / m_a_inc;
        else if( m_a_target > m_a && m_a_inc < 1.0 ) m_a += m_a_target-m_a - (m_a_target-m_a) * m_a_inc;
        else if( m_a_target < m_a && m_a_inc > 1.0 ) m_a -= m_a-m_a_target - (m_a-m_a_target) / m_a_inc;
        else if( m_a_target < m_a && m_a_inc < 1.0 ) m_a -= m_a-m_a_target - (m_a-m_a_target) * m_a_inc;
    }

    if( fabs( m_t_target - m_t ) > .001 )
    {
        if( m_t_target > m_t && m_t_inc > 1.0 ) m_t += m_t_target-m_t - (m_t_target-m_t) / m_t_inc;
        else if( m_t_target > m_t && m_t_inc < 1.0 ) m_t += m_t_target-m_t - (m_t_target-m_t) * m_t_inc;
        else if( m_t_target < m_t && m_t_inc > 1.0 ) m_t -= m_t-m_t_target - (m_t-m_t_target) / m_t_inc;
        else if( m_t_target < m_t && m_t_inc < 1.0 ) m_t -= m_t-m_t_target - (m_t-m_t_target) * m_t_inc;
    }

    glTranslatef( 0.0f, 0.0f, m_t );
    m_a += m_r;
    glRotatef( m_a, 0.0f, 1.0f, 0.0f );

    // translate
    glTranslatef( -1, 0.2f, 0.0f );
    // render panel
    render_panel();
    glPopMatrix();

    glPushMatrix();
    glTranslatef( -.8f, -.9f, 0.0f );
    render_message();
    glPopMatrix();

    glPushMatrix();
    // translate
    glTranslatef( 0.2f, -.9f, 0.0f );
    // render credits
    render_credits();
    glPopMatrix();

    return 0;
}




//-----------------------------------------------------------------------------
// name: render_pre()
// desc: ...
//-----------------------------------------------------------------------------
void AudicleFaceFloor::render_pre()
{
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
void AudicleFaceFloor::render_post()
{
    glPopAttrib();

    AudicleFace::render_post();
}




//-----------------------------------------------------------------------------
// name: render_view()
// desc: ...
//-----------------------------------------------------------------------------
void AudicleFaceFloor::render_view( )
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
        (GLfloat) AudicleWindow::main()->m_h, .01, 100.0 );
    AudicleWindow * aW = AudicleWindow::main();
    GLfloat factor = 1.36f;
    //glOrtho(factor*-aW->m_hsize/aW->m_vsize, factor*aW->m_hsize/aW->m_vsize, -factor, factor, -10, 10 );
    // set the matrix mode to modelview
    glMatrixMode( GL_MODELVIEW );
    // load the identity matrix
    glLoadIdentity( );
    // position the view point
    gluLookAt( 0.0f, 0.0f, 3.5f, 
               0.0f, 0.0f, 0.0f, 
               0.0f, 1.0f, 0.0f );

    // set the position of the lights
    glLightfv( GL_LIGHT0, GL_POSITION, m_light0_pos );
    glLightfv( GL_LIGHT1, GL_POSITION, m_light1_pos );
}




//-----------------------------------------------------------------------------
// name: on_activate()
// desc: ...
//-----------------------------------------------------------------------------
t_CKUINT AudicleFaceFloor::on_activate()
{
    return AudicleFace::on_activate();
}




//-----------------------------------------------------------------------------
// name: on_deactivate()
// desc: ...
//-----------------------------------------------------------------------------
t_CKUINT AudicleFaceFloor::on_deactivate( t_CKDUR dur )
{
    return AudicleFace::on_deactivate( dur );
}




//-----------------------------------------------------------------------------
// name: on_event()
// desc: ...
//-----------------------------------------------------------------------------
t_CKUINT AudicleFaceFloor::on_event( const AudicleEvent & event )
{
    static t_CKBOOL m_mouse_down = FALSE;
    t_CKUINT x;

    if( event.type == ae_event_INPUT )
    {
        InputEvent * ie = (InputEvent *)event.data;
        if( ie->type == ae_input_MOUSE )
        {
            ie->popStack();

            // loop over panel
            for( x = 0; x < m_num_triggers; x++ )
            {
                if( ie->checkID( m_triggers[x]->xid ) )
                {
                    if( ie->state == ae_input_DOWN )
                    {
                        m_last_hit = x+1;
                        m_event->broadcast();
                        m_triggers[x]->size = 1.5f;
                        m_triggers[x]->size2 = 1.5f;
                    }
                }
            }

            // mouse down
            if( ie->state == ae_input_DOWN )
                m_mouse_down = TRUE;
            if( ie->state == ae_input_UP )
                m_mouse_down = FALSE;
        }
		else if( ie->type == ae_input_KEY )
		{
            m_last_hit = -1;

			switch( ie->key )
			{
				case '1':
				case '2':
				case '3':
				case '4':
				case '5':
				case '6':
				case '7':
				case '8':
				case '9':
				case '0':
                    x = ie->key - '0';
                    if( (x-1) < m_num_triggers )
                    {
                        m_last_hit = x;
                        m_triggers[x-1]->size = 1.5f;
                        m_triggers[x-1]->size2 = 1.5f;
                    }
                break;

            }

            m_event->queue_broadcast();
        }
    }

    return AudicleFace::on_event( event );
}
