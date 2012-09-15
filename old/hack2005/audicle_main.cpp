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
// name: audicle_main.cpp
// desc: entry point for audicle
//
// authors: Ge Wang (gewang@cs.princeton.edu)
//          Perry R. Cook (prc@cs.princeton.edu)
//          Ananya Misra (amisra@cs.princeton.edu)
//          Philip Davidson (philipd@cs.princeton.edu)
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
#include "audicle_face_groove.h"
// #include "audicle_face_groove2.h"
#include "audicle_face_mouse.h"
#include "audicle_face_skot.h"
#include "audicle_elcidua.h"

#include "coaudicle.h"
#include "coaudicle_protocol.h"
#include "audicle_net.h"
#include "chuck_errmsg.h"
#include "chuck_globals.h"
#include "audicle_session_manager.h"

#include "audicle_import.h"

#include <map>
#include <string>
#include <vector>
using namespace std;

#define AUDICLE_VERSION "1.0.0.7 (quintesson)"

void audicle_usage();
void version();



//-----------------------------------------------------------------------------
// name: start_local_coaudicle_session()
// desc: local coaudicle
//-----------------------------------------------------------------------------
t_CKBOOL start_local_coaudicle_session()
{     
    // connect to remote server
    const char * host = "localhost";
    int request_port = CO_REQUEST_PORT;
    int update_port = CO_UPDATE_PORT;
	
    //elc->set_session( ClientSessionManagerImp::instance()->local_session() );

    CoSession * local = ClientSessionManagerImp::instance()->new_session( host, request_port, update_port );
    ClientSessionManagerImp::instance()->set_cur_session( local );

    /*
    ClientSessionManagerImp::instance()->add_session( local );
    ClientSessionManagerImp::instance()->set_cur_session( local );
    local->start( (char*)host , request_port, update_port );
    */
    
    // elc->set_session (
    // return simulate_coaudicle();

    return true;    
}




// global
ElDemoFace * g_el_demo_face = NULL;
AudicleFaceGroove * g_face_groove = NULL;
// AudicleFaceGroove2 * g_face_groove2 = NULL;
AudicleFaceFloor * g_face_floor = NULL;
AudicleFaceMouse * g_face_mouse = NULL;
AudicleFaceEditor * g_face_editor = NULL;
AudicleFaceVMSpace * g_face_vmspace = NULL;
AudicleFaceShredder * g_face_shredder = NULL;
AudicleFaceTnT * g_face_tnt = NULL;

// face hash
map<string, AudicleFace *> g_face_lookup;
map<string, string> g_face_alias;
vector<string> g_face_set;




//-----------------------------------------------------------------------------
// name: init_faces()
// desc: ...
//-----------------------------------------------------------------------------
void init_faces()
{
    // instantiate
    g_face_editor = new AudicleFaceEditor;
    g_face_vmspace = new AudicleFaceVMSpace;
    g_face_shredder = new AudicleFaceShredder;
    g_face_tnt = new AudicleFaceTnT;
    g_face_groove = new AudicleFaceGroove;
    g_el_demo_face = new ElDemoFace;
    g_face_floor = new AudicleFaceFloor;
    g_face_mouse = new AudicleFaceMouse;
    
    // map
    g_face_lookup["shreditor"] = g_face_editor; g_face_alias["editor"] = "shreditor";
    g_face_lookup["vmspace"] = g_face_vmspace;
    g_face_lookup["shredder"] = g_face_shredder;
    g_face_lookup["tnt"] = g_face_tnt;
    g_face_lookup["non-specific"] = g_face_groove; g_face_alias["groove"] = "non-specific";
    g_face_lookup["elcidua"] = g_el_demo_face; g_face_alias["co-audicle"] = "elcidua";
    g_face_lookup["slotter"] = g_face_floor; g_face_alias["floor"] = "slotter";
    g_face_lookup["ccr"] = g_face_mouse;
    
    // default face set
    g_face_set.resize( 6 );
    g_face_set[0] = "shreditor";
    g_face_set[1] = "vmspace";
    g_face_set[2] = "shredder";
    g_face_set[3] = "tnt";
    g_face_set[4] = "non-specific";
    g_face_set[5] = "elcidua";
}


