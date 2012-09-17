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
// name: audicle_net.h
// desc: interface for network and audio
//
// authors: Ge Wang (gewang@cs.princeton.edu)
//          Perry R. Cook (prc@cs.princeton.edu)
//          Ananya Misra (amisra@cs.princeton.edu)
//          Philip Davidson (philipd@cs.princeton.edu)
// date: Summer 2005
//-----------------------------------------------------------------------------
#ifndef __AUDICLE_NET_H__
#define __AUDICLE_NET_H__

#include "audicle_def.h"
#include "util_network.h"
#include "util_thread.h"
#include "util_buffers.h"



// defines
#define AUDICLE_HEADER      0xfeedf00d

// forward reference
struct SockPack;




//-----------------------------------------------------------------------------
// name: struct AudiclePak
// desc: ...
//-----------------------------------------------------------------------------
struct AudiclePak
{
    t_CKUINT header;
    t_CKUINT type;
    t_CKTIME time;
    t_CKUINT user;
    char server[32];
    t_CKUINT length;

    t_CKUINT capacity;
    char * data;

    SockPack * sender;

    AudiclePak()
    {
       sender = NULL;

       header = AUDICLE_HEADER;
       type = 0;
       user = 0;
       server[0] = '\0';
       length = 0;
       capacity = 0;
       data = NULL;
    }

    ~AudiclePak()
    {
       capacity = 0;
       SAFE_DELETE_ARRAY( data );
    }

    void alloc( t_CKUINT cap )
    {
       SAFE_DELETE_ARRAY( data );
       data = new char[cap];
       assert( data );
       capacity = cap;
    }

    void ntoh( )
    {
        header = ntohl( header );
        type = ntohl( type );
        user = ntohl( user );
        length = ntohl( length );
    }

    void hton( )
    {
        header = htonl( header );
        type = htonl( type );
        user = htonl( user );
        length = htonl( length );
    }
};


//temp
struct SockPack
{
    ck_socket sock;
    t_CKBOOL send( AudiclePak * pack );
    t_CKINT recv( AudiclePak * pack );
    
    XMutex mutex;

    SockPack() { sock = NULL; }
};


//-----------------------------------------------------------------------------
// name: struct AudicleCS
// desc: client / server superclass
//-----------------------------------------------------------------------------
struct AudicleCS
{
public: 
    AudicleCS();
    ~AudicleCS();

public:
    virtual t_CKBOOL queue( AudiclePak * packet );
    virtual t_CKBOOL dequeue( AudiclePak ** packet );
    //virtual t_CKBOOL send( AudiclePak * packet ) = 0; 
    virtual void close();

protected:
    THREAD_HANDLE m_tid;
    CBufferSimple m_send_queue;
};


//-----------------------------------------------------------------------------
// name: struct AudicleServer
// desc: ...
//-----------------------------------------------------------------------------
struct AudicleServer : public AudicleCS
{
public:
    AudicleServer();
    ~AudicleServer();

public:
    t_CKBOOL start( t_CKINT request_port, t_CKINT update_port );
    t_CKBOOL broadcast( AudiclePak * packet );
    t_CKBOOL send( AudiclePak * packet, int client_id );
    
public:
    SockPack m_request_socket;
    SockPack m_update_socket;
    std::vector<SockPack> m_clients;
};


//-----------------------------------------------------------------------------
// name: struct AudicleClient
// desc: ...
//-----------------------------------------------------------------------------
class ClientSessionImp; //keep a pointer to the session that launched this client.

struct AudicleClient : public AudicleCS
{
public:
    AudicleClient();
    ~AudicleClient();

public:
    t_CKUINT connect( const std::string & host, t_CKINT request_port, t_CKINT update_port );
    t_CKBOOL send_request( AudiclePak * packet );
    t_CKBOOL recv_answer( AudiclePak * packet );

public:
    SockPack m_request_socket;
    SockPack m_update_socket;
    ClientSessionImp * m_session;

protected:
    THREAD_HANDLE m_tid2;
};




//-----------------------------------------------------------------------------
// name: struct GigaMsg
// desc: ...
//-----------------------------------------------------------------------------
struct GigaMsg
{
    unsigned int type;
    unsigned int len;
    unsigned int seq_num;
  
    unsigned char * payload;

    // constructor
    GigaMsg() {
        type = len = seq_num = 0;
        payload = NULL;
    }

    // init
    void init( unsigned int _type, unsigned int _len )
    {
        type = _type;
        len = _len;
        payload = new unsigned char[len];
    }

