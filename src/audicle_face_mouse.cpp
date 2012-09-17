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
// name: audicle_face_mouse.cpp
// desc: interface for audicle face mouse
//
// authors: Ge Wang (gewang@cs.princeton.edu)
//          Perry R. Cook (prc@cs.princeton.edu)
//          Philip Davidson (philipd@cs.princeton.edu)
//          Ananya Misra (amisra@cs.princeton.edu)
// date: Autumn 2004
//-----------------------------------------------------------------------------
#include "audicle_face_mouse.h"
#include "audicle_gfx.h"
#include "audicle.h"

#include <vector>
using namespace std;

// factor
static GLfloat g_factor = .7f;
static GLfloat g_far = 3.5f;
static GLfloat g_angle = 45.0f;

// global texture variables and functions
void glu_init(); // function to init glu, obviously

#define WIDTH 64 // width of texture grid
#define HEIGHT 64 // height of texture grid

enum { IMG_LEFT, IMG_UP, IMG_RIGHT, IMG_DOWN }; 
unsigned char g_texture[1][WIDTH * HEIGHT * 4]; // texture grid itself 
GLuint g_img[1]; // texture images


//-----------------------------------------------------------------------------
// name: init()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL MousePane::init( t_CKUINT w, t_CKUINT h )
{
    // clean up first
    cleanup();

    // allocate
    matrix = new MouseBox *[w*h];
    if( !matrix ) return FALSE;

    // color
    t_CKINT color = 0;

    // make boxes
    for( t_CKUINT i = 0; i < w*h; i++ )
    {
        if( i % w ) color = !color;
        // get id
        matrix[i] = new MouseBox( IDManager::instance()->getPickID() );
        // set
        matrix[i]->value = color;
        matrix[i]->x = i % w;
        matrix[i]->y = i / w;
    }

    // remember
    width = w;
    height = h;

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: cleanup()
// desc: ...
//-----------------------------------------------------------------------------
void MousePane::cleanup( )
{
    if( matrix )
        SAFE_DELETE_ARRAY( matrix );

    width = 0;
    height = 0;
}




//-----------------------------------------------------------------------------
// name: AudicleFaceMouse()
// desc: ...
//-----------------------------------------------------------------------------
AudicleFaceMouse::AudicleFaceMouse( )
{
    if( !this->init( ) )
    {
        fprintf( stderr, "[audicle]: cannot start face...\n" );
        return;
    }
}




//-----------------------------------------------------------------------------
// name: ~AudicleFaceMouse()
// desc: ...
//-----------------------------------------------------------------------------
AudicleFaceMouse::~AudicleFaceMouse( ) { }




static GLUquadricObj * g_quadric;
//-----------------------------------------------------------------------------
// name: init()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL AudicleFaceMouse::init( )
{
    if( !AudicleFace::init() )
        return FALSE;

    m_name = "ChucK ChucK Rocket";
    m_bg_speed = .2;

    // initialize pane
    m_pane = new MousePane;
    // arbitrary dimensions for now
    m_pane->init( 12, 9 );

    // set portal
    m_pane->matrix[10 + 4 * 12]->portal = &m_portal_out;
    m_portal_out.owner = m_pane->matrix[10 + 4 * 12];
    m_pane->matrix[1 + 4 * 12]->portal = &m_portal_in;
    m_portal_in.owner = m_pane->matrix[1 + 4 * 12];

    // initialize panel
    m_panel = new MouseBox;

    // grid
    m_grid = FALSE;
    // box
    m_box = NULL;
    // teleport in
    m_teleport_in = NULL;
    // teleport out
    m_teleport_out = NULL;

    // quadric
    if( !g_quadric )
    {
        g_quadric = gluNewQuadric();
        gluQuadricNormals( g_quadric, GLU_FLAT );
        gluQuadricDrawStyle( g_quadric, GLU_FILL );
		gluQuadricTexture( g_quadric, GL_TRUE );
    }

	// textures
	glu_init();

    // go
    m_selected = 0;

    // hit
    m_num_items = 6;
    m_item_cache = new t_CKINT[m_num_items];
    for( t_CKINT i = 0; i < m_num_items; i++ )
        m_item_cache[i] = 0;
        
    // volume
    m_volume = 1.0;
    m_volume2 = 0.0;

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: destroy()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL AudicleFaceMouse::destroy( )
{
    this->on_deactivate( 0.0 );
    m_id = Audicle::NO_FACE;
    m_state = INACTIVE;

    return TRUE;
}


// color
static GLfloat g_grey[3] = { .75f, .75f, .75f };
static GLfloat g_white[3] = { 1.0f, 1.0f, 1.0f };
static GLfloat g_red[3] = { 1.0f, .5f, .5f };
static GLfloat g_green[3] = { .5f, 1.0f, .5f };
static GLfloat g_blue[3] = { .4f, .4f, 1.0f };
static GLfloat g_yellow[3] = { 1.0f, 1.0f, .5f };
static GLfloat g_orange[3] = { 1.0f, .65f, .3f };
static GLfloat g_magenta[3] = { 1.0f, .5f, 1.0f };
static GLfloat g_uh[3] = { .5f, .8f, 1.0f };
static GLfloat g_cyan[3] = { .5f, 1.0f, 1.0f };
static GLfloat g_pink[3] = { 1.0, .8f, .8f }; // not in g_colors :(

static GLfloat * g_colors[10] = { 
    g_grey, g_white, g_red, g_green, g_blue, g_yellow, g_orange, g_magenta, g_uh, g_cyan };
static t_CKUINT g_num_colors = sizeof(g_colors) / sizeof(GLfloat *);

// box size
static GLfloat g_size = .13f;



//-----------------------------------------------------------------------------
// name: render_box()
// desc: ...
//-----------------------------------------------------------------------------
void AudicleFaceMouse::render_box( MouseBox * box )
{
    // set ID
    glPushName( box->xid );

    // set color
    glColor3fv( g_colors[box->value%g_num_colors] );

	// get direction
	bool arrow;
	if( box->dir != -1 )
		arrow = true;
	
    glPushMatrix();
    
	// bind texture
	if( arrow && g_img[0] )
    {
        glBindTexture( GL_TEXTURE_2D, g_img[0] );
        glEnable( GL_TEXTURE_2D );              
	}

	// draw inner box
    glBegin( GL_QUADS );
        glNormal3f( 0.0f, 0.0f, 1.0f );
		if( arrow ) 
		{
			if( box->dir == IMG_DOWN ) glTexCoord2f( 1.0f, 1.0f );
			else if( box->dir == IMG_RIGHT ) glTexCoord2f( 1.0f, 0.0f );
			else if( box->dir == IMG_UP ) glTexCoord2f( 0.0f, 0.0f );
			else if( box->dir == IMG_LEFT ) glTexCoord2f( 0.0f, 1.0f );
		}
        glVertex2f( g_size, g_size );
		if( arrow ) 
		{
			if( box->dir == IMG_DOWN ) glTexCoord2f( 0.0f, 1.0f );
			else if( box->dir == IMG_RIGHT ) glTexCoord2f( 1.0f, 1.0f );
			else if( box->dir == IMG_UP ) glTexCoord2f( 1.0f, 0.0f );
			else if( box->dir == IMG_LEFT ) glTexCoord2f( 0.0f, 0.0f );
        }
		glVertex2f( -g_size, g_size );
		if( arrow )
		{
			if( box->dir == IMG_DOWN ) glTexCoord2f( 0.0f, 0.0f );
			else if( box->dir == IMG_RIGHT ) glTexCoord2f( 0.0f, 1.0f );
			else if( box->dir == IMG_UP ) glTexCoord2f( 1.0f, 1.0f );
			else if( box->dir == IMG_LEFT ) glTexCoord2f( 1.0f, 0.0f );
		}
		glVertex2f( -g_size, -g_size );
		if( arrow ) 
		{
			if( box->dir == IMG_DOWN ) glTexCoord2f( 1.0f, 0.0f );
			else if( box->dir == IMG_RIGHT ) glTexCoord2f( 0.0f, 0.0f );
			else if( box->dir == IMG_UP ) glTexCoord2f( 0.0f, 1.0f );
			else if( box->dir == IMG_LEFT ) glTexCoord2f( 1.0f, 1.0f );
		}
		glVertex2f( g_size, -g_size );
    glEnd();

	// unbind texture?
	if( arrow && g_img[0] )
    {
        glDisable( GL_TEXTURE_2D );
    }

    // grid
    if( box->mode )
    {
        // draw inner box
        glTranslatef( 0.0f, 0.0f, 0.005f );
        glLineWidth( 2.0f );
        glColor3f( .25f, .25f, .25f );
        glBegin( GL_LINE_LOOP );
            glNormal3f( 0.0f, 0.0f, 1.0f );
            glVertex2f( g_size, g_size );
            glVertex2f( -g_size, g_size );
            glVertex2f( -g_size, -g_size );
            glVertex2f( g_size, -g_size );
        glEnd();
        glLineWidth( 1.0f );
    }

    // portal
    if( box->portal )
    {
        // get it
        MouseItem * item = box->portal;
        // rotate it
        glRotatef( item->m_rotate, 0.0, 0.0, 1.0f );
        item->m_rotate += .5f;
        // draw
        glEnable( GL_LIGHTING );
        item->draw();
        glDisable( GL_LIGHTING );
    }

    // item
    if( box->item > 0 )
    {
        // get it
        MouseItem * item = box->things[box->item];
        // raise it
        glTranslatef( 0.0f, 0.0f, .1f * g_factor + .01f );
        // rotate it
        glRotatef( item->m_rotate, 0.0, 0.0, 1.0f );
        item->m_rotate += 1.0f;
        // draw
        glEnable( GL_LIGHTING );
        item->draw();
        glDisable( GL_LIGHTING );
    }

	glPopMatrix();

    // unset ID
    glPopName();
}




//-----------------------------------------------------------------------------
// name: render_pane()
// desc: ...
//-----------------------------------------------------------------------------
void AudicleFaceMouse::render_pane( )
{
    GLfloat xinc = g_size*2;
    GLfloat yinc = g_size*2;

    // no lighting
    glDisable( GL_LIGHTING );

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

    // lighting
    glEnable( GL_LIGHTING );
}




//-----------------------------------------------------------------------------
// name: render_panel()
// desc: ...
//-----------------------------------------------------------------------------
void AudicleFaceMouse::render_panel( )
{
    GLfloat xinc = .35f;
    char buffer[32];
    MouseItem * item = NULL;

    // no lighting
    glEnable( GL_LIGHTING );

    for( t_CKUINT x = 1; x < m_panel->num_things; x++ )
    {
        glPushMatrix();
        glTranslatef( 0.0f, (x-1) * -xinc, 0.0f );

        glPushMatrix();
        glDisable( GL_LIGHTING );
        glTranslatef( 0.2f, 0.0f, 0.0f );
        glColor4f( 1.0f, 1.0f, 1.0f, 1.0f );
        scaleFont( .052 );
        sprintf( buffer, "%ld", x );
        drawString( buffer );
        glEnable( GL_LIGHTING );
        glPopMatrix();

        item = m_panel->things[x];
        // rotate it
        glRotatef( item->m_rotate, 0.0, 0.0, 1.0f );
        item->m_rotate += .5f;
        
        if( m_selected == x ) glScalef( 1.5f / g_factor, 1.5f / g_factor, 1.5f / g_factor );
        else glScalef( 1.0f / g_factor, 1.0f / g_factor, 1.0f / g_factor );
        // draw
        item->draw();

        glPopMatrix();
    }

    // lighting
    glDisable( GL_LIGHTING );
}




//-----------------------------------------------------------------------------
// name: render_mice()
// desc: ...
//-----------------------------------------------------------------------------
void AudicleFaceMouse::render_mice()
{
    m_mutex.acquire();
    Mouse * mouse = NULL;

    // lighting
    glEnable( GL_LIGHTING );

    t_CKFLOAT t = AudicleWindow::main()->get_current_time( TRUE );
    t_CKFLOAT alpha = (t - t_start) / (t_end - t_start);
    if( alpha > 1.0 ) alpha = 1.0;

    float x;
    float y;

    // loop over
    for( t_CKINT i = 0; i < m_mice.size(); i++ )
    {
        // push
        glPushMatrix();
        // get
        mouse = m_mice[i];
        // interp
        x = mouse->x_start + alpha * (mouse->x_end - mouse->x_start);
        y = mouse->y_start + alpha * (mouse->y_end - mouse->y_start);
        // translate
        glTranslatef( mouse->x = 2 * x * g_size, mouse->y = 2 * -y * g_size, 0.09f );
        // orient
        glRotatef( -90 * (mouse->dir+1), 0.0f, 0.0f, 1.0f );
        // draw
        mouse->draw();
        // rotate
        mouse->m_rotate += .1f;
        
        glPopMatrix();
    }

    // loop over floating
    m_keep.clear();
    m_bye.clear();
    for( t_CKINT j = 0; j < m_floating.size(); j++ )
    {
        mouse = m_floating[j];
        if( mouse->t_done > t )
        {
            glPushMatrix();
            glTranslatef( mouse->x, mouse->y, 8 - (mouse->t_done - t) );
            glRotatef( -90 * (mouse->dir+1), 0.0f, 0.0f, 1.0f );
            mouse->draw();
            mouse->m_rotate += .1f;
            m_keep.push_back( mouse );
            glPopMatrix();
        }
        else
        {
            m_bye.push_back( mouse );
        }
    }

    m_floating = m_keep;    
    // delete
    for( t_CKINT l = 0; l < m_bye.size(); l++ )
        delete m_bye[l];
        
    m_bye.clear();

    // lighting
    glDisable( GL_LIGHTING );

    m_mutex.release();
}




//-----------------------------------------------------------------------------
// name: render()
// desc: ...
//-----------------------------------------------------------------------------
t_CKUINT AudicleFaceMouse::render( void * data )
{
    glPushMatrix();
    // translate
    glTranslatef( -1.65f, 1.1f, 0.0f );
    // render pane
    glPushMatrix();
    render_pane();
    glPopMatrix();
    // render mice
    glPushMatrix();
    render_mice();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    // translate
    glTranslatef( 1.55f, 1.1f, 0.0f );
    // render panel
    render_panel();
    glPopMatrix();
    
    char buffer[32];
    glPushMatrix();
    glDisable( GL_LIGHTING );
    glTranslatef( 1.4f, -0.7f, 0.0f );
    glColor4f( 1.0f, 1.0f, 1.0f, 1.0f );
    scaleFont( .052 );
    sprintf( buffer, "volume: %.3f", m_volume );
    drawString( buffer );
    glPopMatrix();
    glPushMatrix();
    glTranslatef( 1.4f, -.8f, 0.0f );
    scaleFont( .052 );
    sprintf( buffer, "drone: %.3f", m_volume2 );
    drawString( buffer );
    glEnable( GL_LIGHTING );
    glPopMatrix();

    return 0;
}




//-----------------------------------------------------------------------------
// name: add_mouse()
// desc: ...
//-----------------------------------------------------------------------------
void AudicleFaceMouse::add_mouse( int from, int note )
{
    // new mouse
    Mouse * mouse = new Mouse;
    mouse->note = note;

    // where
    if( from )
    {
        mouse->curr = m_portal_out.owner;
        mouse->dir = 0;
        mouse->x_start = m_portal_out.owner->x;
        mouse->y_start = m_portal_out.owner->y;
        mouse->x_end = mouse->x_start;
        mouse->y_end = mouse->y_start;
    }
    else
    {
        mouse->curr = m_portal_in.owner;
        mouse->dir = 2;
        mouse->x_start = m_portal_in.owner->x;
        mouse->y_start = m_portal_in.owner->y;
        mouse->x_end = mouse->x_start;
        mouse->y_end = mouse->y_start;
    }

    // add
    m_mice.push_back( mouse );
}




//-----------------------------------------------------------------------------
// name: move()
// desc: ...
//-----------------------------------------------------------------------------
t_CKINT AudicleFaceMouse::move( Mouse * mouse )
{
    assert( mouse->dir >= 0 );

    if( mouse->dest )
    {
        if( mouse->dest->portal )
            if( mouse->dest == m_portal_in.owner ) return -1;
            else return -2;
        mouse->curr = mouse->dest;
        mouse->x_start = mouse->dest->x;
        mouse->y_start = mouse->dest->y;
        if( mouse->curr->item )
            m_item_cache[mouse->curr->item] = mouse->note;
    }

    // curr location
    MouseBox * box = mouse->curr;
    // coorindate
    t_CKINT x = box->x;
    t_CKINT y = box->y;

    // change dir
    if( box->dir >= 0 ) mouse->dir = box->dir;

    // figure out next
    if( mouse->dir == 0 ) // left
    {
        if( x == 0 ) // on left edge
        {
            if( y == 0 ) // top edge
            {
                y++; // move
                mouse->dir = 3; // down
            }
            else
            {
                y--; // move
                mouse->dir = 1; // up
            }
        }
        else // move left
        {
            x--;
        }
    }
    // figure out next
    else if( mouse->dir == 1 ) // up
    {
        if( y == 0 ) // on top
        {
            if( x == m_pane->width - 1 ) // right edge
            {
                x--; // move
                mouse->dir = 0; // left
            }
            else
            {
                x++; // move
                mouse->dir = 2; // right
            }
        }
        else // move up
        {
            y--;
        }
    }
    // figure out next
    else if( mouse->dir == 2 ) // right
    {
        if( x == m_pane->width - 1 ) // on right edge
        {
            if( y == m_pane->height - 1 ) // bottom
            {
                y--; // move
                mouse->dir = 1; // up
            }
            else
            {
                y++; // move
                mouse->dir = 3; // down
            }
        }
        else // move right
        {
            x++;
        }
    }
    // figure out next
    else if( mouse->dir == 3 ) // down
    {
        if( y == m_pane->height - 1 ) // on bottom edge
        {
            if( x == 0 ) // left
            {
                x++; // move
                mouse->dir = 2; // right
            }
            else
            {
                x--; // move
                mouse->dir = 0; // left
            }
        }
        else // move down
        {
            y++;
        }
    }

    mouse->dest = m_pane->matrix[x + y * m_pane->width];
    mouse->x_end = mouse->dest->x;
    mouse->y_end = mouse->dest->y;

    // there
    if( mouse->dest->mouse )
        return 0;
    else
    {
        mouse->dest->mouse = mouse;
        return 1;
    }
}




//-----------------------------------------------------------------------------
// name: go()
// desc: ...
//-----------------------------------------------------------------------------
void AudicleFaceMouse::go( t_CKFLOAT duration )
{
    m_mutex.acquire();

    // evict
    m_pane->evict();
    m_keep.clear();
    m_bye.clear();

    t_CKINT val;

    // current time
    t_start = AudicleWindow::main()->get_current_time( TRUE );
    t_end = t_start + duration;

    // compute
    for( t_CKUINT i = 0; i < m_mice.size(); i++ )
    {
        // move it
        val = move( m_mice[i] );
        if( val == 1 ) // keep
            m_keep.push_back( m_mice[i] );
        else if( val == -1 && m_teleport_in == NULL ) // hmm
            m_teleport_in = m_mice[i];
        else if( val == -2 && m_teleport_out == NULL )
            m_teleport_out = m_mice[i];
        else // done
            m_bye.push_back( m_mice[i] );
    }

    // copy
    m_mice = m_keep;
    // delete
    for( t_CKUINT j = 0; j < m_bye.size(); j++ )
        delete m_bye[j];

    m_mutex.release();
}




//-----------------------------------------------------------------------------
// name: remove_all()
// desc: ...
//-----------------------------------------------------------------------------
void AudicleFaceMouse::remove_all()
{
    m_mutex.acquire();
    
    t_CKFLOAT t = AudicleWindow::main()->get_current_time( TRUE );

    // delete
    for( t_CKUINT i = 0; i < m_mice.size(); i++ )
    {
        m_mice[i]->t_done = t + 8;
        m_floating.push_back( m_mice[i] );
    }
        
    m_mice.clear();
    
    m_mutex.release();
}


//-----------------------------------------------------------------------------
// name: render_pre()
// desc: ...
//-----------------------------------------------------------------------------
void AudicleFaceMouse::render_pre()
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
    // normalize
    glEnable( GL_NORMALIZE );
    
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
void AudicleFaceMouse::render_post()
{
    glPopAttrib();

    AudicleFace::render_post();
}




//-----------------------------------------------------------------------------
// name: render_view()
// desc: ...
//-----------------------------------------------------------------------------
void AudicleFaceMouse::render_view( )
{
    // set the matrix mode to project
    glMatrixMode( GL_PROJECTION );
    // load the identity matrix
    // load the identity matrix XXX
    // this is handled by AudicleWindow, in order to set up pick matrices...
    // you can assume that LoadIdentity has been called already
    //glLoadIdentity( ); 
    // create the viewing frustum
    gluPerspective( g_angle, (GLfloat) AudicleWindow::main()->m_w / 
        (GLfloat) AudicleWindow::main()->m_h, .005, 100.0 );
    AudicleWindow * aW = AudicleWindow::main();
    GLfloat factor = 1.36f;
    // glOrtho(factor*-aW->m_hsize/aW->m_vsize, factor*aW->m_hsize/aW->m_vsize, -factor, factor, -10, 10 );
    // set the matrix mode to modelview
    glMatrixMode( GL_MODELVIEW );
    // load the identity matrix
    glLoadIdentity( );
    // position the view point
    gluLookAt( 0.0f, g_far * sin( m_eye_y ), g_far * cos( m_eye_y ), 
               0.0f, 0.0f, 0.0f, 
               0.0f, cos( m_eye_y ) < 0.0f ? -1.0f : 1.0f, 0.0f );

    // set the position of the lights
    glLightfv( GL_LIGHT0, GL_POSITION, m_light0_pos );
    glLightfv( GL_LIGHT1, GL_POSITION, m_light1_pos );
}




//-----------------------------------------------------------------------------
// name: on_activate()
// desc: ...
//-----------------------------------------------------------------------------
t_CKUINT AudicleFaceMouse::on_activate()
{
    return AudicleFace::on_activate();
}




//-----------------------------------------------------------------------------
// name: on_deactivate()
// desc: ...
//-----------------------------------------------------------------------------
t_CKUINT AudicleFaceMouse::on_deactivate( t_CKDUR dur )
{
    return AudicleFace::on_deactivate( dur );
}




// cube
void MouseItemCube::draw()
{
    // color
    glColor3fv( g_red );
    // draw
    glutSolidCube( .1f * g_factor );
}

// cone
void MouseItemCone::draw()
{
    // color
    glColor3fv( g_orange );
    // draw
    glTranslatef( 0.0f, 0.0f, -0.04f * g_factor );
    glutSolidCone( .08f * g_factor, .13f * g_factor, 5, 5 );
}

// cylinder
void MouseItemCylinder::draw()
{
    // color
    glColor3fv( g_blue );
    // draw
    glTranslatef( 0.0f, 0.0f, -0.04f * g_factor );
    gluCylinder( g_quadric, .08 * g_factor, .08 * g_factor, .08 * g_factor, 5, 5 );
    gluCylinder( g_quadric, .04 * g_factor, .04 * g_factor, .08 * g_factor, 5, 5 );
}

// torus
void MouseItemTorus::draw()
{
    // color
    glColor3fv( g_yellow );
    // draw
    glutSolidTorus( .03 * g_factor, .06 * g_factor, 10, 10 );
}

// sphere
void MouseItemSphere::draw()
{
    // color
    glColor3fv( g_green );
    // draw
    glutSolidSphere( .065f * g_factor, 6, 6 );
}

// portal in
void MouseItemPortalIn::draw()
{
    // color
    glColor3fv( g_red );
    // draw
    glutWireSphere( g_size, 7, 7 );
}

// portal out
void MouseItemPortalOut::draw()
{
    // color
    glColor3fv( g_green );
    // line width
    glLineWidth( 2.0f );

    GLfloat r, g, b;

    glTranslatef( 0.0f, 0.0f, .04f );

    glDisable( GL_LIGHTING );

    // draw
    r = .5 * (1+sin( 3 * .01 * m_rotate ));
    g = .5 * (1+sin( 2 * .01 * r1 ));
    b = .5 + (1+sin( 1.1 * .01 * r2 ));
    glColor3f( r, g, b );
    glutWireSphere( g_size, 7, 7 );

    // line width
    glLineWidth( 1.0f );
    
    r = .5 * (1+sin( .01 * m_rotate ));
    g = .5 * (1+sin( .01 * r1 ));
    b = .5 * (1+sin( .01 * r2 ));
    glColor3f( r, g, b );
    glRotatef( 2 * r1, 0.0f, 1.0f, 0.0f );
    glutWireSphere( .6 * g_size, 5, 5 );

    glEnable( GL_LIGHTING );

    glRotatef( 2 * r2, 1.0f, 0.0f, 0.0f );
    glutSolidSphere( .3 * g_size, 10, 10 );

    r1 -= .5f;
    r2 -= .8f;
}

// mouse
void Mouse::draw()
{
    glRotatef( 10 * sin(m_rotate), 0.0f, 0.0f, 1.0f );
    // body
    glColor3fv( g_white );
    glutSolidSphere( .55 * g_size, 8, 8 );

    // left ear
    glPushMatrix();
    glColor3fv( g_blue );
    glRotatef( -35.0f, 1.0f, .1f, 0.1f );
    glTranslatef( .4f * g_size, 0.0f, .4f * g_size );
    glScalef( 1.0f, .1f, 1.0f );
    glutSolidSphere( .4 * g_size, 5, 5 );
    glPopMatrix();

    // right ear
    glPushMatrix();
    glColor3fv( g_blue );
    glRotatef( -35.0f, 1.0f, .1f, 0.1f );
    glTranslatef( -.4f * g_size, 0.0f, .4f * g_size );
    glScalef( 1.0f, .1f, 1.0f );
    glutSolidSphere( .4 * g_size, 5, 5 );
    glPopMatrix();

    // left eye
    glPushMatrix();
    glColor3f( 0.0f, 0.0f, 0.0f );
    glRotatef( -15.0f, 1.0f, 0.0f, 0.0f );
    glTranslatef( -.2f * g_size, -.55f * g_size, .1f * g_size );
    glScalef( .3f, .05f, 1.0f );
    glutSolidSphere( .2 * g_size, 5, 5 );
    glPopMatrix();

    // right eye
    glPushMatrix();
    glColor3f( 0.0f, 0.0f, 0.0f );
    glRotatef( -15.0f, 1.0f, 0.0f, 0.0f );
    glTranslatef( .2f * g_size, -.55f * g_size, .1f * g_size );
    glScalef( .3f, .05f, 1.0f );
    glutSolidSphere( .2 * g_size, 5, 5 );
    glPopMatrix();

	// left foot
	glPushMatrix();
	glColor3fv( g_pink );
	glRotatef( 45.0f, 1.0f, 0.0f, 0.0f );
	glTranslatef( -.35f * g_size, -.4f * g_size, -.4f * g_size );
	glScalef( .5f, .5f, .5f );
	glutSolidSphere( .4 * g_size, 5, 5 );
	glPopMatrix();

	// right foot
	glPushMatrix();
	glColor3fv( g_pink );
	glRotatef( 45.0f, 1.0f, 0.0f, 0.0f );
	glTranslatef( .35f * g_size, -.4f * g_size, -.4f * g_size );
	glScalef( .5f, .5f, .5f );
	glutSolidSphere( .4 * g_size, 5, 5 );
	glPopMatrix();

	// tail?
	int tailpoints = 10;
	glPushMatrix();
	glColor3fv( g_blue );
	glTranslatef( 0, .4f*g_size, -.2f*g_size );
	glLineWidth( 4.0f );
	glBegin( GL_LINE_STRIP );
	for(int t = 0; t < tailpoints; t++ )
		glVertex3f( .04f*(sin(m_rotate+2.5*t/tailpoints)-sin(m_rotate)), t*.1/tailpoints, 0 );
	glEnd();
	glLineWidth( 1.0f );
	glPopMatrix();
}



//-----------------------------------------------------------------------------
// name: on_event()
// desc: ...
//-----------------------------------------------------------------------------
t_CKUINT AudicleFaceMouse::on_event( const AudicleEvent & event )
{
    static t_CKBOOL m_mouse_down = FALSE;
    static t_CKUINT which = 0;
    static t_CKUINT which2 = 0;
    static Point2D last;
    static Point2D last2;
    t_CKBOOL hit = FALSE;
    Point2D diff;
    t_CKUINT x, y;

    if( event.type == ae_event_INPUT )
    {
        InputEvent * ie = (InputEvent *)event.data;
        if( ie->type == ae_input_MOUSE )
        {
            ie->popStack();

            if( ie->state == ae_input_DOWN )
            {
                which = !!(ie->mods & ae_input_CTRL);
                which2 = !!(ie->mods & ae_input_SHIFT);
                m_mouse_down = TRUE;
                last = ie->pos;
            }

            // check
            if( !which )
            {
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
                                m_box = m_pane->matrix[x+m_pane->width*y];
                                m_box->mode = 1;
                            }
                        }
                    }
                }
            }

            if( ie->state == ae_input_UP )
            {
                which = FALSE;
                m_mouse_down = FALSE;
            }
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
                    m_selected = x;
                    if( m_box ) m_box->set_item( x );
                break;
				case 27:
                    x = 0;
                    m_selected = x;
                break;
                case 'a':
                    if( m_box ) m_box->set_dir( 0 );
                break;
                case 'w':
                    if( m_box ) m_box->set_dir( 1 );
                break;
                case 'd':
                    if( m_box ) m_box->set_dir( 2 );
                break;
                case 's':
                    if( m_box ) m_box->set_dir( 3 );
                break;
                case 'x':
                    if( m_box ) m_box->set_dir( -1 );
                break;
                case 'X':
                    m_pane->clear_dir();
                    break;
                case 'A':
                    m_pane->reset( m_selected );
                break;
                case 'C':
                    m_pane->reset( 0 );
                break;
                case 'l':
                    m_eye_y = 0.0f;
                break;
                case 'c':
                    if( m_box ) m_box->set_item( 0 );
                    break;
                case 'g':
                    go( .1f );
                break;
                case '*':
                    add_mouse( 0, 72 );
                break;
                case '-':
                    m_volume2 -= .025;
                    if( m_volume2 < 0 ) m_volume2 = 0;
                break;
                case '=':
                    m_volume2 += .025;
                    if( m_volume2 > 1 ) m_volume2 = 1;
                break;
                case '_':
                    m_volume -= .025;
                    if( m_volume < 0 ) m_volume = 0;
                break;
                case '+':
                    m_volume += .025;
                    if( m_volume > 1 ) m_volume = 1;
                break;
                case 'M':
                    remove_all();
                break;
            }
        }
        else if( ie->type == ae_input_MOTION )
        {
            if( m_mouse_down )
            {
                if( which && which2 )
                {
                    g_angle -= 10.0f * ( ie->pos - last ).length() * ( ie->pos[1] < last[1] ? 2.0 : -2.0 );
                    last = ie->pos;
                    if( g_angle < 30.0f ) g_angle = 30.0f;
                    else if( g_angle > 180.0f ) g_angle = 180.0f;
                    render_view();
                }
                else if( which )
                {
                    m_eye_y += ( ie->pos - last ).length() * ( ie->pos[1] < last[1] ? 2.0 : -2.0 );
                    last = ie->pos;
                    render_view();
                }
                else if( which2 )
                {
                    g_far += ( ie->pos - last ).length() * ( ie->pos[1] < last[1] ? 2.0 : -2.0 );
                    last = ie->pos;
                    if( g_far < .01f ) g_far = .01f;
                    render_view();
                }
            }

            if( !which )
            {
                //ie->popStack();

                // loop over pane
                for( y = 0; y < m_pane->height; y++ )
                {
                    for( x = 0; x < m_pane->width; x++ )
                    {
                        if( ie->checkID( m_pane->matrix[x+m_pane->width*y]->xid ) )
                        {
                            m_pane->clear();
                            m_box = m_pane->matrix[x+m_pane->width*y];
                            m_box->mode = 1;
                        }
                    }
                }
            }
        }
    }

    return AudicleFace::on_event( event );
}