//-----------------------------------------------------------------------------
// name: find_face()
// desc: ...
//-----------------------------------------------------------------------------
AudicleFace * find_face( const string & name )
{
    // lookup
    if( g_face_lookup.find( name ) != g_face_lookup.end() )
        return g_face_lookup[name];
    
    // alias
    if( g_face_alias.find( name ) != g_face_alias.end() )
        return find_face( g_face_alias[name] );
        
    // not found
    return NULL;
}


//-----------------------------------------------------------------------------
// name: parse_face()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL parse_face( const char * arg )
{
    // assume it starts with "--face"
    
    // make sure next one is :
    if( arg[0] != ':' )
    {
        fprintf( stderr, "[audicle]: incorrect format for --face: missing ':'...\n" );
        return FALSE;
    }
    
    // advance;
    arg++;
    char buffer[32];
    char * c = buffer;
    
    // get number
    while( *arg && *arg != ':' )
        *c++ = *arg++;
    
    // check
    if( !*arg || !*(arg+1) )
    {
        fprintf( stderr, "[audicle]: incorrect format for --face: missing arguments...\n" );
        return FALSE;
    }
    
    // null terminate
    *c = '\0';

    // convert
    int n = atoi( buffer );
    
    // advance
    arg++;
    
    // check
    if( !n )
    {
        fprintf( stderr, "[audicle]: incorrect # field in --face...\n" );
        return FALSE;
    }
    else if( n > 6 )
    {
        fprintf( stderr, "[audicle]: face # must be in [1-6]...\n" );
        return FALSE;
    }

    // get name
    if( !find_face( arg ) )
    {
        fprintf( stderr, "[audicle]: unknown face name: '%s'...\n", arg );
        return FALSE;
    }
    
    // formal name
    string name = arg;
    if( g_face_alias.find( name ) != g_face_alias.end() )
        name = g_face_alias[name];

    // look for it
    for( int i = 0; i < 6; i++ )
    {
        if( g_face_set[i] == name )
        {
            g_face_set[i] = g_face_set[n-1];
            break;
        }
    }

    // copy
    g_face_set[n-1] = name;
    
    return TRUE;    
}




//-----------------------------------------------------------------------------
// name: init_api()
// desc: ... import audicle API
//-----------------------------------------------------------------------------
t_CKBOOL init_api( Chuck_Env * env )
{
    // launch (initial) audicle
    Audicle::instance()->init();
    // initialize faces
    init_faces();

    // make context
    Chuck_Context * context = type_engine_make_context( NULL, "@[internal-audicle]" );
    // reset env
    env->reset();
    // load it
    type_engine_load_context( env, context );

    // log
    EM_log( CK_LOG_SYSTEM, "(audicle) adding audicle classes..." );
    EM_pushlog();
    init_class_dude( env );
    init_class_world( env );
    init_class_pane( env );
    init_class_groove( env );
    init_class_floor( env );
    init_class_mouse_pane( env );
    init_class_mouse_mania( env );
    EM_poplog();

    // clear context
    type_engine_unload_context( env );

    // commit what is in the type checker at this point
    env->global()->commit();

    return TRUE;
}




#ifdef __PLATFORM_LINUX__
#include <gtk/gtk.h>

int g_argc;
const char ** g_argv;

// gtk thread
void * cb_gtk( void * )
{
    // init
    gtk_init( &g_argc, (char ***)&g_argv );
    // just run it
    gtk_main();
    return NULL;
}

CHUCK_THREAD g_gtk_id;

#endif




