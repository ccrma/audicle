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
// name: audicle_net.cpp
// desc: interface for network and audio
//
// authors: Ge Wang (gewang@cs.princeton.edu)
//          Perry R. Cook (prc@cs.princeton.edu)
//          Philip Davidson (philipd@cs.princeton.edu)
//          Ananya Misra (amisra@cs.princeton.edu)
// date: Summer 2005
//-----------------------------------------------------------------------------
#include "audicle_net.h"
#include "coaudicle.h"
#include "coaudicle_protocol.h"
#include "chuck_bbq.h"
#include "chuck_errmsg.h"
#include "audicle_nexus.h"

using namespace std;


// temp
t_CKBOOL SockPack::send( AudiclePak * pack )
{
    if( !sock )
        return FALSE;
        
    // lock
    mutex.acquire();

    // host to network
    pack->hton();
    // send header, subtract size of data, capacity, and sender
    ck_send( sock, (char *)pack, (char *)&pack->capacity - (char *)&pack->header );
    // back
    pack->ntoh();

    // send payload
    
    // NOTE: data should be hton'd before it's sent...
    ck_send( sock, (char *)pack->data, pack->length );

    // unlock
    mutex.release();

    return TRUE;
}

// also temp
t_CKINT SockPack::recv( AudiclePak * pack )
{
    // get the packet header (maybe)
    int size = (char *)&pack->capacity - (char *)&pack->header;
    int n = ck_recv( sock, (char *)pack, size );
    
    // network to host
    pack->ntoh();

    // log
    EM_log( CK_LOG_FINER, "receiving packet header, got %d bytes...", n );
    EM_pushlog();
    EM_log( CK_LOG_FINER, "packet: header %x", pack->header );
    EM_log( CK_LOG_FINER, "packet: type %d", pack->type );
    EM_log( CK_LOG_FINER, "packet: user %d", pack->user );
    EM_log( CK_LOG_FINER, "packet: server %s", pack->server );
    EM_log( CK_LOG_FINER, "packet: length %d", pack->length );
    EM_poplog();

    // check bytes received
    if( n != size )
    {
        EM_log( CK_LOG_SYSTEM, "incomplete packet in audicle..." );
        usleep( 40000 );
        return -1;
    }
    // check header
    if( pack->header != AUDICLE_HEADER )
    {
        EM_log( CK_LOG_SYSTEM, "header mismatch - possible endian lunacy in audicle..." );
        return -1;
    }

    // allocate rest of packet
    if( pack->length > pack->capacity )
        pack->alloc( pack->length );
    
    // get the rest of the packet
    n += ck_recv( sock, (char *)(pack->data), pack->length );

    // set the sender
    pack->sender = this;
    
    return n;
}


//-----------------------------------------------------------------------------
// name: AudicleCS()
// desc: ...
//-----------------------------------------------------------------------------
AudicleCS::AudicleCS()
{
   m_send_queue.initialize( 20, sizeof(AudiclePak *) );
}


//-----------------------------------------------------------------------------
// name: ~AudicleCS()
// desc: ...
//-----------------------------------------------------------------------------
AudicleCS::~AudicleCS()
{
    this->close();
}
    

//-----------------------------------------------------------------------------
// name: queue()
// desc: add packet to send queue
//-----------------------------------------------------------------------------
t_CKBOOL AudicleCS::queue( AudiclePak * packet )
{
    // TODO: maybe take this out
    AudiclePak * copy  = new AudiclePak;
    *copy = *packet;
    copy->data = NULL;
    copy->alloc( packet->capacity );
    memcpy( copy->data, packet->data, copy->length );

    m_send_queue.put( &copy, 1 ); 
    return TRUE;
}


//-----------------------------------------------------------------------------
// name: dequeue()
// desc: because the send queue is not public
//-----------------------------------------------------------------------------
t_CKBOOL AudicleCS::dequeue( AudiclePak ** packet )
{
    return m_send_queue.get( packet, 1 );
}


//-----------------------------------------------------------------------------
// name: close()
// desc: ???
//-----------------------------------------------------------------------------
void AudicleCS::close()
{
    m_send_queue.cleanup();
}



//-----------------------------------------------------------------------------
// name: AudicleServer()
// desc: ...
//-----------------------------------------------------------------------------
AudicleServer::AudicleServer() : AudicleCS()
{
    // nothing?
}




//-----------------------------------------------------------------------------
// name: ~AudicleServer()
// desc: ...
//-----------------------------------------------------------------------------
AudicleServer::~AudicleServer()
{
    this->close();
}




//-----------------------------------------------------------------------------
// name: struct ServerData
// desc: struct to pass data when creating thread
//-----------------------------------------------------------------------------
struct ServerData
{
    AudicleServer * server;
    //ck_socket client;
    SockPack client;

