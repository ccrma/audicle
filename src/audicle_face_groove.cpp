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
// name: audicle_face_groove.cpp
// desc: interface for audicle face groove
//
// authors: Ge Wang (gewang@cs.princeton.edu)
//          Perry R. Cook (prc@cs.princeton.edu)
//          Philip Davidson (philipd@cs.princeton.edu)
//          Ananya Misra (amisra@cs.princeton.edu)
// date: Autumn 2004
//-----------------------------------------------------------------------------
#include "audicle_face_groove.h"
#include "audicle_gfx.h"
#include "audicle.h"

#include <vector>
using namespace std;




//-----------------------------------------------------------------------------
// name: init()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL AntiPane::init( t_CKUINT w, t_CKUINT h )
{
    // clean up first
    cleanup();

    // allocate
    matrix = new AntiBox *[w*h];
    if( !matrix ) return FALSE;

    // make boxes
    for( t_CKUINT i = 0; i < w*h; i++ )
        // get id
        matrix[i] = new AntiBox( IDManager::instance()->getPickID() );

    // remember
    width = w;
    height = h;

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: cleanup()
// desc: ...
//-----------------------------------------------------------------------------
void AntiPane::cleanup( )
{
    if( matrix )
        SAFE_DELETE_ARRAY( matrix );

    width = 0;
    height = 0;
}




//-----------------------------------------------------------------------------
// name: AudicleFaceGroove()
// desc: ...
//-----------------------------------------------------------------------------
AudicleFaceGroove::AudicleFaceGroove( )
{
    if( !this->init( ) )
    {
        fprintf( stderr, "[audicle]: cannot start face...\n" );
        return;
    }
}




//-----------------------------------------------------------------------------
// name: ~AudicleFaceGroove()
// desc: ...
//-----------------------------------------------------------------------------
AudicleFaceGroove::~AudicleFaceGroove( ) { }




//-----------------------------------------------------------------------------
// name: init()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL AudicleFaceGroove::init( )
{
    if( !AudicleFace::init() )
        return FALSE;

    m_name = "Non-Specific";
    m_bg_speed = .2;

    // initialize pane
    m_pane = new AntiPane;
    // arbitrary dimensions for now
    m_pane->init( 8, 4 );

    // initialize panel
    m_panel = new AntiPane;
    // 8 for now
    m_panel->init( 9, 1 );
    // set values
    for( t_CKUINT i = 0; i < m_panel->width; i++ )
        m_panel->matrix[i]->value = i;
    // select
    m_panel->matrix[0]->mode = 1;
    
    return TRUE;
}




//-----------------------------------------------------------------------------
// name: destroy()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL AudicleFaceGroove::destroy( )
{
    this->on_deactivate( 0.0 );
    m_id = Audicle::NO_FACE;
    m_state = INACTIVE;

    return TRUE;
}


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

static GLfloat * g_colors[9] = { 
    g_grey, g_red, g_green, g_blue, g_yellow, g_orange, g_magenta, g_uh, g_cyan };
static t_CKUINT g_num_colors = sizeof(g_colors) / sizeof(GLfloat *);

// box size
static GLfloat g_size = .15f;



//-----------------------------------------------------------------------------
// name: render_box()
// desc: ...
//-----------------------------------------------------------------------------
void AudicleFaceGroove::render_box( AntiBox * box )
{
    GLfloat size;
    // set ID
    glPushName( box->xid );
    // set color
    glColor3fv( g_colors[box->value%g_num_colors] );

    glPushMatrix();
    if( box->glow > .92f && box->mode != 1 ) glScalef( 1.1f, 1.1f, 1.0f );
    // draw inner box
    glBegin( GL_QUADS );
        glNormal3f( 0.0f, 0.0f, 1.0f );
        glVertex2f( g_size, g_size );
        glVertex2f( -g_size, g_size );
        glVertex2f( -g_size, -g_size );
        glVertex2f( g_size, -g_size );
    glEnd();
    // draw inner box
    glDisable( GL_LIGHTING );
    glTranslatef( 0.0f, 0.0f, 0.005f );
    glLineWidth( 2.0f );
    glColor3f( 1.0f, 1.0f, 1.0f );
    glBegin( GL_LINE_LOOP );
        glNormal3f( 0.0f, 0.0f, 1.0f );
        glVertex2f( g_size, g_size );
        glVertex2f( -g_size, g_size );
        glVertex2f( -g_size, -g_size );
        glVertex2f( g_size, -g_size );
    glEnd();
    glPopMatrix();

    // select
    if( box->mode == 1 )
        box->glow = 1.0f;
    // draw outer box
    if( box->glow > 0.0f )
    {
        GLfloat r, g, b;
        GLfloat * c = g_green;
        r = c[0] * box->glow;
        g = c[1] * box->glow;
        b = c[2] * box->glow;
        glColor3f( r, g, b );
        // box->glow -= .005f;
        box->glow -= .004f;
    }
    else
    {
        // black
        glColor3f( 0.0f, 0.0f, 0.0f );
    }
    glTranslatef( 0.0f, 0.0f, -0.02f );
    size = g_size * 1.2f;
    glBegin( GL_QUADS );
        glNormal3f( 0.0f, 0.0f, 1.0f );
        glVertex2f( size, size );
        glVertex2f( -size, size );
        glVertex2f( -size, -size );
        glVertex2f( size, -size );
    glEnd();

    // unset ID
    glPopName();
}




//-----------------------------------------------------------------------------
// name: render_pane()
// desc: ...
//-----------------------------------------------------------------------------
void AudicleFaceGroove::render_pane( )
{
    GLfloat xinc = .4f;
    GLfloat yinc = .4f;

    // loop over boxes
    for( t_CKUINT y = 0; y < m_pane->height; y++ )
    {
        for( t_CKUINT x = 0; x < m_pane->width; x++ )
        {
            glPushMatrix();
            glTranslatef( x * xinc, -(y * yinc), 0.0f );
            glPushMatrix();
            render_box( m_pane->matrix[x+m_pane->width*y] );
            glPopMatrix();
            glPopMatrix();
        }
    }
}




//-----------------------------------------------------------------------------
// name: render_panel()
// desc: ...
//-----------------------------------------------------------------------------
void AudicleFaceGroove::render_panel( )
{
    GLfloat xinc = .35f;
    char buffer[32];

    for( t_CKUINT x = 1; x < m_panel->width; x++ )
    {
        glPushMatrix();
        glTranslatef( (x-1) * xinc, 0.0f, 0.0f );
        glPushMatrix();
        render_box( m_panel->matrix[x] );
        glPopMatrix();
        glPushMatrix();
        glTranslatef( 0.0f, -0.3f, 1.0f );
        glDisable( GL_LIGHTING );
        glColor4f( 1.0f, 1.0f, 1.0f, 1.0f );
        scaleFont( .052 );
        sprintf( buffer, "%ld", x );
        drawString( buffer );
        glEnable( GL_LIGHTING );
        glPopMatrix();        
        glPopMatrix();
    }

    // 0
    glPushMatrix();
    glTranslatef( m_panel->width * xinc - xinc, 0.0f, 0.0f );
    glPushMatrix();
    render_box( m_panel->matrix[0] );
    glPopMatrix();
    glPushMatrix();
    glTranslatef( 0.0f, -0.3f, 1.0f );
    glDisable( GL_LIGHTING );
    glColor4f( 1.0f, 1.0f, 1.0f, 1.0f );
    scaleFont( .055 );
    sprintf( buffer, "%d", 0 );
    drawString( buffer );
    glEnable( GL_LIGHTING );
    glPopMatrix();
    glPopMatrix();
}




//-----------------------------------------------------------------------------
// name: render()
// desc: ...
//-----------------------------------------------------------------------------
t_CKUINT AudicleFaceGroove::render( void * data )
{
    glPushMatrix();
    // translate
    glTranslatef( -1.4f, 1.00f, 0.0f );
    // render pane
    render_pane();
    glPopMatrix();

    glPushMatrix();
    // translate
    glTranslatef( -1.4f, -.75f, 0.0f );
    // render panel
    render_panel();
    glPopMatrix();

    return 0;
}




//-----------------------------------------------------------------------------
// name: render_pre()
// desc: ...
//-----------------------------------------------------------------------------
void AudicleFaceGroove::render_pre()
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
void AudicleFaceGroove::render_post()
{
    glPopAttrib();

    AudicleFace::render_post();
}




//-----------------------------------------------------------------------------
// name: render_view()
// desc: ...
//-----------------------------------------------------------------------------
void AudicleFaceGroove::render_view( )
{
    // set the matrix mode to project
    glMatrixMode( GL_PROJECTION );
    // load the identity matrix
    // load the identity matrix XXX
    // this is handled by AudicleWindow, in order to set up pick matrices...
    // you can assume that LoadIdentity has been called already
    //glLoadIdentity( ); 
    // create the viewing frustum
    //gluPerspective( 45.0, (GLfloat) AudicleWindow::main()->m_w / 
    //    (GLfloat) AudicleWindow::main()->m_h, .01, 100.0 );
    AudicleWindow * aW = AudicleWindow::main();
    GLfloat factor = 1.36f;
    glOrtho(factor*-aW->m_hsize/aW->m_vsize, factor*aW->m_hsize/aW->m_vsize, -factor, factor, -10, 10 );
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
t_CKUINT AudicleFaceGroove::on_activate()
{
    return AudicleFace::on_activate();
}




//-----------------------------------------------------------------------------
// name: on_deactivate()
// desc: ...
//-----------------------------------------------------------------------------
t_CKUINT AudicleFaceGroove::on_deactivate( t_CKDUR dur )
{
    return AudicleFace::on_deactivate( dur );
}




//-----------------------------------------------------------------------------
// name: on_event()
// desc: ...
//-----------------------------------------------------------------------------
t_CKUINT AudicleFaceGroove::on_event( const AudicleEvent & event )
{
    static t_CKBOOL m_mouse_down = FALSE;
    t_CKUINT x, y;

    if( event.type == ae_event_INPUT )
    {
        InputEvent * ie = (InputEvent *)event.data;
        if( ie->type == ae_input_MOUSE )
        {
            ie->popStack();

            // loop over panel
            for( x = 0; x < m_panel->width; x++ )
            {
                if( ie->checkID( m_panel->matrix[x]->xid ) )
                {
                    if( ie->state == ae_input_DOWN )
                    {
                        m_panel->clear();
                        m_panel->matrix[x]->mode = 1;
                        m_panel->which = x;
                    }
                }
            }

            // loop over pane
            for( y = 0; y < m_pane->height; y++ )
            {
                for( x = 0; x < m_pane->width; x++ )
                {
                    if( ie->checkID( m_pane->matrix[x+m_pane->width*y]->xid ) )
                    {
                        if( ie->state == ae_input_DOWN )
                        {
                            m_pane->clear();
                            m_pane->matrix[x+m_pane->width*y]->value = m_panel->which;
                        }
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
                    if( x < m_panel->width )
                    {
                        m_panel->clear();
                        m_panel->matrix[x]->mode = 1;
                        m_panel->which = x;
                    }
                break;
				case 27:
                    x = 0;
                    m_panel->clear();
                    m_panel->matrix[x]->mode = 1;
                    m_panel->which = x;
                break;
                case 'A':
                    m_pane->reset( m_panel->which );
                break;
                case 'C':
                    m_pane->reset( 0 );
                break;
            }
        }
        else if( ie->type == ae_input_MOTION )
        {
/*            ie->popStack();

            // loop over panel
            for( x = 0; x < m_panel->width; x++ )
            {
                if( ie->checkID( m_panel->matrix[x]->xid ) )
                {
                    m_panel->clear();
                    m_panel->matrix[x]->mode = 1;
                    m_panel->which = x;
                }
            }
*/
        }
    }

    return AudicleFace::on_event( event );
}
