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
// file: audicle_nexus.cpp
// desc: chuck entry point
//
// author: Ge Wang (gewang@cs.princeton.edu)
//         Perry R. Cook (prc@cs.princeton.edu)
//         Ananya Misra (amisra@cs.princeton.edu)
//         Philip Davidson (philipd@cs.princeton.edu)
// date: version 1.1.x.x - Autumn 2002
//       version 1.2.x.x - Autumn 2004
//-----------------------------------------------------------------------------
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "chuck_compile.h"
#include "chuck_vm.h"
#include "chuck_bbq.h"
#include "chuck_errmsg.h"
#include "chuck_lang.h"
#include "chuck_otf.h"
#include "chuck_globals.h"

#include "util_thread.h"
#include "util_network.h"
#include "util_string.h"

#include <signal.h>
#ifndef __PLATFORM_WIN32__
  #define CHUCK_THREAD pthread_t
  #include <pthread.h>
  #include <unistd.h>
  #include <netinet/in.h>
  #include <arpa/inet.h>
#else 
  #define CHUCK_THREAD HANDLE
#endif

#include "audicle_nexus.h"

using namespace std;




// global variables
#if defined(__MACOSX_CORE__)
  t_CKINT g_priority = 85;
#elif defined(__PLATFORM_WIN32__)
  t_CKINT g_priority = 0;
#else
  t_CKINT g_priority = 0x7fffffff;
#endif


// thread id for second thread
CHUCK_THREAD g_tid = 0;

// default destination host name
char g_host[256] = "127.0.0.1";

// audicle stuff
struct commandline { int argc; const char ** argv; t_CKBOOL co; };
CHUCK_THREAD g_tid2 = 0;
CHUCK_THREAD g_tid3 = 0;
void * go_chuck( void * );
int main2( int argc, const char ** argv );
int main2b( int argc, const char ** argv );
t_CKBOOL g_init_done = FALSE;
t_CKBOOL g_init_done2 = FALSE;

// extern
t_CKBOOL init_api( Chuck_Env * env );

// the main to chuck
t_CKBOOL chuck_chuck( int argc, const char ** argv, t_CKBOOL co )
{
    if( g_vm )
    {
        fprintf( stderr, "[audicle](nexus): chuck already chucked...\n" );
        return FALSE;
    }

    // log
    EM_log( CK_LOG_SYSTEM, "initializing chuck..." );

    // pass the commandline
    commandline * a = new commandline; a->argc = argc; a->argv = argv; a->co = co;

#ifndef __PLATFORM_WIN32__
    pthread_create( &g_tid2, NULL, go_chuck, (void *)a );
#else
    g_tid2 = CreateThread( NULL, 0, (LPTHREAD_START_ROUTINE)go_chuck, (void *)a, 0, 0 );
#endif

    return TRUE;
}

// start chuck
void * go_chuck( void * b )
{
    commandline * a = (commandline *)b;
    if( !a->co ) main2( a->argc, a->argv );
    else main2b( a->argc, a->argv );

    return NULL;
}

// return the virtual machine
Chuck_VM * the()
{
    return g_vm;
}Chuck_Compiler * the2()
 {
     return g_compiler;
 }t_CKBOOL & the3()
  {
     return g_init_done;
  }


// clean up
void chuck_clean2( Chuck_VM * vm )
{
    if( vm )
    {
        EM_log( CK_LOG_SYSTEM, "(nexus) stopping virtual machine..." );
        EM_pushlog();
        // stop
        vm->stop();
        EM_log( CK_LOG_SYSTEM, "(nexus) detaching open STK/MIDI file handles..." );
        // detach
        all_detach();
        EM_poplog();
    }
}

// done
void chuck_clean()
{
    chuck_clean2( g_vm );
    g_vm = NULL;
}




//-----------------------------------------------------------------------------
// name: timer()
// desc: ...
//-----------------------------------------------------------------------------
void * timer( void * p )
{
    if ( p ) { 
        t_CKUINT t = *(t_CKUINT *)p;
        usleep( t );
    }
    fprintf( stderr, "[chuck]: operation timed out...\n" );
    exit(1);
}




