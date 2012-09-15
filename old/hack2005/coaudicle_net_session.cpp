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
// name: coaudicle_net_session.cpp
// desc: ...
//
// authors: Philip Davidson (philipd@cs.princeton.edu)
//          Ge Wang (gewang@cs.princeton.edu)
//          Perry R. Cook (prc@cs.princeton.edu)
//          Ananya Misra (amisra@cs.princeton.edu)
// date: Autumn 2005
//-----------------------------------------------------------------------------
#include "coaudicle_net_session.h"

void swap4byteregion( t_CKUINT * ptr, t_CKUINT num ) { 
    // htonl(a) = ntohl(a) unless something is really wrong, no?
    for ( ; ptr && num != 0 ; --num, ++ptr ) *ptr = htonl(*ptr);
}

void buffer_edit_data_header::swap_me() { 
    
    //we byteswap the entire header ( contains only uints, TextLoc (2 int) )    
    t_CKUINT   num  = sizeof(buffer_edit_data_header) / sizeof(t_CKUINT);
    swap4byteregion( &data_size, num );
    
}
