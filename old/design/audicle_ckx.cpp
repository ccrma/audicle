/*----------------------------------------------------------------------------
    ChucK Concurrent, On-the-fly Audio Programming Language
      Compiler and Virtual Machine

    Copyright (c) 2004 Ge Wang and Perry R. Cook.  All rights reserved.
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
// file: audicle_ckx.cpp
// desc: ...
//
// author: Ge Wang (gewang@cs.princeton.edu)
//         Perry R. Cook (prc@cs.princeton.edu)
//         Philip L. Davidson (philipd@alumni.princeton.edu)
// date: Spring 2004
//-----------------------------------------------------------------------------
#include "audicle_ckx.h"
#include "audicle_base.h"
#include "thread.h"

Thread * audicle_thread;
THREAD_RETURN THREAD_TYPE start_audicle_process(void*);

#define AUDICLE_CKADDEXPORT(t, n) QUERY->add_export( QUERY, #t, #n, audicle_##n##_impl, TRUE )
#define AUDICLE_CKADDPARAM(t, n)  QUERY->add_param ( QUERY, #t, #n )

//-----------------------------------------------------------------------------
// name: glu_query()
// desc: query entry point
//-----------------------------------------------------------------------------
//DLL_QUERY glu_query( Chuck_DL_Query * QUERY )
CK_DLL_QUERY
{
    QUERY->set_name( QUERY, "audicle" );
    
	fprintf( stderr, "audicle query loaded...\n");
    // basic Open / Close 
    AUDICLE_CKADDEXPORT ( int, Open );
	AUDICLE_CKADDEXPORT ( int, NewWindow );
    AUDICLE_CKADDEXPORT ( int, Close );
    
    return TRUE;
}


CK_DLL_FUNC( audicle_Open_impl )
{
	fprintf(stderr, "audicle thread starter function\n");
	launch_audicle_process();
	audicle_thread = new Thread();
	if ( audicle_thread->start( start_audicle_process ) ) { 
		fprintf ( stderr, "audicle thread launch succeeded\n");
	} else { 
		fprintf ( stderr, "audicle thread launch failed \n");
	}
}

THREAD_RETURN THREAD_TYPE start_audicle_process ( void * arg )  { 
	fprintf(stderr, "audicle thread launch function\n");
	launch_audicle_process();
	fprintf(stderr, "audicle thread out ( shouldn't happen until exit ) \n");
	return 0;
}

CK_DLL_FUNC( audicle_NewWindow_impl )
{
	fprintf(stderr, "audicle create window function\n");

}

CK_DLL_FUNC( audicle_Close_impl ) { 
	fprintf(stderr, "you are trapped\n" );
}