//-----------------------------------------------------------------------------
// name: reply_sucker()
// desc: ...
//-----------------------------------------------------------------------------
void * reply_sucker( void * a )
{
    Chuck_Msg * msg = NULL;
    while( g_vm )
    {
        msg = g_vm->get_reply();
        if( !msg ) usleep( 50000 );
        else
        {
            msg->reply( msg );
            delete msg;
        }
    }

    return NULL;
}

// process message
t_CKUINT process_msg2( Net_Msg * msg, ck_msg_func reply, void * data,
                       FILE * fd )
{
    Chuck_Msg * cmd = new Chuck_Msg;
    Chuck_VM_Code * code = NULL;
    
    // fprintf( stderr, "UDP message recv...\n" );
    if( msg->type == MSG_REPLACE || msg->type == MSG_ADD )
    {
        assert( !msg->param2 );

        // parse, type-check, and emit
        if( !g_compiler->go( msg->buffer, fd ) )
            return 0;

        // get the code
        code = g_compiler->output();
        // name it
        code->name += string(msg->buffer);

        // set the flags for the command
        cmd->type = msg->type;
        cmd->code = code;
        if( msg->type == MSG_REPLACE )
            cmd->param = msg->param;
    }
    else if( msg->type == MSG_STATUS || msg->type == MSG_REMOVE || msg->type == MSG_REMOVEALL
             || msg->type == MSG_KILL || msg->type == MSG_TIME )
    {
        cmd->type = msg->type;
        cmd->param = msg->param;
    }
    else
    {
        fprintf( stderr, "[chuck]: unrecognized incoming command from network: '%ld'\n", cmd->type );
        SAFE_DELETE(cmd);
        return 0;
    }

    cmd->reply = reply;
    cmd->user = data;

    g_vm->queue_msg( cmd, 1 );

    return 1;

// error:

    return 0;
}




//-----------------------------------------------------------------------------
// name: signal_int()
// desc: ...
//-----------------------------------------------------------------------------
extern "C" void signal_int( int sig_num )
{
    fprintf( stderr, "[chuck]:(from nexus) cleaning up...\n" );

    if( g_vm )
    {
        // get vm
        Chuck_VM * vm = g_vm;
        // flag the global one
        g_vm = NULL;
        // clean
        chuck_clean2( vm );

        // things don't work so good on windows...
#ifndef __PLATFORM_WIN32__
        // pthread_kill( g_tid, 2 );
        if( g_tid ) pthread_cancel( g_tid );
        // log
        EM_log( CK_LOG_SYSTEM, "(nexus) cleaning up the virtual machine..." );
        // if( g_tid ) usleep( 50000 );
        SAFE_DELETE( vm );
#else
        // close handle
        if( g_tid ) CloseHandle( g_tid );
#endif
        // ck_close( g_sock );
    }

#ifndef __PLATFORM_WIN32__
    // pthread_join( g_tid, NULL );
#endif

    // log
    EM_log( CK_LOG_SYSTEM, "you have successfully escaped from the audicle! (maybe)" );
    
    exit(2);
}




//-----------------------------------------------------------------------------
// name: next_power_2()
// desc: ...
// thanks: to Niklas Werner / music-dsp
//-----------------------------------------------------------------------------
t_CKUINT next_power_2( t_CKUINT n )
{
    t_CKUINT nn = n;
    for( ; n &= n-1; nn = n );
    return nn * 2;
}




//-----------------------------------------------------------------------------
// name: uh()
// desc: ...
//-----------------------------------------------------------------------------
void uh( )
{
    // TODO: play white noise and/or sound effects
    srand( time( NULL ) );
    while( true )
    {
        int n = (int)(rand() / (float)RAND_MAX * poop_size);
        printf( "%s\n", poop[n] );
        usleep( (unsigned long)(rand() / (float)RAND_MAX * 2000000) );
    }
}




