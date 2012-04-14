//-----------------------------------------------------------------------------
// name: audicle_face_shredder.h
// desc: interface for audicle face shredder
//
// authors: Ge Wang (gewang@cs.princeton.edu)
//          Perry R. Cook (prc@cs.princeton.edu)
// date: 2/16/2004
//-----------------------------------------------------------------------------
#ifndef __AUDICLE_FACE_SHREDDER_H__
#define __AUDICLE_FACE_SHREDDER_H__

#include "audicle_def.h"
#include "audicle_face.h"

#include <map>
using namespace std;




//-----------------------------------------------------------------------------
// name: class AudicleFaceShredder
// desc: ...
//-----------------------------------------------------------------------------
class AudicleFaceShredder : public AudicleFace
{
public:
    AudicleFaceShredder( );
    virtual ~AudicleFaceShredder( );

public:
    virtual t_CKBOOL init();
    virtual t_CKBOOL destroy();

public:
    virtual void render_pre( );
    virtual void render_post( );
    virtual t_CKUINT render( void * data );
    virtual void render_view( );
    virtual t_CKUINT on_activate( );
    virtual t_CKUINT on_deactivate( t_CKDUR duration = 0.0 );
    virtual t_CKUINT on_event( const AudicleEvent & event );

public:
    void render_1( t_CKFLOAT delta );
    void render_2( t_CKFLOAT delta );
    void render_3( t_CKFLOAT delta );
    static void initialize_stat_data ( Shred_Stat * stat );
    t_CKUINT which;
    t_CKUINT m_b1_id;
    t_CKBOOL sphere_down;
    
    t_CKUINT on_event_1( const AudicleEvent & event );
    t_CKUINT on_event_2( const AudicleEvent & event );
    t_CKUINT on_event_3( const AudicleEvent & event );

public:
    void set_view( int w, int h );

protected:
    t_CKBOOL m_init;
    t_CKFLOAT m_last_time;
    t_CKFLOAT m_time;

protected:
    t_CKUINT m_test_id;
    Point2D m_test_pt;
    
protected:
    vector<Shred_Stat *> m_stats;
    vector<Shred_Stat *> m_happy;
    map<Shred_Stat *, Shred_Stat *> m_done;
    map<t_CKUINT, Shred_Data *> m_map;
};


#endif
