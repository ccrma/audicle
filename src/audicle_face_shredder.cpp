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
// name: audicle_face_shredder.cpp
// desc: interface for audicle face shredder
//
// authors: Ge Wang (gewang@cs.princeton.edu)
//          Perry R. Cook (prc@cs.princeton.edu)
//          Philip Davidson (philipd@cs.princeton.edu)
//          Ananya Misra (amisra@cs.princeton.edu)
// date: Autumn 2004
//-----------------------------------------------------------------------------
#include "audicle_face_shredder.h"
#include "audicle_gfx.h"
#include "audicle_utils.h"
#include "audicle.h"
#include "chuck_stats.h"

#include "digiio_rtaudio.h"




//-----------------------------------------------------------------------------
// name: AudicleFaceShredder()
// desc: ...
//-----------------------------------------------------------------------------
AudicleFaceShredder::AudicleFaceShredder( )
{
    if( !this->init( ) )
    {
        fprintf( stderr, "[audicle]: cannot start face...\n" );
        return;
    }
}




//-----------------------------------------------------------------------------
// name: ~AudicleFaceShredder()
// desc: ...
//-----------------------------------------------------------------------------
AudicleFaceShredder::~AudicleFaceShredder( ) { }




//-----------------------------------------------------------------------------
// name: init()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL AudicleFaceShredder::init( )
{
    EM_log( CK_LOG_INFO, "AudicleFaceShredder: initializing..." );

    if( !AudicleFace::init() )
        return FALSE;
        
    m_name = "Shredder";
    m_bg[0] = 1.0; m_bg[1] = 1.0; m_bg[2] = 1.0; m_bg[3] = 1.0;
    
    // get id
    m_test_id = IDManager::instance()->getPickID();
    m_last_time = -1.0;
    
    which = 1;
    m_b1_id = IDManager::instance()->getPickID();
    sphere_down = FALSE;
    
    return TRUE;
}




//-----------------------------------------------------------------------------
// name: destroy()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL AudicleFaceShredder::destroy( )
{
    EM_log( CK_LOG_INFO, "AudicleFaceShredder: cleaning up..." );

    this->on_deactivate( 0.0 );
    m_id = Audicle::NO_FACE;
    m_state = INACTIVE;

    return TRUE;
}


t_CKFLOAT frand( )
{ return rand() / (t_CKFLOAT)RAND_MAX * 2.0 - 1.0; }

