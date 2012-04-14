//-----------------------------------------------------------------------------
// name: audicle_face_editor.h
// desc: interface for audicle text editor face
//
// authors: Philip Davidson (philipd@cs.princeton.edu)
//          Ge Wang (gewang@cs.princeton.edu)
//          Perry R. Cook (prc@cs.princeton.edu)
// date: Autumn 2004
//-----------------------------------------------------------------------------
#ifndef __AUDICLE_FACE_EDITOR_H__
#define __AUDICLE_FACE_EDITOR_H__

#include "audicle_face.h"
#include "audicle_ui_coding.h"


//-----------------------------------------------------------------------------
// name: class AudicleFaceVMSpace
// desc: ...
//-----------------------------------------------------------------------------
class AudicleFaceEditor: public AudicleFace
{

public:
    AudicleFaceEditor ( );
    virtual ~AudicleFaceEditor( );

public:
    WindowManager * wm;
    CodeWindowHandler * wh;

public:
    virtual t_CKBOOL init();
    virtual t_CKBOOL destroy();

public:
    virtual void render_pre( );
    virtual t_CKUINT render( void * data );
    virtual void render_post( );
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

