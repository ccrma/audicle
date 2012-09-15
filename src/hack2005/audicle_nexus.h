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
// name: audicle_nexus.h
// desc: interface for audicle and chuck
//
// authors: Ge Wang (gewang@cs.princeton.edu)
//          Perry R. Cook (prc@cs.princeton.edu)
//          Ananya Misra (amisra@cs.princeton.edu)
//          Philip Davidson (philipd@cs.princeton.edu)
// date: Autumn 2004
//-----------------------------------------------------------------------------
#ifndef __AUDICLE_NEXUS_H__
#define __AUDICLE_NEXUS_H__

#include "audicle_def.h"

#include "chuck_vm.h"
#include "chuck_otf.h"
#include "chuck_errmsg.h"
#include "chuck_compile.h"

// co-audicle buffer
// 911 404
#define CO_REQUEST_PORT 10101
#define CO_UPDATE_PORT 10110
#define CO_BUFFER_SIZE 1024
#define CO_AUDIO_PORT  9999

// this launches chuck
t_CKBOOL chuck_chuck( int argc, const char ** argv, t_CKBOOL co );
// this sends message to chuck
t_CKUINT process_msg2( Net_Msg * msg, ck_msg_func reply, void * data,
                       FILE * fd = NULL );
// stop!
void chuck_clean();

// get the VM
Chuck_VM * the();
// get the Compiler
Chuck_Compiler * the2();
// done?
t_CKBOOL & the3();


#endif
