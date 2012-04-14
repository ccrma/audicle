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
// name: audicle_face_vmspace.cpp
// desc: interface for audicle face vmspace
//
// authors: Ge Wang (gewang@cs.princeton.edu)
//          Perry R. Cook (prc@cs.princeton.edu)
//          Philip Davidson (philipd@cs.princeton.edu)
//          Ananya Misra (amisra@cs.princeton.edu)
// date: Autumn 2004
//-----------------------------------------------------------------------------
#include "audicle_face_vmspace.h"
#include "audicle_gfx.h"
#include "audicle.h"

#include "digiio_rtaudio.h"
#include "util_xforms.h"




SAMPLE * g_in_buffer;
SAMPLE * g_out_buffer;
SAMPLE * g_buffer;
SAMPLE * g_window;
GLboolean g_ready = FALSE;
int g_buffer_size = 0;
int g_num_channels_out = 0;
int g_num_channels_in = 0;
SAMPLE * g_rect;
SAMPLE * g_hamm;
SAMPLE * g_hann;
t_CKUINT g_win;

// gain
GLfloat g_gain = 0.95f;
GLfloat g_time_scale = 1.0f;
GLfloat g_freq_scale = 1.0f;
GLint g_time_view = 1;
GLint g_freq_view = 2;

// flags
GLboolean g_display = TRUE;
GLboolean g_raw = FALSE;

GLboolean g_running = TRUE;

struct muh_complex { float re; float im; };
struct Pt2D { float x; float y; };
Pt2D ** g_spectrums = NULL;
unsigned int g_depth = 128;
GLboolean g_usedb = FALSE;
GLboolean g_wutrfall = TRUE;
float g_z = 0.0f;
float g_space = .2f;
GLboolean * g_draw = NULL;

t_CKUINT g_id;
t_CKUINT g_changed = FALSE;
t_CKFLOAT g_changedf = 0.0;
t_CKFLOAT sphere_down = FALSE;
t_CKFLOAT sphere_down2 = FALSE;
t_CKUINT g_id2;
t_CKFLOAT g_rate = .0250;



//-----------------------------------------------------------------------------
// name: AudicleFaceVMSpace()
// desc: ...
//-----------------------------------------------------------------------------
AudicleFaceVMSpace::AudicleFaceVMSpace( )
{
    if( !this->init( ) )
    {
        fprintf( stderr, "[audicle]: cannot start face...\n" );
        return;
    }
}




//-----------------------------------------------------------------------------
// name: ~AudicleFaceVMSpace()
// desc: ...
//-----------------------------------------------------------------------------
AudicleFaceVMSpace::~AudicleFaceVMSpace( ) { }