//-----------------------------------------------------------------------------
// name: main()
// desc: entry point
//-----------------------------------------------------------------------------
int main( int argc, const char ** argv )
{
    t_CKBOOL co = FALSE;
	t_CKBOOL fullscreen = FALSE;
    t_CKINT i = 0;
    t_CKINT log_level = CK_LOG_INFO;

#ifdef __PLATFORM_LINUX__
    // copy
    g_argc = argc;
    g_argv = argv;
    // init
    gtk_init( &g_argc, (char ***)&g_argv );
#endif
    
    // parse command line args
    for( i = 1; i < argc; i++ )
    {
        if( argv[i][0] == '-' )
        {
            if( !strncmp(argv[i], "--log", 5) )
                log_level = argv[i][5] ? atoi( argv[i]+5 ) : CK_LOG_INFO;
            else if( !strncmp(argv[i], "--verbose", 9) )
                log_level = argv[i][9] ? atoi( argv[i]+9 ) : CK_LOG_INFO;
            else if( !strncmp(argv[i], "-v", 2) )
                log_level = argv[i][2] ? atoi( argv[i]+2 ) : CK_LOG_INFO;
			else if( !strncmp(argv[i], "--fullscreen", 12 ) )
				fullscreen = TRUE;
            else if( !strncmp(argv[i], "--version", 9 ) 
                     || !strncmp(argv[i], "--about", 7 ) )
            {
                version();
                exit( 2 );
            }
        }
    }

    // set log level
    EM_setlog( log_level );

    // log
    EM_log( CK_LOG_SYSTEM, "audicle version: %s", AUDICLE_VERSION );
    EM_log( CK_LOG_SYSTEM, "(audicle) initializing..." );

    // initialize graphics
    AudicleGfx::init();

    // initialize main window
    AudicleWindow::main()->init( 1024, 768, 0, 0, "audicle", fullscreen );

    // launch chuck
    chuck_chuck( argc, argv, co );

    // hack: wait for chuck to start
    while( !the() || !the2() || !the3() )
        usleep( 10000 );

    // init audicle api
    init_api( the2()->env );

    // hand off
    the3() = FALSE;

    // wait for real-time audio to start
    // (this is not necessary except to help logging)
    usleep( 100000 );

    // log
    EM_log( CK_LOG_SYSTEM, "(audicle) chuck initialization complete..." );
    EM_log( CK_LOG_SYSTEM, "(audicle) initializing faces..." );
    EM_pushlog();

    // parse command line args
    for( i = 1; i < argc; i++ )
    {
        if( argv[i][0] == '-' )
        {
            if( !strncmp(argv[i], "--face", 6 ) )
                if( parse_face( argv[i]+6 ) == FALSE )
                    return 1;
        }
    }
    
    // add faces to the audicle
    for( i = 0; i < 6; i++ )
    {
        // log
        EM_log( CK_LOG_SYSTEM, "(audicle) face lookup: %i:%s", i+1, g_face_set[i].c_str() );

        // make sure
        AudicleFace * face = find_face( g_face_set[i] );
        if( !face ) assert( FALSE );
        
        // go
        Audicle::instance()->add( face );
    }
    
    /*
    Audicle::instance()->add( new AudicleFaceEditor );
    Audicle::instance()->add( g_face_mouse = new AudicleFaceMouse );
    Audicle::instance()->add( new AudicleFaceVMSpace );
    Audicle::instance()->add( new AudicleFaceShredder );
    Audicle::instance()->add( new AudicleFaceTnT );
    Audicle::instance()->add( g_face_groove = new AudicleFaceGroove );
    Audicle::instance()->add( g_face_floor = new AudicleFaceFloor );
    Audicle::instance()->add( g_el_demo_face = new ElDemoFace ); //elcidua
    */

    // log
    EM_poplog();

    // audicle api
    // init_api( the2()->env );

    // log
    EM_poplog();

    // move to the initial face
    Audicle::instance()->move_to( (t_CKUINT)0 );
    
    // set state
    CoAudicle::instance()->set_vm( the() );
    CoAudicle::instance()->set_compiler( the2() );

	//this should be enough to keep the little guy happy. 
	CoSession* local = ClientSessionManagerImp::instance()->new_session( "demo session", 0 , 0);
	ClientSessionManagerImp::instance()->make_cur_session( local );

    // initialize co-audicle
    if( co )
    {
        // log
        EM_log( CK_LOG_SYSTEM, "(audicle) starting co-audicle..." );

        // getip
        CoAudicle::instance()->getip();

        // start server
        CoAudicle::instance()->server.start( CO_REQUEST_PORT, CO_UPDATE_PORT );
        // start coco server
        CoAudicle::instance()->coco_server.initialize();
        
        // initialize audio server
        CoAudicle::instance()->audio_server.initialize( CO_BUFFER_SIZE, 4 );
        // initialize audio client
        CoAudicle::instance()->audio_client.initialize( CO_BUFFER_SIZE, 32, CO_AUDIO_PORT, NULL );
        
        // start the client
        CoAudicle::instance()->audio_client.start();
        
        // start the server
        CoAudicle::instance()->audio_server.add( CoAudicle::instance()->the_ip.c_str(), CO_AUDIO_PORT );
        CoAudicle::instance()->audio_server.start();
        
        // start audicle session
        start_local_coaudicle_session();

        // CoSession * local = ClientSessionManagerImp::instance()->new_session( "fake session", 0,0 );
        // ClientSessionManagerImp::instance()->make_cur_session( local );
    }

    audicle_usage();

    // main loop
    AudicleGfx::loop();

    // done
    // chuck_clean();

    return 0;
}




