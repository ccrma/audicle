//-----------------------------------------------------------------------------
// name: audicle_main.cpp
// desc: entry point for audicle
//
// authors: Ge Wang (gewang@cs.princeton.edu)
//          Perry R. Cook (prc@cs.princeton.edu)
// date: 2/16/2004
//-----------------------------------------------------------------------------
#include "audicle_def.h"
#include "audicle_gfx.h"
#include "audicle_nexus.h"
#include "audicle.h"

#include "audicle_face_editor.h"
#include "audicle_face_vmspace.h"
#include "audicle_face_shredder.h"
#include "audicle_face_tnt.h"
#include "audicle_face_compiler.h"
#include "audicle_elcidua.h"




//-----------------------------------------------------------------------------
// name: main()
// desc: entry point
//-----------------------------------------------------------------------------
int main( int argc, char ** argv )
{
    
    // initialize graphics
    AudicleGfx::init();
    // initialize main window
    AudicleWindow::main()->init( 1024, 768, 0, 0, "audicle", FALSE );
    // launch chuck
    chuck_chuck( argc, argv );

    usleep(1000000);
    
    // launch (initial) audicle

    Audicle::instance()->init();
    // add faces to the audicle

    t_CKUINT editor = Audicle::instance()->add( new AudicleFaceEditor  );
    Audicle::instance()->add( new AudicleFaceVMSpace );
    Audicle::instance()->add( new AudicleFaceShredder );
    Audicle::instance()->add( new AudicleFaceTnT );
    Audicle::instance()->add( new AudicleFaceCompiler );
    t_CKUINT elcid = Audicle::instance()->add( new ElciduaFace );
    // HACK!
    // a hack ,while we add in real sessions.  HAAAAACK!
    ElciduaFace * elc = ( ElciduaFace *) Audicle::instance()->face( elcid );
    AudicleFaceEditor * ed = ( AudicleFaceEditor *) Audicle::instance()->face( editor );
    elc->set_user_wm( ed->wm );
    // end hack

    // move to the initial face
    Audicle::instance()->move_to( (t_CKUINT)0 );
    // main loop


    AudicleGfx::loop();

    return 0;
}
