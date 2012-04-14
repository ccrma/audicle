//-----------------------------------------------------------------------------
// name: audicle_face.h
// desc: interface for audicle face
//
// authors: Ge Wang (gewang@cs.princeton.edu)
//          Perry R. Cook (prc@cs.princeton.edu)
// date: 2/16/2004
//-----------------------------------------------------------------------------
#ifndef __AUDICLE_FACE_H__
#define __AUDICLE_FACE_H__

#include "audicle_def.h"
#include "audicle_event.h"
#include "audicle_gfx.h"
#include "audicle_stats.h"
#include "audicle_font.h"

using namespace std;

// forward reference
class Audicle;



//-----------------------------------------------------------------------------
// name: class AudicleFace
// desc: interface
//-----------------------------------------------------------------------------
class AudicleFace
{
public:
    AudicleFace( );
    virtual ~AudicleFace( );
    
    virtual t_CKBOOL init();
    virtual t_CKBOOL destroy();

public: // id and state
    t_CKUINT id( ) const { return m_id; }
    t_CKUINT state( ) const { return m_state; }
    const string & name( ) const { return m_name; }
    void set_id( t_CKUINT id ) { m_id = id; }
    void set_name( const string & name ) { m_name = name; }

public: // state
    enum { INACTIVE, ACTIVATING, ACTIVE, DEACTIVATING };

public: // rendering functions
    virtual void render_pre( );
    virtual t_CKUINT render( void * data );
    virtual void render_post( );
    virtual void render_view( ) {}

public: // audicle signals
    virtual t_CKUINT on_activate( );
    virtual t_CKUINT on_deactivate( t_CKDUR duration = 0.0 );
    virtual t_CKUINT on_event( const AudicleEvent & event );

public:
    Color4D m_bg;
    Color4D m_bg2;
    t_CKFLOAT m_bg_alpha;
    t_CKFLOAT m_bg_speed;
    t_CKUINT sleep_time;

protected:
    virtual void xfade_bg( );

protected:
    t_CKUINT m_id;
    t_CKUINT m_state;
    string m_name;

protected:
    // light 0 position
    t_CKSINGLE m_light0_pos[4];

    // light 1 parameters
    t_CKSINGLE m_light1_ambient[4];
    t_CKSINGLE m_light1_diffuse[4];
    t_CKSINGLE m_light1_specular[4];
    t_CKSINGLE m_light1_pos[4];

    // modelview stuff
    t_CKSINGLE m_angle_y;
    t_CKSINGLE m_eye_y;
};


class Shred_Stat;


//-----------------------------------------------------------------------------
// name: struct Shred_Data
// desc: ...
//-----------------------------------------------------------------------------
struct Shred_Data
{

    Shred_Stat * stat;
    t_CKBOOL in_shredder_map; //XXX HAAAAACK!  
    Color4D color;
    Point3D pos;
    Point3D pos2;
    Point3D vel;
    Point3D acc;
    t_CKFLOAT radius;
    t_CKFLOAT radius2;
    t_CKUINT name;
    t_CKFLOAT x;
    
    t_CKBOOL mouse_down;
    t_CKBOOL mouse_clicked;
    
    t_CKUINT ref_count;

    Shred_Data()
    {
        stat = NULL;
        in_shredder_map = false;
        radius = .2;
        name = 0;
        ref_count = 1;
        x = 0;
    }
};




//-----------------------------------------------------------------------------
// name: struct Shred_Time
// desc: ...
//-----------------------------------------------------------------------------
struct Shred_Time
{
    vector < ShredActivation > history; // hey memory... fuck you!
    bool active;
    bool switching;
    t_CKUINT group_cycles;
    t_CKTIME switch_time;
    t_CKDUR  switch_span;
};




#endif