    ServerData( AudicleServer * s, SockPack c )
    { server = s; client = c; }
};




//-----------------------------------------------------------------------------
// name: server_loop_for_each_client()
// desc: ...
//-----------------------------------------------------------------------------
static void * server_loop_for_each_client( void * no )
{
    ServerData * gh = (ServerData *)no;
    AudicleServer * server = gh->server;
    SockPack client = gh->client;
    AudiclePak packet;
    int n;

    // delete good hack
    SAFE_DELETE( gh );

#ifndef __PLATFORM_WIN32__
    // catch SIGPIPE
    // signal( SIGPIPE, signal_pipe );
#endif

    EM_log( CK_LOG_SYSTEM, "audicle server loop for each client starting..." );

    // print socket
    EM_log( CK_LOG_INFO, "socket: %i", client.sock );

    // set time out
    ck_recv_timeout( client.sock, 0, 5000000 );
   
    // woohoo
    while( true )
    {
        // receive packet
        n = client.recv( &packet );
        if( n < 0 )
            break;

        // handle the packet
        if( !server_handle_packet( &packet ) )
        {
            EM_log( CK_LOG_SYSTEM, "error handling packet in audicle...SERVER LOOP ENDED.." );
            break;
        }
    }

    ck_close( client.sock );
    
    return NULL;
}




//-----------------------------------------------------------------------------
// name: server_request_accept_loop()
// desc: ...
//-----------------------------------------------------------------------------
static void * server_request_accept_loop( void * no )
{
    AudicleServer * server = (AudicleServer *)no;
    ck_socket client;
    THREAD_HANDLE id = 0;

#ifndef __PLATFORM_WIN32__
    // catch SIGPIPE
    // signal( SIGPIPE, signal_pipe );
#endif

    EM_log( CK_LOG_SYSTEM, "starting audicle server request accept loop..." );

    // print socket
    EM_log( CK_LOG_INFO, "socket: %i", server->m_request_socket.sock );

    // woohoo
    while( true )
    {
        client = ck_accept( server->m_request_socket.sock );
        if( !client )
        {
            EM_log( CK_LOG_SYSTEM, "socket error during accept()... AUDICLE" );
            usleep( 40000 );
            ck_close( client );
            continue;
        }

        EM_log( CK_LOG_SYSTEM, "client connecting to request channel..." );
        
        // add to list of clients
        SockPack new_client;
        new_client.sock = client;
        server->m_clients.push_back( new_client );

        #ifndef __PLATFORM_WIN32__
            pthread_create( &id, NULL, server_loop_for_each_client, new ServerData( server, new_client ) );
        #else
            /*id =*/ CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)server_loop_for_each_client,
                 new ServerData( server, new_client ), 0, 0);
        #endif
    }
    
    return NULL;
}




//-----------------------------------------------------------------------------
// name: server_update_accept_loop()
// desc: ...
//-----------------------------------------------------------------------------
static void * server_update_accept_loop( void * no )
{
    AudicleServer * server = (AudicleServer *)no;
    ck_socket client;
    THREAD_HANDLE id = 0;

#ifndef __PLATFORM_WIN32__
    // catch SIGPIPE
    // signal( SIGPIPE, signal_pipe );
#endif

    EM_log( CK_LOG_SYSTEM, "starting audicle server update accept loop..." );

    // print socket
    EM_log( CK_LOG_INFO, "socket: %i", server->m_update_socket.sock );

    // woohoo
    while( true )
    {
        client = ck_accept( server->m_update_socket.sock );
        if( !client )
        {
            EM_log( CK_LOG_SYSTEM, "socket error during accept()... AUDICLE" );
            usleep( 40000 );
            ck_close( client );
            continue;
        }

        EM_log( CK_LOG_SYSTEM, "client connecting to update channel..." );
        
        // add to list of clients
        SockPack new_client;
        new_client.sock = client;
        // TODO: match this with the request client: server->m_clients.push_back( new_client );

        AudiclePak pack; 
        if( new_client.recv( &pack ) )
            if( !server_handle_update_info( &pack ) )
            {
                EM_log( CK_LOG_SYSTEM, "i am broken. thanks." );
            }
    
    }
    
    return NULL;
}




