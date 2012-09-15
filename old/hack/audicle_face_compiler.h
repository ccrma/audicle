//-----------------------------------------------------------------------------
// name: audicle_face_compiler.h
// desc: interface for audicle face compiler space
//
// authors: Ge Wang (gewang@cs.princeton.edu)
//          Perry R. Cook (prc@cs.princeton.edu)
// date: 2/16/2004
//-----------------------------------------------------------------------------
#ifndef __AUDICLE_FACE_COMPILER_H__
#define __AUDICLE_FACE_COMPILER_H__

#include "audicle_def.h"
#include "audicle_face.h"




//-----------------------------------------------------------------------------
// name: class AudicleFaceCompiler
// desc: ...
//-----------------------------------------------------------------------------
class AudicleFaceCompiler : public AudicleFace
{
public:
    AudicleFaceCompiler( );
    virtual ~AudicleFaceCompiler( );

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
    void set_view( int w, int h );

protected:
    t_CKBOOL m_init;
};




#endif