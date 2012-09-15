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
// name: audicle_face_mouse.h
// desc: interface for audicle face mouse
//
// authors: Ge Wang (gewang@cs.princeton.edu)
//          Perry R. Cook (prc@cs.princeton.edu)
//          Ananya Misra (amisra@cs.princeton.edu)
//          Philip Davidson (philipd@cs.princeton.edu)
// date: 2/16/2004
//-----------------------------------------------------------------------------
#ifndef __AUDICLE_FACE_MOUSE_H__
#define __AUDICLE_FACE_MOUSE_H__

#include "audicle_def.h"
#include "audicle_face.h"
#include "audicle_gfx.h"


struct MouseBox;


// items
struct MouseItem
{
    MouseItem()
    { m_rotate = 0.0f; }

    virtual ~MouseItem() { }
    virtual void draw() = 0;

public:
    GLfloat m_rotate;
};


// cube
struct MouseItemCube : public MouseItem
{ virtual void draw(); };

// cone
struct MouseItemCone : public MouseItem
{ virtual void draw(); };

// cylinder
struct MouseItemCylinder : public MouseItem
{ virtual void draw(); };

// torus
struct MouseItemTorus : public MouseItem
{ virtual void draw(); };

// sphere
struct MouseItemSphere : public MouseItem
{ virtual void draw(); };

// portal in
struct MouseItemPortalIn : public MouseItem
{ virtual void draw(); };

// portal out
struct MouseItemPortalOut : public MouseItem
{
    MouseItemPortalOut()
    {
        owner = NULL;
        r1 = 0.0f;
        r2 = 0.0f;
    }
    virtual void draw();
    MouseBox * owner;
    GLfloat r1;
    GLfloat r2;
};


// mouse
struct Mouse : public MouseItem
{
    Mouse()
    {
        curr = NULL;
        dest = NULL;
        x = 0.0f;
        y = 0.0f;
        z = 0.0f;
        dir = -1;
        x_start = 0.0f;
        y_start = 0.0f;
        x_end = 0.0f;
        y_end = 0.0f;
        note = 72;
        t_done = 0;
    }

    virtual void draw();

    GLfloat x;
    GLfloat y;
    GLfloat z;
    MouseBox * curr;
    t_CKINT note;

    GLfloat x_start;
    GLfloat y_start;
    GLfloat x_end;
    GLfloat y_end;

    t_CKINT dir;
    MouseBox * dest;
    t_CKFLOAT t_done;
};



//-----------------------------------------------------------------------------
// name: struct MouseBox
// desc: ...
//-----------------------------------------------------------------------------
struct MouseBox
{
    // opengl id
    t_CKUINT xid;
    // holds
    t_CKINT value;
    // direction (-1 none, 0 left, 1 up, 2 right, 3 down)
    t_CKINT dir;
    // item (-1 none)
    t_CKINT item;
    // portal
    MouseItem * portal;
    // mode ( 0 normal, 1 selected )
    t_CKUINT mode;
    // glow state
    t_CKFLOAT glow;
    // things
    MouseItem ** things;
    // number
    t_CKUINT num_things;
    // mouse
    Mouse * mouse;
    
    t_CKINT x;
    t_CKINT y;
    t_CKINT auto_dir;

    // constructor
    MouseBox( t_CKUINT _id = 0, t_CKUINT _value = 0,
             t_CKUINT _mode = 0, t_CKFLOAT _glow = 0.0 )
    {
        xid = _id;
        value = _value;
        mode = _mode;
        glow = _glow;
        dir = -1;
        item = 0;
        portal = NULL;
        num_things = 6;
        things = new MouseItem *[num_things];
        things[0] = NULL;
        things[1] = new MouseItemSphere;
        things[2] = new MouseItemCube;
        things[3] = new MouseItemCone;
        things[4] = new MouseItemCylinder;
        things[5] = new MouseItemTorus;

        x = 0;
        y = 0;
        auto_dir = -1;
        mouse = NULL;
    }

    // item
    void set_item( t_CKINT item )
    {
        if( item >= num_things ) return;
        else if( item <= 0 ) this->item = 0;
        else if( portal ) return;
        else
        {
            this->item = item;
        }
    }