//-----------------------------------------------------------------------------
// name: main3()
// desc: entry point
//-----------------------------------------------------------------------------
int main3( int argc, char ** argv )
{
    EM_setlog( 9 );
    if( argc == 1 )
    {
        AudicleServer server;
        server.start( CO_REQUEST_PORT, CO_UPDATE_PORT );

        AudiclePak packet;
        packet.user = 1;

        while( true )
        {
            server.broadcast( &packet );
            packet.user += 2;
            usleep( 1000000 );
        }
        //while( true )
        //    usleep( 100000 );
    }
    else
    {
        AudicleClient client;
        client.connect( argv[1], CO_REQUEST_PORT, CO_UPDATE_PORT );

        AudiclePak packet;
        packet.user = 2;
        
        while( true )
        {
            client.queue( &packet );
            packet.user += 2;
            usleep( 1000000 );
        }
    }

    return 0;
}




//-----------------------------------------------------------------------------
// name: main4()
// desc: entry point
//-----------------------------------------------------------------------------
int main4( int argc, char ** argv )
{
    EM_setlog( 9 );

    CoAudicle::instance()->set_vm( new Chuck_VM );
    CoAudicle::instance()->set_compiler( new Chuck_Compiler );
    Chuck_Stats::instance()->set_vm_ref( CoAudicle::instance()->vm() );

    // initialize audio server
    CoAudicle::instance()->audio_server.initialize( 4192, 4 );
    // initialize audio client
    CoAudicle::instance()->audio_client.initialize( 4192, 100, 9999, NULL );

    // parse, type-check, and emit
    if( !CoAudicle::instance()->compiler()->go( argv[1] ) )
        return 0;

    // get the code
    Chuck_VM_Code * code = CoAudicle::instance()->compiler()->output();
    // name it
    code->name += string(argv[1]);
    // spork it
    CoAudicle::instance()->vm()->spork( code, NULL );
        
    // start the client
    CoAudicle::instance()->audio_client.start();
    // start the server
    CoAudicle::instance()->audio_server.add( "localhost", 9999 );
    CoAudicle::instance()->audio_server.start();

    while( true )
        usleep( 100000 );

    return 0;
}



