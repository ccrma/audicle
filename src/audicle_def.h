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
// name: audicle_def.h
// desc: ...
//
// authors: Ge Wang (gewang@cs.princeton.edu)
//          Perry R. Cook (prc@cs.princeton.edu)
//          Ananya Misra (amisra@cs.princeton.edu)
//          Philip Davidson (philipd@cs.princeton.edu)
// date: Autumn 2004
//-----------------------------------------------------------------------------
#ifndef __AUDICLE_DEF_H__
#define __AUDICLE_DEF_H__

#include "chuck_def.h"

#include <stdio.h>
#include <math.h>
#include <assert.h>
#ifndef __PLATFORM_WIN32__
  #include <unistd.h>
  #include <netdb.h>
#endif

#include <string>
#include <vector>
#include <queue>
#include <iostream>
#include <fstream>

#ifdef __PLATFORM_WIN32__
#define DM '\\'
#define SYS_BACKSPACE 8
#endif

#ifdef __WINDOWS_PTHREAD__
#define DM '\\'
#define SYS_BACKSPACE 8
#endif

#ifdef __PLATFORM_MACOSX__
#define DM '/'
#define SYS_BACKSPACE 127
#endif

#ifdef __PLATFORM_LINUX__
#define DM '/'
#define SYS_BACKSPACE 8
#endif

#define SYS_RETURN 13

#ifndef EAT_PI
#define EAT_PI 3.14159265358979
#endif
#ifndef TWO_PI
#define TWO_PI ( 2 * EAT_PI )
#endif 

#ifndef NOMINMAX

#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif

#endif  /* NOMINMAX */


#endif