//-----------------------------------------------------------------------------
// name: start()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL AudicleServer::start( t_CKINT request_port, t_CKINT update_port )
{
    // start tcp server
    m_request_socket.sock = ck_tcp_create( 1 );
    if( !m_request_socket.sock || !ck_bind( m_request_socket.sock, request_port ) || !ck_listen( m_request_socket.sock, 10 ) )
    {
        fprintf( stderr, "[audicle/server]: cannot bind to tcp port %i...\n", request_port );
        ck_close( m_request_socket.sock );
        m_request_socket.sock = NULL;
        return FALSE;
    }

    EM_log( CK_LOG_SYSTEM, "starting audicle request server on TCP port %d...", request_port );

#ifndef __PLATFORM_WIN32__
    pthread_create( &m_tid, NULL, server_request_accept_loop, this );
#else
    /*m_tid =*/ CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)server_request_accept_loop, this, 0, 0);
#endif

    // start tcp server
    m_update_socket.sock = ck_tcp_create( 1 );
    if( !m_update_socket.sock || !ck_bind( m_update_socket.sock, update_port ) || !ck_listen( m_update_socket.sock, 10 ) )
    {
        fprintf( stderr, "[audicle/server]: cannot bind to tcp port %i...\n", update_port );
        ck_close( m_update_socket.sock );
        m_update_socket.sock = NULL;
        return FALSE;
    }

    EM_log( CK_LOG_SYSTEM, "starting audicle update server on TCP port %d...", update_port );

#ifndef __PLATFORM_WIN32__
    pthread_create( &m_tid, NULL, server_update_accept_loop, this );
#else
    /*m_tid =*/ CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)server_update_accept_loop, this, 0, 0);
#endif

    return TRUE;
}


//-----------------------------------------------------------------------------
// name: broadcast()
// desc: send packet to all clients
//-----------------------------------------------------------------------------
t_CKBOOL AudicleServer::broadcast( AudiclePak * packet )
{
    t_CKBOOL ret = TRUE;
    for( int i = 0; i < m_clients.size(); i++ )
    {
        ret = ret && m_clients[i].send( packet );
    }
    return ret;
}


//-----------------------------------------------------------------------------
// name: AudicleClient()
// desc: ...
//-----------------------------------------------------------------------------
AudicleClient::AudicleClient() : AudicleCS()
{
    // nothing???
}




//-----------------------------------------------------------------------------
// name: ~AudicleClient()
// desc: ...
//-----------------------------------------------------------------------------
AudicleClient::~AudicleClient()
{
    this->close();
}


//-----------------------------------------------------------------------------
// name: client_send_loop()
// desc: ...
//-----------------------------------------------------------------------------
static void * client_send_loop( void * yes )
{
    AudicleClient * sender = (AudicleClient *)yes;
    THREAD_HANDLE id = 0;
    AudiclePak * packet;

#ifndef __PLATFORM_WIN32__
    // catch SIGPIPE
    // signal( SIGPIPE, signal_pipe );
#endif

    EM_log( CK_LOG_SYSTEM, "audicle send loop starting..." );
    
    // print socket
    EM_log( CK_LOG_INFO, "socket: %i", sender->m_request_socket.sock );

    // woohoo
    while( true )
    {
        // get next packet from queue
        while( !sender->dequeue( &packet ) )
        {
            usleep( 40000 );
        }

        // send (should it be doing something else instead of just using the send function?
        if( !sender->send_request( packet ) )
            EM_log( CK_LOG_SYSTEM, "could not send packet in audicle..." );

        SAFE_DELETE( packet );
    }
    
    return NULL;
}


//-----------------------------------------------------------------------------
// name: client_recv_update_loop()
// desc: ...
//-----------------------------------------------------------------------------
static void * client_recv_update_loop( void * no )
{
    AudicleClient * receiver = (AudicleClient *)no;
    AudiclePak packet;
    int n;

#ifndef __PLATFORM_WIN32__
    // catch SIGPIPE
    // signal( SIGPIPE, signal_pipe );
#endif

    EM_log( CK_LOG_SYSTEM, "audicle client receiver loop starting..." );

    // print socket
    EM_log( CK_LOG_INFO, "socket: %i", receiver->m_update_socket.sock );

    // set time out
    ck_recv_timeout( receiver->m_update_socket.sock , 0, 5000000 );
    
    // woohoo
    while( true )
    {
        n = receiver->m_update_socket.recv( &packet ); 
        if( n < 0 )
            break;

        // handle the packet
        if( !client_handle_packet( receiver, &packet ) )
        {
            EM_log( CK_LOG_SYSTEM, "error handling packet in audicle...CLIENT_RECV ENDED" );
            break;
        }
    }

    return NULL;
}