//-----------------------------------------------------------------------------
// name: init()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL AudicleFaceVMSpace::init( )
{
    if( !AudicleFace::init() )
        return FALSE;

    // set the buffer_size
    g_buffer_size = Digitalio::m_buffer_size;
    // set the channels
    g_num_channels_out = Digitalio::m_num_channels_out;
	g_num_channels_in = Digitalio::m_num_channels_in;
    // allocate buffers
    g_out_buffer = new SAMPLE[g_buffer_size*g_num_channels_out];
    g_in_buffer = new SAMPLE[g_buffer_size*g_num_channels_in];
    g_buffer = new SAMPLE[g_buffer_size*g_num_channels_out];
    memset( g_out_buffer, 0, g_buffer_size*sizeof(SAMPLE)*g_num_channels_out);
    memset( g_in_buffer, 0, g_buffer_size*sizeof(SAMPLE)*g_num_channels_in);
    memset( g_buffer, 0, g_buffer_size*sizeof(SAMPLE)*g_num_channels_out);
    g_window = NULL;
    // set the buffer
    Digitalio::set_extern( g_in_buffer, g_out_buffer );
    g_rect = new SAMPLE[g_buffer_size];
    g_hamm = new SAMPLE[g_buffer_size];
    g_hann = new SAMPLE[g_buffer_size];
    // blackmann
    blackman( g_rect, g_buffer_size );
    // hanning
    hanning( g_hann, g_buffer_size );
    // hamming window
    hamming( g_hamm, g_buffer_size );
    g_window = g_hamm;

    g_spectrums = new Pt2D *[g_depth];
    for( int i = 0; i < g_depth; i++ )
    {
        g_spectrums[i] = new Pt2D[g_buffer_size];
        memset( g_spectrums[i], 0, sizeof(Pt2D)*g_buffer_size );
    }
    g_draw = new GLboolean[g_depth];
    memset( g_draw, 0, sizeof(GLboolean)*g_depth );
    
    m_name = "VM-Space";
    m_bg_speed = .2;
    
    g_id = IDManager::instance()->getPickID();
    g_id2 = IDManager::instance()->getPickID();

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: destroy()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL AudicleFaceVMSpace::destroy( )
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
t_CKUINT AudicleFaceVMSpace::render( void * data )
{
    static const int LP = 4;
    static long int count = 0;
    static char str[1024];
    static unsigned wf = 0;

    float fval;
    GLint i;

    // rotate the sphere about y axis
    glRotatef( 0.0f, 0.0f, 1.0f, 0.0f );

    // color waveform
    glColor3f( 0.4f, 0.4f, 1.0f );

    // wait for data
    //while( !g_ready )
    //    usleep( 0 );

    // get the window
    memcpy( g_buffer, g_out_buffer, g_buffer_size * sizeof(SAMPLE) * g_num_channels_out );
    //g_ready = FALSE;
    
    SAMPLE * p = g_buffer + g_buffer_size;
    SAMPLE * p2 = g_buffer;
    SAMPLE * a = g_buffer;

    while( a != p)
    { 
		*a = 0.0f;
        for( i = 0; i < g_num_channels_out; i++ )
		{
			*a += *p2;
			p2 ++;
		}
		a++;
	}

    // apply the window
    GLfloat x = -1.8f, inc = 3.6f / g_buffer_size, y = .7f;
    if( g_window ) apply_window( g_buffer, g_window, g_buffer_size );

    // draw the time domain waveform
    glBegin( GL_LINE_STRIP );
    GLint ii = ( g_buffer_size - (g_buffer_size/g_time_view) ) / 2;
    for( i = ii; i < ii + g_buffer_size / g_time_view; i++ )
    {
        glVertex3f( x, g_gain * g_time_scale * 1.6 * g_buffer[i] + y, 0.0f );
        x += inc * g_time_view;
    }
    glEnd();
    
    if( g_changed )
    {
        glColor3f( 1.0 * g_changedf, .5 * g_changedf, .5 * g_changedf );
        GLfloat x = -1.8f, inc = 3.6f / g_buffer_size, y = .7f;
        // draw the time domain waveform
        glBegin( GL_LINE_STRIP );
        GLint ii = ( g_buffer_size - (g_buffer_size/g_time_view) ) / 2;
        for( i = ii; i < ii + g_buffer_size / g_time_view; i++ )
        {
            glVertex3f( x, g_gain * g_time_scale * .8 * (g_window ? g_window[i] : 1.0 ) + y, 0.0f );
            x += inc * g_time_view;
        }
        glEnd();
        
        g_changedf -= g_rate;
        if( g_changedf < 0.0 ) g_changed = FALSE;
    }

    static t_CKFLOAT r = 0.0;
    glPushMatrix();
    glPushName( g_id );
    glTranslatef( 1.7f, 0.0f, 0.0f );
    glColor3f( 1.0f, .8f, .5f );
    glRotatef( r, 0.0f, 0.0f, 1.0f );
    glutWireSphere( sphere_down ? .06 : .08, 15, 15 );
    glPopName();
    glPopMatrix();

    glPushMatrix();
    glPushName( g_id2 );
    glTranslatef( 1.45f, 0.0f, 0.0f );
    glColor3f( 1.0f, .5f, .5f );
    glRotatef( r, 0.0f, 0.0f, 1.0f );
    glutWireSphere( sphere_down2 ? .06 : .08, 15, 15 );
    glPopName();
    glPopMatrix();
    r += 1;
    
    // fft
    rfft( g_buffer, g_buffer_size/2, FFT_FORWARD );

    x = -1.8f;
    y = -1.0f;
    muh_complex * cbuf = (muh_complex *)g_buffer;

    // draw the frequency domain representation
    for( i = 0; i < g_buffer_size/2; i++ )
    {
        g_spectrums[wf][i].x = x;
        if( !g_usedb )
            g_spectrums[wf][i].y = g_gain * g_freq_scale * 
                ::pow( (double) 25 * cmp_abs( cbuf[i] ), 0.5 ) + y;
        else
            g_spectrums[wf][i].y = g_gain * g_freq_scale * 
                ( 20.0f * log10( cmp_abs(cbuf[i])/8.0 ) + 80.0f ) / 80.0f + y + .5f;
        x += inc * g_freq_view;
    }

    g_draw[wf] = true;
    
    glPushMatrix();
    glTranslatef( fp[0], fp[1], fp[2] );

    for( i = 0; i < g_depth; i++ )
    {
        if( g_draw[(wf+i)%g_depth] )
        {
            Pt2D * pt = g_spectrums[(wf+i)%g_depth];
            fval = (g_depth-i)/(float)g_depth;
            
            glColor3f( .4f * fval, 1.0f * fval, .4f * fval );

            x = -1.8f;
            glBegin( GL_LINE_STRIP );
            for( int j = 0; j < g_buffer_size/g_freq_view; j++, pt++ )
                glVertex3f( x + j*(inc*g_freq_view), pt->y, -i * g_space + g_z );
            glEnd();
        }
    }
    
    glPopMatrix();
    
    if( !g_wutrfall )
        g_draw[wf] = false;

    wf--;
    wf %= g_depth;

    return 0;
}




//-----------------------------------------------------------------------------
// name: render_pre()
// desc: ...
//-----------------------------------------------------------------------------
void AudicleFaceVMSpace::render_pre()
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
void AudicleFaceVMSpace::render_post()
{
    glPopAttrib();

    AudicleFace::render_post();
}




//-----------------------------------------------------------------------------
// name: render_view()
// desc: ...
//-----------------------------------------------------------------------------
void AudicleFaceVMSpace::render_view( )
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
t_CKUINT AudicleFaceVMSpace::on_activate()
{
    return AudicleFace::on_activate();
}




//-----------------------------------------------------------------------------
// name: on_deactivate()
// desc: ...
//-----------------------------------------------------------------------------
t_CKUINT AudicleFaceVMSpace::on_deactivate( t_CKDUR dur )
{
    return AudicleFace::on_deactivate( dur );
}




//-----------------------------------------------------------------------------
// name: on_event()
// desc: ...
//-----------------------------------------------------------------------------
t_CKUINT AudicleFaceVMSpace::on_event( const AudicleEvent & event )
{
    static t_CKUINT m_mouse_down = FALSE;
    static t_CKUINT which = 0;
    static Point2D last;
    t_CKBOOL hit = FALSE;
    Point2D diff;

    if( event.type == ae_event_INPUT )
    {
        InputEvent * ie = (InputEvent *)event.data;
        if( ie->type == ae_input_MOUSE )
        {
            ie->popStack();

            if( ie->checkID( g_id ) )
            {
                if( ie->state == ae_input_DOWN )
                {
                    hit = TRUE;
                    sphere_down = TRUE;
                }
                else
                {
                    if( !g_window ) g_window = g_hamm;
                    else if( g_window == g_hamm ) g_window = g_hann;
                    else if( g_window == g_hann ) g_window = g_rect;
                    else if( g_window == g_rect ) g_window = NULL;
                    g_changed = TRUE;
                    g_changedf = 1.0;
                    g_rate = .0250;
                }
            }
            
            if( ie->checkID( g_id2 ) )
            {
                if( ie->state == ae_input_DOWN )
                {
                    hit = TRUE;
                    sphere_down2 = TRUE;
                    g_rate = .0125;
                }
                else
                {
                    g_changed = TRUE;
                    g_changedf = 1.0;
                }
            }
            
            if( ie->state == ae_input_UP && sphere_down )
                sphere_down = FALSE;
                
            if( ie->state == ae_input_UP && sphere_down2 )
                sphere_down2 = FALSE;

            if( hit == FALSE )
            {
                if( ie->state == ae_input_DOWN )
                {
                    which = !!(ie->mods & ae_input_CTRL);
                    m_mouse_down = TRUE;
                    last = ie->pos;
                }
                else
                    m_mouse_down = FALSE;
            }
        }
        else if( ie->type == ae_input_MOTION )
        {
            if( m_mouse_down )
            {
                if( which )
                {
                    fp[0] += ( ie->pos - last ).length() * ( ie->pos[0] < last[0] ? -1.0 : 1.0 );
                    last = ie->pos;
                }
                else
                {
                    m_eye_y += ( ie->pos - last ).length() * ( ie->pos[1] < last[1] ? 2.0 : -2.0 );
                    last = ie->pos;
                    render_view();
                }
            }
        }
    }

    return AudicleFace::on_event( event );
}