//-----------------------------------------------------------------------------
// name: get_count()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL get_count( const char * arg, t_CKUINT * out )
{
    // no comment
    if( !strncmp( arg, "--", 2 ) ) arg += 2;
    else if( !strncmp( arg, "-", 1 ) ) arg += 1;
    else return FALSE;

    // end of string
    if( *arg == '\0' ) return FALSE;
    // not digit
    if( *arg < '0' || *arg > '9' ) return FALSE;

    // number
    *out = (t_CKUINT)atoi( arg );

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: usage()
// desc: ...
//-----------------------------------------------------------------------------
void usage()
{
    fprintf( stderr, "usage: chuck --[options|commands] [+-=^] file1 file2 file3 ...\n" );
    fprintf( stderr, "   [options] = halt|loop|audio|silent|dump|nodump|about\n" );
    fprintf( stderr, "               srate<N>|bufsize<N>|bufnum<N>|dac<N>|adc<N>|\n" );
    fprintf( stderr, "               remote<hostname>|port<N>|verbose<N>|probe\n" );
    fprintf( stderr, "   [commands] = add|remove|replace|status|time|kill\n" );
    fprintf( stderr, "   [+-=^] = shortcuts for add, remove, replace, status\n\n" );
    fprintf( stderr, "chuck version: %s\n", CK_VERSION );
    fprintf( stderr, "   http://chuck.cs.princeton.edu/\n\n" );
}




//-----------------------------------------------------------------------------
// name: main2()
// desc: entry point
//-----------------------------------------------------------------------------
int main2( int argc, const char ** argv )
{
    Chuck_Compiler * compiler = NULL;
    Chuck_VM * vm = NULL;
    Chuck_VM_Code * code = NULL;
    Chuck_VM_Shred * shred = NULL;
    
    t_CKBOOL enable_audio = TRUE;
    t_CKBOOL vm_halt = FALSE;
    t_CKUINT srate = SAMPLING_RATE_DEFAULT;
#ifdef __WINDOWS_DS__
    t_CKUINT buffer_size = 512;
#else
    t_CKUINT buffer_size = 1024;
#endif
    t_CKUINT num_buffers = NUM_BUFFERS_DEFAULT;
    t_CKUINT dac = 0;
    t_CKUINT adc = 0;
    t_CKUINT dac_chans = 2;
    t_CKUINT adc_chans = 2;
    t_CKBOOL dump = FALSE;
    t_CKBOOL probe = FALSE;
    t_CKBOOL set_priority = FALSE;
    t_CKBOOL auto_depend = FALSE;
    t_CKBOOL block = FALSE;
    t_CKINT  log_level = CK_LOG_SYSTEM;

    t_CKUINT files = 0;
    t_CKUINT count = 1;
    t_CKINT i;
    
    string filename;
    vector<string> args;

    // parse command line args
    for( i = 1; i < argc; i++ )
    {
        if( argv[i][0] == '-' || argv[i][0] == '+' ||
            argv[i][0] == '=' || argv[i][0] == '^' || argv[i][0] == '@' )
        {
            if( !strcmp(argv[i], "--dump") || !strcmp(argv[i], "+d")
                || !strcmp(argv[i], "--nodump") || !strcmp(argv[i], "-d") )
                continue;
            else if( get_count( argv[i], &count ) )
                continue;
            else if( !strcmp(argv[i], "--audio") || !strcmp(argv[i], "-a") )
                enable_audio = TRUE;
            else if( !strcmp(argv[i], "--silent") || !strcmp(argv[i], "-s") )
                enable_audio = FALSE;
            // else if( !strcmp(argv[i], "--halt") || !strcmp(argv[i], "-t") )
            //     vm_halt = TRUE;
            else if( !strcmp(argv[i], "--loop") || !strcmp(argv[i], "-l") )
                vm_halt = FALSE;
            else if( !strcmp(argv[i], "--callback") )
                block = FALSE;
            else if( !strcmp(argv[i], "--blocking") )
                block = TRUE;
            else if( !strncmp(argv[i], "--srate", 7) )
                srate = atoi( argv[i]+7 ) > 0 ? atoi( argv[i]+7 ) : srate;
            else if( !strncmp(argv[i], "-r", 2) )
                srate = atoi( argv[i]+2 ) > 0 ? atoi( argv[i]+2 ) : srate;
            else if( !strncmp(argv[i], "--bufsize", 9) )
                buffer_size = atoi( argv[i]+9 ) > 0 ? atoi( argv[i]+9 ) : buffer_size;
            else if( !strncmp(argv[i], "-b", 2) )
                buffer_size = atoi( argv[i]+2 ) > 0 ? atoi( argv[i]+2 ) : buffer_size;
            else if( !strncmp(argv[i], "--bufnum", 8) )
                num_buffers = atoi( argv[i]+8 ) > 0 ? atoi( argv[i]+8 ) : num_buffers;
            else if( !strncmp(argv[i], "-n", 2) )
                num_buffers = atoi( argv[i]+2 ) > 0 ? atoi( argv[i]+2 ) : num_buffers;
            else if( !strncmp(argv[i], "--dac", 5) )
                dac = atoi( argv[i]+5 ) > 0 ? atoi( argv[i]+5 ) : 0;
            else if( !strncmp(argv[i], "--adc", 5) )
                adc = atoi( argv[i]+5 ) > 0 ? atoi( argv[i]+5 ) : 0;
            else if( !strncmp(argv[i], "--chan", 6) )
                dac_chans = adc_chans = atoi( argv[i]+6 ) > 0 ? atoi( argv[i]+6 ) : 2;
            else if( !strncmp(argv[i], "-c", 2) )
                dac_chans = adc_chans = atoi( argv[i]+2 ) > 0 ? atoi( argv[i]+2 ) : 2;
            else if( !strncmp(argv[i], "--level", 7) )
            {   g_priority = atoi( argv[i]+7 ); set_priority = TRUE; }
            else if( !strncmp(argv[i], "--remote", 8) )
                strcpy( g_host, argv[i]+8 );
            else if( !strncmp(argv[i], "@", 1) )
                strcpy( g_host, argv[i]+1 );
            else if( !strncmp(argv[i], "--port", 6) )
                g_port = atoi( argv[i]+6 );
            else if( !strncmp(argv[i], "-p", 2) )
                g_port = atoi( argv[i]+2 );
            else if( !strncmp(argv[i], "--auto", 6) )
                auto_depend = TRUE;
            else if( !strncmp(argv[i], "-u", 2) )
                auto_depend = TRUE;
            else if( !strncmp(argv[i], "--log", 5) )
                log_level = argv[i][5] ? atoi( argv[i]+5 ) : CK_LOG_INFO;
            else if( !strncmp(argv[i], "--verbose", 9) )
                log_level = argv[i][9] ? atoi( argv[i]+9 ) : CK_LOG_INFO;
            else if( !strncmp(argv[i], "-v", 2) )
                log_level = argv[i][2] ? atoi( argv[i]+2 ) : CK_LOG_INFO;
            else if( !strcmp( argv[i], "--probe" ) )
                probe = TRUE;
            else if( !strcmp( argv[i], "--poop" ) )
                uh();
			else if( !strncmp(argv[i], "--fullscreen", 12 ) )
				;
            else if( !strncmp(argv[i], "--face", 6 ) )
                ;
			else if( !strcmp(argv[i], "--help") || !strcmp(argv[i], "-h")
                 || !strcmp(argv[i], "--about") )
            {
                usage();
                exit( 2 );
            }
            else if( otf_send_cmd( argc, argv, i, g_host, g_port ) )
                exit( 0 );
            else
            {
                fprintf( stderr, "[chuck]: invalid flag '%s'\n", argv[i] );
                usage();
                exit( 1 );
            }
        }
        else
            files++;
    }
    
    // probe
    if( probe )
    {
        Digitalio::probe();

        EM_error2b( 0, "" );
        probeMidiIn();
        EM_error2b( 0, "" );
        probeMidiOut();
        EM_error2b( 0, "" );
    
        // exit
        exit( 0 );
    }
    
    // check buffer size
    buffer_size = next_power_2( buffer_size-1 );
    // audio, boost
    if( !set_priority && !enable_audio ) g_priority = 0x7fffffff;
    // set priority
    Chuck_VM::our_priority = g_priority;

    // HACK
    // if( !strncmp(mini(argv[0]), "audicle", 7 ) )
    //    vm_halt = FALSE;

    if ( !files && vm_halt )
    {
        fprintf( stderr, "[chuck]: no input files... (try --help)\n" );
        exit( 1 );
    }

    // set log level
    // EM_setlog( log_level );
    
    // output version
    EM_log( CK_LOG_SYSTEM, "chuck version: %s", CK_VERSION );

    // allocate the vm - needs the type system
    vm = g_vm = new Chuck_VM;
    if( !vm->initialize( enable_audio, vm_halt, srate, buffer_size,
                         num_buffers, dac, adc, dac_chans, adc_chans, block ) )
    {
        fprintf( stderr, "[chuck]: %s\n", vm->last_error() );
        exit( 1 );
    }

    // allocate the compiler
    compiler = g_compiler = new Chuck_Compiler;
    std::list<std::string> chugin_search_paths;
    std::list<std::string> named_dls;
    // initialize the compiler
    compiler->initialize( vm, chugin_search_paths, named_dls );
    // enable dump
    compiler->emitter->dump = dump;
    // set auto depend
    compiler->set_auto_depend( auto_depend );

    // vm synthesis subsystem - needs the type system
    if( !vm->initialize_synthesis( ) )
    {
        fprintf( stderr, "[chuck]: %s\n", vm->last_error() );
        exit( 1 );
    }

    // set reference
    Chuck_Stats::instance()->set_vm_ref( vm );

    // catch SIGINT
    signal( SIGINT, signal_int );
#ifndef __PLATFORM_WIN32__
    // catch SIGPIPE
    signal( SIGPIPE, signal_pipe );
#endif

    // reset count
    count = 1;

    // done
    g_init_done = TRUE;

    // wait
    while( g_init_done )
        usleep( 10000 );

    // log
    EM_log( CK_LOG_SYSTEM, "starting compilation..." );
    // push indent
    EM_pushlog();

    // loop through and process each file
    for( i = 1; i < argc; i++ )
    {
        // make sure
        if( argv[i][0] == '-' || argv[i][0] == '+' )
        {
            if( !strcmp(argv[i], "--dump") || !strcmp(argv[i], "+d" ) )
                compiler->emitter->dump = TRUE;
            else if( !strcmp(argv[i], "--nodump") || !strcmp(argv[i], "-d" ) )
                compiler->emitter->dump = FALSE;
            else
                get_count( argv[i], &count );

            continue;
        }

        // parse out command line arguments
        if( !extract_args( argv[i], filename, args ) )
        {
            // error
            fprintf( stderr, "[chuck]: malformed filename with argument list...\n" );
            fprintf( stderr, "    -->  '%s'", argv[i] );
            exit( 1 );
        }

        // log
        EM_log( CK_LOG_FINE, "compiling '%s'...", filename.c_str() );
        // push indent
        EM_pushlog();

        // parse, type-check, and emit
        if( !compiler->go( filename.c_str(), NULL ) )
            exit( 1 );

        // get the code
        code = compiler->output();
        // name it
        code->name += string(filename.c_str());

        // log
        EM_log( CK_LOG_FINE, "sporking %d %s...", count,
                count == 1 ? "instance" : "instances" );

        // spork it
        while( count-- )
        {
            shred = vm->spork( code, NULL );
            shred->args = args;
        }

        // pop indent
        EM_poplog();

        // reset count
        count = 1;
    }

    // pop indent
    EM_poplog();

    // reset the parser
    reset_parse();

    // boost priority
    if( Chuck_VM::our_priority != 0x7fffffff )
    {
        // try
        if( !Chuck_VM::set_priority( Chuck_VM::our_priority, vm ) )
        {
            // error
            fprintf( stderr, "[chuck]: %s\n", vm->last_error() );
            exit( 1 );
        }
    }

    // log
    EM_log( CK_LOG_SYSTEM, "(nexus) starting listener on port: %ld...", g_port );

    // start tcp server
    g_sock = ck_tcp_create( 1 );
    if( !g_sock || !ck_bind( g_sock, g_port ) || !ck_listen( g_sock, 10 ) )
    {
        fprintf( stderr, "(nexus) cannot bind to tcp port %ld...\n", g_port );
        ck_close( g_sock );
        g_sock = NULL;
    }
    else
    {
#ifndef __PLATFORM_WIN32__
        pthread_create( &g_tid, NULL, otf_cb, NULL );
        pthread_create( &g_tid3, NULL, reply_sucker, NULL );
#else
        g_tid = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)otf_cb, NULL, 0, 0);
        g_tid3 = CreateThread( NULL, 0, (LPTHREAD_START_ROUTINE)reply_sucker, NULL, 0, 0 );
#endif
    }

    // done
    g_init_done = TRUE;

    // run the vm
    vm->run();

    // detach
    all_detach();

    // free vm
    g_vm = NULL; SAFE_DELETE( vm );
    // free the compiler
    SAFE_DELETE( compiler );

    return 0;
}