    // item
    void set_dir( t_CKINT direction )
    {
        if( direction < 0 ) dir = -1;
        else dir = direction % 4;
    }
};




//-----------------------------------------------------------------------------
// name: struct MousePane
// desc: ...
//-----------------------------------------------------------------------------
struct MousePane
{
    // width
    t_CKUINT width;
    // height
    t_CKUINT height;
    // uh
    t_CKUINT which;
    // matrix of boxes
    MouseBox ** matrix;
    // for chuck
    t_CKINT SELF;

    // constructor
    MousePane() { width = 0; height = 0; which = 0; matrix = NULL; SELF = 0; }
    // destructor
    ~MousePane() { cleanup(); }
    // initialize
    t_CKBOOL init( t_CKUINT w, t_CKUINT h );
    // cleanup
    void cleanup( );
    // clear
    void clear( ) { if( matrix ) {
        for( t_CKUINT i = 0; i < width*height; i++ )
            matrix[i]->mode = 0;
        }
    }
    // set
    void reset( t_CKINT x ) { if( matrix ) {
        for( t_CKUINT i = 0; i < width*height; i++ )
            matrix[i]->set_item( x );
        }
    }
    // evict
    void evict()
    {
        for( t_CKUINT i = 0; i < width*height; i++ )
            matrix[i]->mouse = NULL;
    }
    // dir
    void clear_dir( ) { if( matrix ) {
        for( t_CKUINT i = 0; i < width*height; i++ )
            matrix[i]->dir = -1;
        }
    }
};




//-----------------------------------------------------------------------------
// name: class AudicleFaceMouse
// desc: ...
//-----------------------------------------------------------------------------
class AudicleFaceMouse : public AudicleFace
{
public:
    AudicleFaceMouse( );
    virtual ~AudicleFaceMouse( );

public:
    virtual t_CKBOOL init();
    virtual t_CKBOOL destroy();

public:
    virtual void render_pre( );
    virtual void render_post( );
    void render_box( MouseBox * box );
    void render_pane( );
    void render_panel( );
    void render_mice( );
    virtual t_CKUINT render( void * data );
    virtual void render_view( );
    virtual t_CKUINT on_activate( );
    virtual t_CKUINT on_deactivate( t_CKDUR duration = 0.0 );
    virtual t_CKUINT on_event( const AudicleEvent & event );

public:
    void set_view( int w, int h );
    void add_mouse( int from, int note );
    t_CKINT move( Mouse * mouse );
    void go( t_CKFLOAT duration );

public:
    t_CKINT get_item( t_CKINT index )
    { t_CKINT val = index < m_num_items ? m_item_cache[index] : 0; if( index < m_num_items ) m_item_cache[index] = 0; return val; } 
    t_CKINT get_teleport_in()
    { t_CKINT val = m_teleport_in ? m_teleport_in->note : 0; SAFE_DELETE(m_teleport_in); return val; }
    t_CKINT get_teleport_out()
    { t_CKINT val = m_teleport_out ? m_teleport_out->note : 0; SAFE_DELETE(m_teleport_out); return val; }
    t_CKINT get_num_mice()
    { return m_mice.size(); }
    t_CKFLOAT get_volume()
    { return m_volume; }
    t_CKFLOAT get_volume2()
    { return m_volume2; }
    void remove_all();

public:
    t_CKBOOL m_init;
    t_CKBOOL m_grid;

    // selected number
    t_CKUINT m_selected;
    // pane
    MousePane * m_pane;
    // panel
    MouseBox * m_panel;
    // box
    MouseBox * m_box;
    // mice
    std::vector<Mouse *> m_mice;
    std::vector<Mouse *> m_keep;
    std::vector<Mouse *> m_bye;
    std::vector<Mouse *> m_floating;
    t_CKINT * m_item_cache;
    t_CKUINT m_num_items;
    t_CKFLOAT m_volume;
    t_CKFLOAT m_volume2;
    Mouse * m_teleport_in;
    Mouse * m_teleport_out;
    // portal
    MouseItemPortalOut m_portal_in;
    MouseItemPortalOut m_portal_out;
    XMutex m_mutex;

    Point3D fp;
    t_CKTIME t_start;
    t_CKTIME t_end;
};




#endif