//-----------------------------------------------------------------------------
// name: connect()
// desc: ...
//-----------------------------------------------------------------------------
t_CKUINT AudicleClient::connect( const string & host, t_CKINT request_port, t_CKINT update_port )
{
    m_request_socket.sock = ck_tcp_create( 0 );
    if( !m_request_socket.sock )
    {
        fprintf( stderr, "[audicle]: cannot open request socket to send command...\n" );
        return FALSE;
    }

    EM_log( CK_LOG_SYSTEM, "connecting to %s on TCP port %i (request)...", host.c_str(), request_port );
    
    if( !ck_connect( m_request_socket.sock, host.c_str(), request_port ) )
    {
        fprintf( stderr, "[audicle]: cannot open TCP socket on %s:%i...\n", host.c_str(), request_port );
        return FALSE;
    }
    
    ck_send_timeout( m_request_socket.sock, 0, 2000000 );

    // send JOIN_REQUEST and get answer
    t_CKUINT user_id = client_send_join_request( this, CO_AUDIO_PORT );
    if( user_id == 0 )
        return FALSE;
    
    m_update_socket.sock = ck_tcp_create( 0 );
    if( !m_update_socket.sock )
    {
        fprintf( stderr, "[audicle]: cannot open request socket to send command...\n" );
        return FALSE;
    }

    EM_log( CK_LOG_SYSTEM, "connecting to %s on TCP port %i (update)...", host.c_str(), update_port );
    
    if( !ck_connect( m_update_socket.sock, host.c_str(), update_port ) )
    {
        fprintf( stderr, "[audicle]: cannot open TCP socket on %s:%i...\n", host.c_str(), update_port );
        return FALSE;
    }
    
    ck_send_timeout( m_update_socket.sock, 0, 2000000 );
    
    client_send_update_info( this, user_id );

#ifndef __PLATFORM_WIN32__
    pthread_create( &m_tid, NULL, client_send_loop, this );
    pthread_create( &m_tid2, NULL, client_recv_update_loop, this );
#else
    /*m_tid =*/  CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)client_send_loop, this, 0, 0);
    /*m_tid2 =*/ CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)client_recv_update_loop, this, 0, 0);
#endif

    

    return user_id;
}


//-----------------------------------------------------------------------------
// name: send_request
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL AudicleClient::send_request( AudiclePak * packet )
{ 
    EM_log ( CK_LOG_SYSTEM, "*****request sent...%d ", packet->type );
    return m_request_socket.send( packet );
}


//-----------------------------------------------------------------------------
// name: recv_answer
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL AudicleClient::recv_answer( AudiclePak * packet )
{ 
    return m_request_socket.recv( packet );
}


//-----------------------------------------------------------------------------
// name: GigaSend()
// desc: ...
//-----------------------------------------------------------------------------
GigaSend::GigaSend( )
{
    m_sock = NULL;
    m_red = 1;
    m_buffer_size = 0;
    m_hostname = "127.0.0.1";
    m_port = 8890;
    m_msg.seq_num = 0;
    m_ptr_w = m_writebuf;
    m_ptr_end = NULL;
}
t_CKBOOL GigaSend::good( ) { return m_sock != NULL; }




//-----------------------------------------------------------------------------
// name: ~GigaSend()
// desc: ...
//-----------------------------------------------------------------------------
GigaSend::~GigaSend( )
{
    this->disconnect();
}




//-----------------------------------------------------------------------------
// name: connect()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL GigaSend::connect( const char * hostname, int port )
{
    if( m_sock )
        return FALSE;

    m_sock = ck_udp_create( );
    if( !ck_connect( m_sock, hostname, -port ) )
    {
        cerr << "[chuck](via netout): error: cannot connect to '" << hostname << ":" 
             << port << "'" << endl;
        return FALSE;
    }
    
    m_hostname = hostname;
    m_port = port;
    
    return TRUE;
}




//-----------------------------------------------------------------------------
// name: set_bufsize()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL GigaSend::set_bufsize( t_CKUINT bufsize )
{
    m_buffer_size = bufsize * sizeof(SAMPLE) * Digitalio::m_num_channels_out;
    m_len = sizeof(GigaMsg) + m_buffer_size - sizeof( m_msg.payload );
    m_msg.type = 0;
    m_msg.len = bufsize;
    m_ptr_w = m_writebuf;
    m_ptr_end = m_writebuf + bufsize;

    return TRUE;
}
t_CKUINT GigaSend::get_bufsize() { return m_buffer_size; }




