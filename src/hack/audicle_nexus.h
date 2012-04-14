//-----------------------------------------------------------------------------
// name: audicle_nexus.h
// desc: interface for audicle and chuck
//
// authors: Ge Wang (gewang@cs.princeton.edu)
//          Perry R. Cook (prc@cs.princeton.edu)
//          Philip Davidson (philipd@cs.princeton.edu)
//          Ananya Misra (amisra@cs.princeton.edu)
// date: Autumn 2004
//-----------------------------------------------------------------------------
#ifndef __AUDICLE_NEXUS_H__
#define __AUDICLE_NEXUS_H__

#include "audicle_def.h"

#include "chuck_otf.h"
#include "chuck_errmsg.h"


extern "C" {
    // this launches chuck
    t_CKBOOL chuck_chuck( int argc, char ** argv );
    // this sends message to chuck
    t_CKUINT process_msg2( Net_Msg * msg, ck_msg_func reply, void * data,
                           FILE * fd = NULL );
    // get the VM
    Chuck_VM * the();
}


#endif