    // destructor
    ~GigaMsg( )
    {
        if( payload )
        {
            delete [] payload;
            payload = NULL;
        }
        
        type = 0;
        len = 0;
        seq_num = 0;
    }
};




//-----------------------------------------------------------------------------
// name: class GigaSend
// desc: ...
//-----------------------------------------------------------------------------
class GigaSend
{
public:
    GigaSend( );
    ~GigaSend( );

    t_CKBOOL connect( const char * hostname, int port );
    t_CKBOOL disconnect( );
    t_CKBOOL send( const t_CKBYTE * buffer );
    t_CKBOOL set_bufsize( t_CKUINT buffer_size );
    t_CKUINT get_bufsize( );
    t_CKBOOL good( );

    t_CKBOOL tick_out( SAMPLE sample );
    t_CKBOOL tick_out( SAMPLE l, SAMPLE r );
    t_CKBOOL tick_out( SAMPLE * samples, t_CKINT n );

    void set_redundancy( t_CKUINT n );
    t_CKUINT get_redundancy( );
    
    // data
    std::string m_hostname;
    int m_port;
    std::string m_name;

protected:
    ck_socket m_sock;
    t_CKUINT m_red;
    t_CKUINT m_buffer_size;
    GigaMsg m_msg;
    t_CKUINT m_len;
    t_CKBYTE m_buffer[0x8000];

    SAMPLE m_writebuf[0x8000];
    SAMPLE * m_ptr_w;
    SAMPLE * m_ptr_end;
};




//-----------------------------------------------------------------------------
// name: class GigaRecv
// desc: ...
//-----------------------------------------------------------------------------
class GigaRecv
{
public:
    GigaRecv( );
    ~GigaRecv( );

    t_CKBOOL listen( int port );
    t_CKBOOL disconnect( );
    t_CKBOOL recv( t_CKBYTE * buffer );
    t_CKBOOL expire();
    t_CKBOOL set_bufsize( t_CKUINT size );
    t_CKUINT get_bufsize( );
    t_CKBOOL good( );
    void reset( );
    
    t_CKBOOL tick_in( SAMPLE * sample );
    t_CKBOOL tick_in( SAMPLE * l, SAMPLE * r );
    t_CKBOOL tick_in( SAMPLE * samples, t_CKINT n );
    
    // data
    int m_port;
    std::string m_name;

protected:
    ck_socket m_sock;
    t_CKUINT m_buffer_size;
    GigaMsg m_msg;
    t_CKUINT m_len;
    t_CKBYTE m_buffer[0x8000];
    
    SAMPLE m_readbuf[0x8000];
    SAMPLE * m_ptr_r;
    SAMPLE * m_ptr_end;
};




//-----------------------------------------------------------------------------
// name: class AudicleAudioSend
// desc: ...
//-----------------------------------------------------------------------------
class AudicleAudioSend
{
public:
    // constructor
    AudicleAudioSend();
    // destructor
    ~AudicleAudioSend();

public:
    // vm init
    t_CKBOOL initialize( t_CKUINT bufsize, t_CKUINT depth );
    // add host to connect to
    t_CKBOOL add( const char * host, int port );
    // remove the host
    t_CKBOOL remove( const char * host );
    // start
    t_CKBOOL start();

public:
    std::map<std::string, GigaSend *> m_hosts;
    XMutex m_mutex;
    t_CKUINT m_todo;
    t_CKUINT m_bufsize;
    t_CKUINT m_max;
};




// notify
typedef void (* notify_func)( t_CKUINT num );

//-----------------------------------------------------------------------------
// name: class AudicleAudioRecv
// desc: ...
//-----------------------------------------------------------------------------
class AudicleAudioRecv
{
public:
    // constructor
    AudicleAudioRecv();
    // destructor
    ~AudicleAudioRecv();

public:
    // initialize the sound device
    t_CKBOOL initialize( t_CKUINT bufsize, t_CKUINT totalsize, t_CKINT port, notify_func more );
    // start the process
    t_CKBOOL start();
    // host
    void expect_host( t_CKUINT server_id );
    // stop the audio
    t_CKBOOL stop();

public:
    // queue of buffers to be played
    std::queue<SAMPLE *> m_playbuffer;
    // queue of buffers to be filled
    std::queue<SAMPLE *> m_donebuffer;
    // audio buffer size
    t_CKUINT m_bufsize;
    // size of queues
    t_CKUINT m_totalsize;
    // function to call
    notify_func m_more;
    // host to expect
    t_CKUINT m_server_id;

    // giga
    GigaRecv m_recv;
    // mutex
    XMutex m_mutex;
};




#endif