//-----------------------------------------------------------------------------
// name: render()
// desc: ...
//-----------------------------------------------------------------------------
t_CKUINT AudicleFaceShredder::render( void * d )
{
    static t_CKFLOAT x = 0.0;

    glPushMatrix();
    glPushName( m_b1_id );
    glTranslatef( 1.5f, -1.1f, .1f );
    glColor3f( .25f, .75f, .25f );
    glRotatef( x, 0.0f, 0.0f, 1.0f );
    x += .1f ;
    glutWireSphere( sphere_down ? .06 : .075, 15, 15 );
    glPopName();
    glPopMatrix();

    glRotatef( m_eye_y * 100.0f, 1.0f, 0.0f, 0.0f );

    // get shreds from ChucK VM
    Chuck_Stats::instance()->get_shreds( m_stats, m_done );
    
    Shred_Stat * stat = NULL;
    Shred_Data * data = NULL;
    Color4D c;
    Point3D p, v, a;
    Point3D b;
    t_CKFLOAT theta;

    m_time = AudicleWindow::main()->get_current_time();
    if( m_last_time < 0.0 ) m_last_time = m_time;
    t_CKFLOAT delta = m_time - m_last_time;
    m_last_time = m_time;
    int i;

    // loop through
    for( i = 0; i < m_stats.size(); i++ )
    {
        stat = m_stats[i];
        data = (Shred_Data *)stat->data;
        // initialized?

        if( stat->data == NULL )
        {
            initialize_stat_data ( stat );
        }

        if( stat->data->in_shredder_map == false ) 
        { 
            //this will catch data that have been inited
            //outside the shredder ( like in TNT! ) 
            m_map[stat->data->name] = stat->data;
            stat->data->in_shredder_map = true;
        }        
    }
    
    // loop through
    for( i = 0; i < m_stats.size(); i++ )
    {
        stat = m_stats[i];
        data = (Shred_Data *)stat->data;
        p = data->pos;
        // v = data->vel;
        // a = data->acc;
        // advance
        // v += a * delta;
        theta = stat->average_cycles / 100000.0 + v[0];
        b[0] = cos(theta) * p[0] + sin(theta) * p[1];
        b[1] = -sin(theta) * p[0] + cos(theta) * p[1];
        b[2] = p[2];
        
        data->pos = b;
        // data->vel = v;
    }

    // loop through
    for( i = 0; i < m_stats.size(); i++ )
    {
        data = (Shred_Data *)m_stats[i]->data;
        if( data->radius < .15 )
            data->radius += .05 * delta;

        for( int j = 0; j < m_stats.size(); j++ )
        {
            Shred_Data * other = (Shred_Data *)m_stats[j]->data;
            if( data == other ) continue;

            t_CKFLOAT norm = ( data->pos - other->pos ).length();
            if( norm < ( data->radius + other->radius + .02 ) )
            {
                Shred_Data * smaller = other->radius < data->radius ? other : data;
                Shred_Data * bigger = smaller == other ? data : other;

                t_CKFLOAT ratio = norm / ( data->radius + other->radius + .02 );
                data->radius *= ratio;
                other->radius *= ratio;
                
                if( smaller->radius < .07 ) smaller->radius = .07;
                if( bigger->radius < .07 ) bigger->radius = .07;
            }
        }
    }

    // loop through
    m_happy.clear();
    map<Shred_Stat *, Shred_Stat *>::iterator iter;
    for( iter = m_done.begin(); iter != m_done.end(); iter++ )
    {
        stat = (*iter).second;
        data = (Shred_Data *)stat->data;
        // data->color.scale( .9 );
        
        if( !data || data->pos[2] > 20.0 )
        {
            if( data ) data->ref_count--;
            m_happy.push_back( stat );
            continue;
        }

        data->x += .0015;
        data->pos[2] += data->x * delta;
        // data->radius -= .1 * delta;
    }
    
    // loop through
    for( i = 0; i < m_happy.size(); i++ )
    {
        m_done.erase( m_done.find( m_happy[i] ) );
    }

    if( which == 1 ) render_1( delta );
    else if( which == 2 ) render_2( delta );
//    else if( which == 3 ) render_3();

    return 0;
}



void
AudicleFaceShredder::initialize_stat_data( Shred_Stat * stat ) { 

    Shred_Data * data = new Shred_Data;
    data->stat = stat;
    data->pos[0] = frand();
    data->pos[1] = frand();
    data->pos[2] = 0.0;
    data->pos2[0] = -1.7;
    data->vel[0] = frand()/3000.0;
    data->vel[1] = frand()/1000.0;
    data->color[0] = rand()/(t_CKFLOAT)RAND_MAX;
    data->color[1] = rand()/(t_CKFLOAT)RAND_MAX;
    data->color[2] = rand()/(t_CKFLOAT)RAND_MAX;
    data->color[3] = .5;
    data->radius = .001;
    data->radius2 = .04;
    data->name = IDManager::instance()->getPickID();
	
    data->in_shredder_map = false;
    stat->data = data;
}



//-----------------------------------------------------------------------------
// name: render_1()
// desc: ...
//-----------------------------------------------------------------------------
void AudicleFaceShredder::render_1( t_CKFLOAT delta )
{
    Shred_Stat * stat = NULL;
    Shred_Data * data = NULL;
    Color4D c;
    Point3D p;
    int i;

    // loop through
    for( i = 0; i < m_stats.size(); i++ )
    {    
        data = (Shred_Data *)m_stats[i]->data;
        c = data->color;
        p = data->pos;
        
        // push the name
        glPushName( data->name );
        glPushMatrix();
        glColor4f( c[0], c[1], c[2], c[3] );
        glTranslatef( p[0], p[1], p[2] );
        glutSolidSphere( data->radius, 15, 15 );
        glPopMatrix();
        glPopName();
    }

    // loop through
    t_CKINT y;
    map<Shred_Stat *, Shred_Stat *>::iterator iter;
    for( iter = m_done.begin(); iter != m_done.end(); iter++ )
    {
        data = (Shred_Data *)(*iter).second->data;
        c = data->color;
        p = data->pos;

        // push the name
        //glPushName( data->name );
        glPushMatrix();
        glColor4f( c[0], c[1], c[2], c[3] );
        glTranslatef( p[0], p[1], p[2] );
        y = (int)(5.0 + 10.0 * (20-p[2]) / 20.0 );
        glutSolidSphere( data->radius, y, y);
        glPopMatrix();
        //glPopName();    
    }
}