//-----------------------------------------------------------------------------
// name: glu_init()
// desc: initialize textures (and potentially quadric)
//-----------------------------------------------------------------------------
void glu_init()
{
    // initialize quadric
    /*g_quadric = gluNewQuadric(); 
    gluQuadricNormals( g_quadric, ( GLenum )GLU_SMOOTH );
    gluQuadricTexture( g_quadric, GL_TRUE );*/

    // set texture state
    glPixelStorei( GL_UNPACK_ALIGNMENT, 4 );
    glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
    glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST );

    // uh... initialize image
    int x, y, i = 0;
	unsigned char r = 100, g = 100, b = 100, a = 255;

    // down arrow
    for( y = 0; y < HEIGHT; y++ ) 
    {
        for( x = 0; x < WIDTH; x++ ) 
        {
            if( (x >= WIDTH/8 && x < 7*WIDTH/8 && y < 7*HEIGHT/16
                && y >= -x*HEIGHT/WIDTH+9*HEIGHT/16 && y >= x*HEIGHT/WIDTH-7*HEIGHT/16) 
                || ( x >= 7*WIDTH/16 && x <= 9*WIDTH/16 && y >= 7*HEIGHT/16 && y < 15*HEIGHT/16 ) )
            {
                g_texture[i][4*(y*WIDTH + x)] = r;
				g_texture[i][4*(y*WIDTH + x) + 1] = g;
				g_texture[i][4*(y*WIDTH + x) + 2] = b;
                g_texture[i][4*(y*WIDTH + x) + 3] = a;
            }
            else 
            {
                g_texture[i][4*(y*WIDTH + x)] = 255;
				g_texture[i][4*(y*WIDTH + x) + 1] = 255;
                g_texture[i][4*(y*WIDTH + x) + 2] = 255;
                g_texture[i][4*(y*WIDTH + x) + 3] = 0;
            }   
        }
    }

    // generate OpenGL texture
    glGenTextures( 1, &g_img[0] );
    glBindTexture( GL_TEXTURE_2D, g_img[i] );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR ); 
    if( g_img[i] )
        glTexImage2D( 
            GL_TEXTURE_2D,
            0,
            GL_RGBA,
            WIDTH,
            HEIGHT,
            0,
            GL_RGBA,
            GL_UNSIGNED_BYTE,
            g_texture[i]
        );
    else
    {
        // fprintf( stderr, "g_img[%i] is %i\n", i, g_img[i]);
    }
}