//-----------------------------------------------------------------------------
// name: audicle_usage()
// desc: print usage
//-----------------------------------------------------------------------------
void audicle_usage()
{
    fprintf( stderr, "\n------------ audicle command line flags ------------\n" );
    fprintf( stderr, "'--version' - print version information\n" );
    fprintf( stderr, "'--logN' or '--verboseN' or '-vN' - set log level to N\n" );
    fprintf( stderr, "'--fullscreen' - start in full screen mode\n" );
    fprintf( stderr, "'--face:n:id' - add face 'id' as nth face of cube (n < 6)\n" );
    fprintf( stderr, "                (see below for face id)\n\n" );
    fprintf( stderr, "   (also see ChucK command line flags)\n" );
	fprintf( stderr, "\n------------ (from any face) -----------------------\n" );
    fprintf( stderr, "'ctrl + arrow' - switch faces\n" );
    fprintf( stderr, "'ctrl + g' - toggle full screen\n" );
    fprintf( stderr, "'ctrl + q' or 'apple + q' - quit\n" );
    fprintf( stderr, "'`' - show/hide audicle console\n" );
    fprintf( stderr, "\n------------ ShrEditor (id: editor) ----------------\n" );
    fprintf( stderr, "'ctrl + f' or 'ctrl + o' - open a ChucK file\n" );
    fprintf( stderr, "'ctrl + n' - create new edit buffer\n" );
	fprintf( stderr, "'ctrl + u' - line up edit buffers nicely\n" );
    fprintf( stderr, "\n------------ VM Space (id: vmspace) ----------------\n" );
    fprintf( stderr, "[mouse drag up/down] - rotate view\n" );
    fprintf( stderr, "\n------------ Shredder (id: shredder) ---------------\n" );
	fprintf( stderr, "[mouse drag up/down] - rotate view\n" );
    fprintf( stderr, "\n------------ Time 'n' Timing (id: tnt) -------------\n" );
    fprintf( stderr, "'w' - toggle 'now' location on screen\n" );
    fprintf( stderr, "'a' - zoom in (finer time granularity)\n" );
    fprintf( stderr, "'d' - zoom out (more history)\n" );
	fprintf( stderr, "'s' - toggle spike height\n" );
	fprintf( stderr, "\n------------ Co-Audicle (id: co) -------------------\n" );
	fprintf( stderr, "'1' - show / hide dude\n" );
	fprintf( stderr, "'5' - show / hide bindle\n" );
	fprintf( stderr, "'9' - show / hide halo\n" );
	fprintf( stderr, "'e' - make dude lean back and look around\n" );
	fprintf( stderr, "'r' - make dude wave both arms\n" );
	fprintf( stderr, "'t' - make dude wave one arm\n" );
	fprintf( stderr, "'y' - made dude nod 5 times\n" );
	fprintf( stderr, "'u' - make dude nod once\n" );
	fprintf( stderr, "'i' - make dude's legs long\n" );
	fprintf( stderr, "'space bar' - make dude jump\n" );
	fprintf( stderr, "'=' - add another dude\n" );
	fprintf( stderr, "'-' - remove a dude\n" );
	fprintf( stderr, "'0' - show or hide extra dudes\n" );
	fprintf( stderr, "'d', 'f', or 'j' - make extra dudes copy main dude\n" );
    fprintf( stderr, "\n------------ Non-Specific (id: groove) -------------\n" );
    fprintf( stderr, "[1-9] - selects color\n" );
    fprintf( stderr, "'0' - selects blank\n" );
    fprintf( stderr, "[left mouse button] - place color\n" );
    fprintf( stderr, "'A' - set all to selected color\n" );
    fprintf( stderr, "'C' - set all to blank\n" );
    fprintf( stderr, "\n------------ Skot Machine (id: slotter) ------------\n" );
    fprintf( stderr, "[1-3] - bets 1, 2, 3\n" );
    fprintf( stderr, "\n------------ ChucK ChucK Rocket (id: ccr) ----------\n" );
    fprintf( stderr, "[1-5] - selects sound\n" );
    fprintf( stderr, "[a,s,d,w] - left,down,right,up arrow\n" );
    fprintf( stderr, "'x' - remove arrow\n" );
    fprintf( stderr, "[left mouse button] - select tile\n" );
    fprintf( stderr, "ctrl + [mouse drag up/down] - rotate view\n" );
    fprintf( stderr, "shift + [mouse drag up/down] - zoom view\n" );
    fprintf( stderr, "ctrl + shift + [mouse drag up/down] - field of view\n" );
    fprintf( stderr, "----------------------------------------------------\n" );
    version();
}



//-----------------------------------------------------------------------------
// name: version()
// desc: ...
//-----------------------------------------------------------------------------
void version()
{
    fprintf( stderr, "\n" );
    fprintf( stderr, "version:\n" );
    fprintf( stderr, "    |=> audicle: %s\n", AUDICLE_VERSION );
    fprintf( stderr, "    |=>   chuck: %s\n", CK_VERSION );
#if defined(__PLATFORM_WIN32__)
    fprintf( stderr, "   exe target: microsoft win32\n" );
#elif defined(__WINDOWS_DS__)
    fprintf( stderr, "   exe target: microsoft win32 + cygwin\n" );
#elif defined(__LINUX_ALSA__)
    fprintf( stderr, "   exe target: linux (alsa)\n" );
#elif defined(__LINUX_OSS__)
    fprintf( stderr, "   exe target: linux (oss)\n" );
#elif defined(__LINUX_JACK__)
    fprintf( stderr, "   exe target: linux (jack)\n" );
#elif defined(__MACOSX_UB__)
    fprintf( stderr, "   exe target: mac os x : universal binary\n" );
#elif defined(__MACOSX_CORE__) && defined(__LITTLE_ENDIAN)
    fprintf( stderr, "   exe target: mac os x : intel\n" );
#elif defined(__MACOSX_CORE__)
    fprintf( stderr, "   exe target: mac os x : powerpc\n" );
#else
    fprintf( stderr, "   exe target: uh... unknown\n" );
#endif
    fprintf( stderr, "   http://audicle.cs.princeton.edu/\n\n" );
}