//-----------------------------------------------------------------------------
// name: render_2()
// desc: ...
//-----------------------------------------------------------------------------
void AudicleFaceShredder::render_2( t_CKFLOAT delta )
{    
    Shred_Stat * stat = NULL;
    Shred_Data * data = NULL;
    Color4D c;
    Point3D p, v, a;
    Point3D b;
    // t_CKFLOAT theta;
    static char buffer[2048];
    const char * t;
    // char * d;

    int i;
    
    // loop through
    t_CKFLOAT y = 1.15;
    for( i = 0; i < m_stats.size(); i++ )
    {
        stat = m_stats[i];
        data = (Shred_Data *)stat->data;
        data->pos2[1] = y;
        y -= .15;
    }

    // loop through
    Shred_Stat * s;
    t_CKFLOAT srate = Digitalio::sampling_rate() / 1000.0;
    for( i = 0; i < m_stats.size(); i++ )
    {    
        s = m_stats[i];
        data = m_stats[i]->data;
        c = data->color;
        p = data->pos2;

        switch( s->state )
        {
            case 0: t = "inactive"; break;
            case 1: t = "active"; break;
            case 2: t = "waiting"; break;
            case 3: t = "done"; break;
            default: t = "none"; break;
        }
        sprintf( buffer, "%ld - %s - %s - %s - cycle(%ld) - acts(%ld) - c/a(%.2f) - avg(%.2fms)",
                 s->xid, t, s->name.c_str(), s->source.c_str(), s->cycles, s->activations, s->average_cycles, s->average_ctrl / srate );

        // push the name
        glPushName( data->name );
        glPushMatrix();
        glColor4f( c[0], c[1], c[2], c[3] );
        glTranslatef( p[0], p[1], p[2] );
        glutSolidSphere( data->radius2, 15, 15 );
        glTranslatef( .1f, -.033f, 0.0f );
        glPushMatrix();
        glColor4f( 0.0, 0.0, 0.0, 1.0 );
        scaleFont( .052 );
        drawString( buffer );
        glPopMatrix();
        glPopMatrix();
        glPopName();
    }

    // loop through
    map<Shred_Stat *, Shred_Stat *>::iterator iter;
    for( iter = m_done.begin(); iter != m_done.end(); iter++ )
    {
        s = (*iter).second;
        data = (Shred_Data *)(*iter).second->data;
        c = data->color;
        data->pos2[1] = y;
        y -= .15;
        p = data->pos2;

        switch( s->state )
        {
            case 0: t = "inactive"; break;
            case 1: t = "active"; break;
            case 2: t = "waiting"; break;
            case 3: t = "done"; break;
            default: t = "none"; break;
        }
        sprintf( buffer, "%ld - %s - %s - %s - cycle(%ld) - acts(%ld) - c/a(%.2f) - avg(%.2fms)",
                 s->xid, t, s->name.c_str(), s->source.c_str(), s->cycles, s->activations, s->average_cycles, s->average_ctrl / srate );

        // push the name
        glPushName( data->name );
        glPushMatrix();
        glColor4f( c[0], c[1], c[2], c[3] );
        glTranslatef( p[0], p[1], p[2] );
        glutSolidSphere( data->radius2, 15, 15 );
        glTranslatef( .1f, -.033f, 0.0f );
        glPushMatrix();
        glDisable( GL_LIGHTING );
        glColor4f( 0.4f, 0.4f, 0.4f, 1.0f );
        scaleFont( .052 );
        drawString( buffer );
        glEnable( GL_LIGHTING );
        glPopMatrix();
        glPopMatrix();
        glPopName();
    }
}




//-----------------------------------------------------------------------------
// name: render_pre()
// desc: ...
//-----------------------------------------------------------------------------
void AudicleFaceShredder::render_pre()
{
    glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );

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
    
    glEnable( GL_BLEND );
}




//-----------------------------------------------------------------------------
// name: render_post()
// desc: ...
//-----------------------------------------------------------------------------
void AudicleFaceShredder::render_post()
{
    glPopAttrib();

    AudicleFace::render_post();
}




//-----------------------------------------------------------------------------
// name: render_view()
// desc: ...
//-----------------------------------------------------------------------------
void AudicleFaceShredder::render_view( )
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
t_CKUINT AudicleFaceShredder::on_activate()
{
    m_last_time = -1.0;
    return AudicleFace::on_activate();
}