//-----------------------------------------------------------------------------
// name: disconnect()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL GigaSend::disconnect( )
{
    if( !m_sock )
        return FALSE;

    ck_close( m_sock );
    m_sock = NULL;

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: set_redundancy()
// desc: ...
//-----------------------------------------------------------------------------
void GigaSend::set_redundancy( t_CKUINT n )
{
    m_red = n;
}




//-----------------------------------------------------------------------------
// name: get_redundancy()
// desc: ...
//-----------------------------------------------------------------------------
t_CKUINT GigaSend::get_redundancy( )
{
    return m_red;
}




//-----------------------------------------------------------------------------
// name: send()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL GigaSend::send( const t_CKBYTE * buffer )
{
    m_msg.seq_num++;

    memcpy( m_buffer, &m_msg, sizeof( unsigned int ) * 3 );
    memcpy( m_buffer + sizeof( unsigned int ) * 3, buffer, m_buffer_size );

    for( int i = 0; i < m_red; i++ )
        //ck_send( m_sock, (const char * )m_buffer, m_len );
        if( ck_send2( m_sock, (const char *)m_buffer, m_len ) < 0 )
            perror( "[audio server]" );

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: tick_out()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL GigaSend::tick_out( SAMPLE sample )
{
    // send
    if( m_ptr_w >= m_ptr_end )
    {
        this->send( (t_CKBYTE *)m_writebuf );
        m_ptr_w = m_writebuf;
    }

    *m_ptr_w++ = sample;
    
    return TRUE;
}




//-----------------------------------------------------------------------------
// name: GigaRecv()
// desc: ...
//-----------------------------------------------------------------------------
GigaRecv::GigaRecv( )
{
    m_sock = NULL;
    m_buffer_size = 0;
    m_msg.seq_num = 1;
    m_port = 8890;
    m_ptr_r = NULL;
    m_ptr_end = NULL;
}
t_CKBOOL GigaRecv::good( ) { return m_sock != NULL; }




//-----------------------------------------------------------------------------
// name: ~GigaRecv()
// desc: ...
//-----------------------------------------------------------------------------
GigaRecv::~GigaRecv( )
{
    this->disconnect( );
}




//-----------------------------------------------------------------------------
// name: listen()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL GigaRecv::listen( int port )
{
    if( m_sock )
        return FALSE;

    m_sock = ck_udp_create();

    // bind
    if( !ck_bind( m_sock, port ) )
    {
        cerr << "[chuck](via netin): error: cannot bind to port '" << port << "'" << endl;
        return FALSE;
    }
    
    m_port = port;
    m_msg.seq_num = 1;
    
    return TRUE;
}




//-----------------------------------------------------------------------------
// name: reset()
// desc: ...
//-----------------------------------------------------------------------------
void GigaRecv::reset()
{
    goto beach;

beach:
    m_msg.seq_num = 1;
}




//-----------------------------------------------------------------------------
// name: disconnect()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL GigaRecv::disconnect( )
{
    if( !m_sock )
        return FALSE;
        
    ck_close( m_sock );
    m_sock = NULL;
    
    return TRUE;
}




//-----------------------------------------------------------------------------
// name: set_bufsize()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL GigaRecv::set_bufsize( t_CKUINT bufsize )
{
    m_buffer_size = bufsize;
    m_len = sizeof(GigaMsg) + bufsize - sizeof( m_msg.payload );
    m_msg.type = 0;
    m_msg.len = m_len;

    return TRUE;
}
t_CKUINT GigaRecv::get_bufsize() { return m_buffer_size; }




//-----------------------------------------------------------------------------
// name: recv()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL GigaRecv::recv( t_CKBYTE * buffer )
{
    GigaMsg * msg = (GigaMsg *)m_buffer;
    t_CKUINT problem = 0;

    if( !m_sock )
        return FALSE;

    do{
        problem = 0;
        if( ck_recv2( m_sock, (char *)m_buffer, 0x8000 ) < 0 )
        {
            perror( "[audio client]" );
            usleep( 500000 );
            problem = 1;
        }
    }
    while( problem );//|| msg->seq_num < m_msg.seq_num );

    if( msg->seq_num > (m_msg.seq_num + 1) )
        cerr << "[chuck](via netin): dropped packet, expect: " << m_msg.seq_num + 1
             << " got: " << msg->seq_num << endl;

    m_msg.seq_num = msg->seq_num;
    m_msg.len = msg->len;
    m_ptr_end = m_readbuf + msg->len;

    memcpy( buffer, m_buffer + sizeof( unsigned int ) * 3, m_msg.len * sizeof(SAMPLE) * Digitalio::m_num_channels_out );

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: expire()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL GigaRecv::expire()
{
    m_msg.seq_num++;
    return true;
}




//-----------------------------------------------------------------------------
// name: tick_in()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL GigaRecv::tick_in( SAMPLE * sample )
{
    if( m_ptr_r >= m_ptr_end )
    {
        this->recv( (t_CKBYTE *)m_readbuf );
        m_ptr_r = m_readbuf;
    }
    
    *sample = *m_ptr_r++;
    
    return TRUE;
}




//-----------------------------------------------------------------------------
// name: AudicleAudioSend()
// desc: ...
//-----------------------------------------------------------------------------
AudicleAudioSend::AudicleAudioSend()
{
    m_todo = 0;
    m_max = 0;
}




//-----------------------------------------------------------------------------
// name: ~AudicleAudioSend()
// desc: ...
//-----------------------------------------------------------------------------
AudicleAudioSend::~AudicleAudioSend()
{
}




//-----------------------------------------------------------------------------
// name: initialize()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL AudicleAudioSend::initialize( t_CKUINT bufsize, t_CKUINT depth )
{
    // remember
    m_bufsize = bufsize;
    // max
    m_max = depth;
    // todo
    m_todo = m_max;

    return FALSE;
}




//-----------------------------------------------------------------------------
// name: add()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL AudicleAudioSend::add( const char * host, int port )
{
    // make new network
    GigaSend * send = new GigaSend;
    send->set_bufsize( m_bufsize );
    if( !send->connect( host, port ) )
        return FALSE;

    // lock
    m_mutex.acquire();
    // make sure empty
    if( m_hosts.find( host ) != m_hosts.end() )
    {
        // error
        EM_error3( "(via audiosend): duplicate host ignored..." );
        // unlock
        m_mutex.release();
        return FALSE;
    }
    
    // log
    EM_log( CK_LOG_SYSTEM, "adding '%s' to audio send map...", host );

    // add host to list
    m_hosts[host] = send;
    // unlock
    m_mutex.release();

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: remove()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL AudicleAudioSend::remove( const char * host )
{
    // lock
    m_mutex.acquire();
    // see if present
    if( m_hosts.find( host ) == m_hosts.end() )
    {
        // error
        EM_error3( "(via audiosend): cannot remove host '%s'...", host );
        // unlock
        m_mutex.release();
        return FALSE;
    }

    // log
    EM_log( CK_LOG_SYSTEM, "removing '%s' from audio send map...", 
        (*m_hosts.find( host )).first.c_str() );

    // erase
    m_hosts.erase( m_hosts.find( host ) );
    // unlock
    m_mutex.release();

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: audio_send_loop()
// desc: ...
//-----------------------------------------------------------------------------
static void * audio_send_loop( void * data )
{
    AudicleAudioSend * send = (AudicleAudioSend *)data;
    Chuck_VM * vm = CoAudicle::instance()->vm();
    BBQ * bbq = CoAudicle::instance()->vm()->bbq();
    t_CKUINT todo = send->m_todo;
    t_CKUINT howmuch = todo;
    vector<string> names;
    vector<GigaSend *> hosts;

    // log
    EM_log( CK_LOG_SYSTEM, "starting audio send loop..." );
    // priority boost
    if( Chuck_VM::our_priority != 0x7fffffff )
        Chuck_VM::set_priority( Chuck_VM::our_priority, NULL );

    // woohoo
    while( vm->m_running )
    {
        // copy 
        todo = send->m_todo;
        howmuch = todo;
        names.clear();
        hosts.clear();

        // left to do?
        if( send->m_todo )
        {
            // lock
            send->m_mutex.acquire();
            // get list of subscribers
            map<string, GigaSend *>::iterator iter;
            for( iter = send->m_hosts.begin(); 
                 iter != send->m_hosts.end();
                 iter++ )
            {
                // add
                names.push_back( (*iter).first );
                hosts.push_back( (*iter).second );
            }
            // unlock
            send->m_mutex.release();

            // compute the next buffer
            while( todo )
            {
                vm->run( send->m_bufsize );

                // to all clients
                for( int i = 0; i < hosts.size(); i++ )
                {
                    // log
                    EM_log( CK_LOG_FINEST, "sending audio to %s", names[i].c_str() );
                    hosts[i]->send( (unsigned char *)Digitalio::m_buffer_out );
                }

                todo--;
            }

            // decrement
            send->m_todo -= howmuch;
        }
        else
        {
            // wait a bit
            usleep( 5000 );
        }
    }

    return NULL;
}




//-----------------------------------------------------------------------------
// name: start()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL AudicleAudioSend::start()
{
    THREAD_HANDLE id = 0;

    // start receiving thread
    #ifndef __PLATFORM_WIN32__
        pthread_create( &id, NULL, audio_send_loop, this );
    #else
        /*id =*/ CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)audio_send_loop,
             this, 0, 0);
    #endif

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: AudicleAudioRecv()
// desc: ...
//-----------------------------------------------------------------------------
AudicleAudioRecv::AudicleAudioRecv()
{
    m_bufsize = 0;
    m_totalsize = 0;
    m_more = NULL;
    m_server_id = 0;
}




//-----------------------------------------------------------------------------
// name: ~AudicleAudioRecv()
// desc: ...
//-----------------------------------------------------------------------------
AudicleAudioRecv::~AudicleAudioRecv()
{
    this->stop();
}




//-----------------------------------------------------------------------------
// name: net_audio()
// desc: callback
//-----------------------------------------------------------------------------
int net_audio( char * buffer, int buffer_size, void * user_data )
{
    DWORD__ len = buffer_size * sizeof(SAMPLE) * Digitalio::m_num_channels_out;
    AudicleAudioRecv * recv = (AudicleAudioRecv *)user_data;
    SAMPLE * play = NULL;

    // EM_log( CK_LOG_FINEST, "net_audio" );

    // TODO: abuse priority

    // TODO: copy in data?

    // TODO: check xrun?

    // get next buffer
    recv->m_mutex.acquire();
    if( recv->m_playbuffer.size() > 0 )
    {
        play = recv->m_playbuffer.front();
        recv->m_playbuffer.pop();
    }

    // copy local buffer to be rendered
    if( play )
    {
        memcpy( buffer, play, len );
        // copy to extern
        if( Digitalio::m_extern_out ) memcpy( Digitalio::m_extern_out, buffer, len );
        // put in done buffer
        recv->m_donebuffer.push( play );
    }
    // set all elements of local buffer to silence
    else memset( buffer, 0, len );
    recv->m_mutex.release();

    // notify
    CoAudicle::instance()->audio_server.m_todo++;
    if( CoAudicle::instance()->audio_server.m_todo > CoAudicle::instance()->audio_server.m_max )
        CoAudicle::instance()->audio_server.m_todo = CoAudicle::instance()->audio_server.m_max;
    //if( recv->m_more )
    //    recv->m_more( recv->m_donebuffer.size() );

    return 0;
}




//-----------------------------------------------------------------------------
// name: initialize()
// desc: initialize the sound device
//-----------------------------------------------------------------------------
t_CKBOOL AudicleAudioRecv::initialize( t_CKUINT bufsize, t_CKUINT totalsize, 
                                       t_CKINT port, notify_func more )
{
    // initialize sound card
    if( !CoAudicle::instance()->vm()->bbq()->initialize( 2, 2, 44100, 16, bufsize,
        8, 0, 0, FALSE, CoAudicle::instance()->vm(), TRUE, (void *)net_audio, this ) )
        return FALSE;

    // log
    EM_log( CK_LOG_SYSTEM, "starting audio listener on UDP port %d...", port );

    // giga
    if( !m_recv.listen( port ) )
        return FALSE;

    // set buffer size
    m_recv.set_bufsize( bufsize * sizeof(SAMPLE) * Digitalio::m_num_channels_out );

    // set the get more samples function
    m_more = more;

    // allocate the buffers
    SAMPLE * buffer;
    for( t_CKUINT i = 0; i < totalsize; i++ )
    {
        // make buffer
        buffer = new SAMPLE[bufsize * Digitalio::m_num_channels_out];
        // put in queue
        m_donebuffer.push( buffer );
    }

    // set the size
    m_totalsize = totalsize;

    // notify
    if( more )
        more( totalsize );

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: audio_recv_loop()
// desc: ...
//-----------------------------------------------------------------------------
static void * audio_recv_loop( void * no )
{
    AudicleAudioRecv * recv = (AudicleAudioRecv *)no;
    SAMPLE * buffer = NULL;

    EM_log( CK_LOG_SYSTEM, "starting audio recv loop..." );

    // woohoo
    while( CoAudicle::instance()->vm()->m_running )
    {
        buffer = NULL;

        // lock
        recv->m_mutex.acquire();
        // TODO: not crash
        if( !recv->m_donebuffer.empty() )
        {
            // get a free buffer
            buffer = recv->m_donebuffer.front();
            recv->m_donebuffer.pop();
        }
        // unlock
        recv->m_mutex.release();

        // recv
        if( buffer )
        {
            // guess what this does...
            recv->m_recv.recv( (unsigned char *)buffer );
            // log
            EM_log( CK_LOG_FINEST, "audio buffer received..." );
            // lock
            recv->m_mutex.acquire();
            // put on play list
            recv->m_playbuffer.push( buffer );
            // unlock
            recv->m_mutex.release();
        }
        else
            // wait
            usleep( 5000 );
    }
    
    return NULL;
}




//-----------------------------------------------------------------------------
// name: start()
// desc: start the process, but what process?
//-----------------------------------------------------------------------------
t_CKBOOL AudicleAudioRecv::start()
{
    THREAD_HANDLE id = 0;

    // start audio
    CoAudicle::instance()->vm()->bbq()->digi_in()->initialize();
    CoAudicle::instance()->vm()->bbq()->digi_out()->initialize();
    CoAudicle::instance()->vm()->bbq()->digi_in()->start();
    CoAudicle::instance()->vm()->bbq()->digi_out()->start();

    // start receiving thread
    #ifndef __PLATFORM_WIN32__
        pthread_create( &id, NULL, audio_recv_loop, this );
    #else
        /*id =*/ CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)audio_recv_loop,
             this, 0, 0);
    #endif

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: expect_host()
// desc: expect a potential different host to receive audio from?
//-----------------------------------------------------------------------------
void AudicleAudioRecv::expect_host( t_CKUINT server_id )
{
}




//-----------------------------------------------------------------------------
// name: stop()
// desc: ...the audio
//-----------------------------------------------------------------------------
t_CKBOOL AudicleAudioRecv::stop()
{
    return TRUE;
}








//-----------------------------------------------------------------------------
// name: CUTS
// desc: stuff that got commented out and maybe could be deleted?
//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------
// name: close()
// desc: ...
//-----------------------------------------------------------------------------
//void AudicleServer::close()
//{
//}


//-----------------------------------------------------------------------------
// name: send()
// desc: ...
//-----------------------------------------------------------------------------
/*t_CKBOOL AudicleClient::send( AudiclePak * packet )
{
    // host to network
    packet->hton();
    // send header
    ck_send( m_socket, (char *)packet, sizeof(AudiclePak) - sizeof(void *) - sizeof(t_CKUINT) );
    // send payload
    ck_send( m_socket, (char *)packet->data, packet->length );
    // back
    packet->ntoh();

    return TRUE;
}*/


//-----------------------------------------------------------------------------
// name: queue()
// desc: add packet to send queue
//-----------------------------------------------------------------------------
/*t_CKBOOL AudicleClient::queue( AudiclePak * packet )
{
    // TODO: maybe take this out
    AudiclePak * copy  = new AudiclePak;
    *copy = *packet;
    copy->data = NULL;
    copy->alloc( packet->capacity );
    memcpy( copy->data, packet->data, copy->length );

    m_send_queue.put( &copy, 1 ); 
    return TRUE;
}*/


//-----------------------------------------------------------------------------
// name: dequeue()
// desc: because the send queue is not public
//-----------------------------------------------------------------------------
/*t_CKBOOL AudicleClient::dequeue( AudiclePak ** packet )
{
    return m_send_queue.get( packet, 1 );
}*/

//-----------------------------------------------------------------------------
// name: server_loop_for_each_client()
// desc: ...
//-----------------------------------------------------------------------------
/*static void * server_loop_for_each_client( void * no )
{
    ServerData * gh = (ServerData *)no;
    AudicleServer * server = gh->server;
    ck_socket client = gh->client;
    AudiclePak packet;
    int size;
    int n;

    // delete good hack
    SAFE_DELETE( gh );

#ifndef __PLATFORM_WIN32__
    // catch SIGPIPE
    // signal( SIGPIPE, signal_pipe );
#endif

    EM_log( CK_LOG_SYSTEM, "audicle server loop for each client starting..." );

    // set time out
    ck_recv_timeout( client, 0, 5000000 );
    // the size of header
    size = sizeof(AudiclePak) - sizeof(void *) - sizeof(t_CKUINT);

    // woohoo
    while( true )
    {
        // get the packet header
        n = ck_recv( client, (char *)&packet, size );
        // network to host
        packet.ntoh();

        // log
        EM_log( CK_LOG_FINER, "audicle receiving packet header, got %d bytes...", n );
        EM_pushlog();
        EM_log( CK_LOG_FINER, "packet: header %x", packet.header );
        EM_log( CK_LOG_FINER, "packet: type %d", packet.type );
        EM_log( CK_LOG_FINER, "packet: user %d", packet.user );
        EM_log( CK_LOG_FINER, "packet: length %d", packet.length );
        EM_poplog();

        // check bytes received
        if( n != size )
        {
            EM_log( CK_LOG_SYSTEM, "incomplete packet in audicle..." );
            usleep( 40000 );
            break;
        }
        // check header
        if( packet.header != AUDICLE_HEADER )
        {
            EM_log( CK_LOG_SYSTEM, "header mismatch - possible endian lunacy in audicle..." );
            break;
        }

        // allocate rest of packet
        if( packet.length > packet.capacity )
            packet.alloc( packet.length );
        
        // get the rest of the packet
        n = ck_recv( client, (char *)(packet.data), packet.length );
        
        // handle the packet
        if( !server_handle_packet( &packet ) )
        {
            EM_log( CK_LOG_SYSTEM, "error handling packet in audicle..." );
            break;
        }
    }

    ck_close( client );
    
    return NULL;
}*/