//-----------------------------------------------------------------------------
// name: main2b()
// desc: entry point
//-----------------------------------------------------------------------------
int main2b( int argc, const char ** argv )
{
    t_CKINT i;
    t_CKINT log_level = CK_LOG_SYSTEM;
    
    // parse command line args
    for( i = 1; i < argc; i++ )
    {
        if( argv[i][0] == '-' || argv[i][0] == '+' ||
            argv[i][0] == '=' || argv[i][0] == '^' || argv[i][0] == '@' )
        {
            if( !strcmp(argv[i], "--dump") || !strcmp(argv[i], "+d")
                || !strcmp(argv[i], "--nodump") || !strcmp(argv[i], "-d") )
                continue;
            else if( !strncmp(argv[i], "--level", 7) )
            {   g_priority = atoi( argv[i]+7 ); }
            else if( !strncmp(argv[i], "--log", 5) )
                log_level = argv[i][5] ? atoi( argv[i]+5 ) : CK_LOG_INFO;
            else if( !strncmp(argv[i], "--verbose", 9) )
                log_level = argv[i][9] ? atoi( argv[i]+9 ) : CK_LOG_INFO;
            else if( !strncmp(argv[i], "-v", 2) )
                log_level = argv[i][2] ? atoi( argv[i]+2 ) : CK_LOG_INFO;
            else if( !strcmp( argv[i], "--poop" ) )
                uh();
            else if( !strcmp(argv[i], "--help") || !strcmp(argv[i], "-h")
                 || !strcmp(argv[i], "--about") )
            {
                usage();
                exit( 2 );
            }
            else
            {
                fprintf( stderr, "[audicle]: invalid flag '%s'\n", argv[i] );
                usage();
                exit( 1 );
            }
        }
    }

    // set priority
    Chuck_VM::our_priority = g_priority;

    // set log level
    EM_setlog( log_level );

    // initialize the vm
    Chuck_VM * vm = g_vm = new Chuck_VM;
    if( !vm->initialize(
        FALSE, // no realtime
        FALSE, // don't halt
        44100, // sample rate
        CO_BUFFER_SIZE, // buffer size
        8, // num_buffers (not used)
        0, // dac (not used)
        0, // adc = 0 (not used)
        2, // channels
        2, // channels
        FALSE // callback (not used)
        ) )
    {
        fprintf( stderr, "[chuck]: %s\n", vm->last_error() );
        exit( 1 );
    }

    // initialize the compiler
    Chuck_Compiler * compiler = g_compiler = new Chuck_Compiler;
    std::list<std::string> chugin_search_paths;
    std::list<std::string> named_dls;
    compiler->initialize( vm, chugin_search_paths, named_dls );
    // enable dump
    compiler->emitter->dump = FALSE;
    // set auto depend
    compiler->set_auto_depend( FALSE );
    
    // initialize synthesis
    if( !vm->initialize_synthesis() )
    {
        fprintf( stderr, "[chuck]: %s\n", vm->last_error() );
        exit( 1 );
    }

    // shreduler
    vm->compensate_bbq();
    // set reference
    Chuck_Stats::instance()->set_vm_ref( vm );

    // catch SIGINT
    signal( SIGINT, signal_int );
#ifndef __PLATFORM_WIN32__
    // catch SIGPIPE
    signal( SIGPIPE, signal_pipe );
#endif

    // reset count
    t_CKUINT count = 1;

    // log
    EM_log( CK_LOG_SYSTEM, "starting compilation..." );
    // push indent
    EM_pushlog();

    // loop through and process each file
    for( i = 1; i < argc; i++ )
    {
        // make sure
        if( argv[i][0] == '-' || argv[i][0] == '+' )
        {
            if( !strcmp(argv[i], "--dump") || !strcmp(argv[i], "+d" ) )
                compiler->emitter->dump = TRUE;
            else if( !strcmp(argv[i], "--nodump") || !strcmp(argv[i], "-d" ) )
                compiler->emitter->dump = FALSE;
            else
                get_count( argv[i], &count );

            continue;
        }

        // log
        EM_log( CK_LOG_FINE, "compiling '%s'...", argv[i] );
        // push indent
        EM_pushlog();

        // parse, type-check, and emit
        if( !compiler->go( argv[i], NULL ) )
            return 1;

        // get the code
        Chuck_VM_Code * code = compiler->output();
        // name it
        code->name += string(argv[i]);

        // log
        EM_log( CK_LOG_FINE, "sporking %d %s...", count,
                count == 1 ? "instance" : "instances" );

        // spork it
        while( count-- ) /*shred =*/ vm->spork( code, NULL );

        // pop indent
        EM_poplog();

        // reset count
        count = 1;
    }

    // pop indent
    EM_poplog();

    // reset the parser
    reset_parse();

    // boost priority
    if( Chuck_VM::our_priority != 0x7fffffff )
    {
        // try
        if( !Chuck_VM::set_priority( Chuck_VM::our_priority, vm ) )
        {
            // error
            fprintf( stderr, "[chuck]: %s\n", vm->last_error() );
            exit( 1 );
        }
    }

    // log
    EM_log( CK_LOG_SYSTEM, "starting listener on port: %ld...", g_port );

    // start tcp server
    g_sock = ck_tcp_create( 1 );
    if( !g_sock || !ck_bind( g_sock, g_port ) || !ck_listen( g_sock, 10 ) )
    {
        fprintf( stderr, "[chuck]: cannot bind to tcp port %ld...\n", g_port );
        ck_close( g_sock );
        g_sock = NULL;
    }
    else
    {
#ifndef __PLATFORM_WIN32__
        pthread_create( &g_tid, NULL, otf_cb, NULL );
        pthread_create( &g_tid3, NULL, reply_sucker, NULL );
#else
        g_tid = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)otf_cb, NULL, 0, 0);
        g_tid3 = CreateThread( NULL, 0, (LPTHREAD_START_ROUTINE)reply_sucker, NULL, 0, 0 );
#endif
    }

    // run the vm
    // vm->run();
    vm->m_running = TRUE;
    Chuck_Stats::instance()->set_vm_ref( vm );

    // done
    g_init_done = TRUE;

    while( vm->m_running )
        usleep( 100000 );

    // detach
    all_detach();

    // free vm
    g_vm = NULL; SAFE_DELETE( vm );
    // free the compiler
    SAFE_DELETE( compiler );

    return 0;
}