//-----------------------------------------------------------------------------
// name: on_deactivate()
// desc: ...
//-----------------------------------------------------------------------------
t_CKUINT AudicleFaceShredder::on_deactivate( t_CKDUR dur )
{
    return AudicleFace::on_deactivate( dur );
}




//-----------------------------------------------------------------------------
// name: on_event()
// desc: ...
//-----------------------------------------------------------------------------
t_CKUINT AudicleFaceShredder::on_event( const AudicleEvent & event )
{
    int handled = 0;

    if( event.type == ae_event_INPUT )
    {
        InputEvent * ie = (InputEvent *)event.data;
        if( ie->type == ae_input_MOUSE )
        {
            ie->popStack();
            
            if( ie->checkID( m_b1_id ) )
            {
                if( ie->state == ae_input_DOWN )
                {
                    sphere_down = TRUE;
                    handled = TRUE;
                }
                else
                {
                    if( which == 1 ) which = 2;
                    else if( which ==2 ) which = 1;
                }
            }
            
            if( ie->state == ae_input_UP && sphere_down )
                sphere_down = FALSE;
        }
    }

    if( !handled )
    {
        if( which == 1 ) return on_event_1( event );
        if( which == 2 ) return on_event_2( event );
    }
    
    return AudicleFace::on_event( event );
}




//-----------------------------------------------------------------------------
// name: on_event_1()
// desc: ...
//-----------------------------------------------------------------------------
t_CKUINT AudicleFaceShredder::on_event_1( const AudicleEvent & event )
{
    static t_CKUINT m_mouse_down = FALSE;
    static Point2D last;
    map<t_CKUINT, Shred_Data *>::iterator iter;
    t_CKBOOL hit = FALSE;
    Point2D diff;

    if( event.type == ae_event_INPUT )
    {
        InputEvent * ie = (InputEvent *)event.data;
        if( ie->type == ae_input_MOUSE )
        {
            ie->popStack();
            
            for( iter = m_map.begin(); iter != m_map.end(); iter++ )
            {
                if( ie->checkID( (*iter).second->name ) )
                {
                    hit = TRUE;
                    if( ie->state == ae_input_DOWN )
                        (*iter).second->mouse_down = 1;
                    else
                    {
                        if( (*iter).second->mouse_down == 1 )
                            (*iter).second->mouse_clicked = 1;
                        (*iter).second->mouse_down = 0;
                        if( m_mouse_down )
                            m_mouse_down = FALSE;
                        fprintf( stderr, "click: %s\n", (*iter).second->stat->name.c_str() );
                    }
                }
            }

            if( hit == FALSE )
            {
                if( ie->state == ae_input_DOWN )
                {
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
                m_eye_y += ( ie->pos - last ).length() * ( ie->pos[1] < last[1] ? 2.0 : -2.0 );
                last = ie->pos;
                //render_view();
            }
        }
    }

    return AudicleFace::on_event( event );
}




//-----------------------------------------------------------------------------
// name: on_event_2()
// desc: ...
//-----------------------------------------------------------------------------
t_CKUINT AudicleFaceShredder::on_event_2( const AudicleEvent & event )
{
    static t_CKUINT m_mouse_down = FALSE;
    static Point2D last;
    map<t_CKUINT, Shred_Data *>::iterator iter;
    t_CKBOOL hit = FALSE;
    Point2D diff;

    if( event.type == ae_event_INPUT )
    {
        InputEvent * ie = (InputEvent *)event.data;
        if( ie->type == ae_input_MOUSE )
        {
            ie->popStack();
            
            for( iter = m_map.begin(); iter != m_map.end(); iter++ )
            {
                if( ie->checkID( (*iter).second->name ) )
                {
                    if( ie->state == ae_input_DOWN )
                        (*iter).second->mouse_down = 1;
                    else
                    {
                        if( (*iter).second->mouse_down == 1 )
                            (*iter).second->mouse_clicked = 1;
                        (*iter).second->mouse_down = 0;
                        if( m_mouse_down )
                            m_mouse_down = FALSE;
                        fprintf( stderr, "click: %s\n", (*iter).second->stat->name.c_str() );
                    }
                }
            }
            
            if( hit == FALSE )
            {
                if( ie->state == ae_input_DOWN )
                {
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
                m_eye_y += ( ie->pos - last ).length() * ( ie->pos[1] < last[1] ? 2.0 : -2.0 );
                last = ie->pos;
                render_view();
            }
        }
    }

    return AudicleFace::on_event( event );
}
