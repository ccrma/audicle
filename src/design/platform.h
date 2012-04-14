
#ifndef _PLATFORM_H_INCLUDED_
#define _PLATFORM_H_INCLUDED_

//system implementations
#include "sys.h"
#include "dirscan.h"
#include "thread.h"

#define PICK_TOLERANCE 2
#define PICK_BUFFER_SIZE 256

extern GLuint RenderMode;
extern uint * PickBuffer;

//gl string shortcuts
void drawString ( const char * str );
void drawString_mono ( const char * str ) ;
double drawString_length ( const char* str ) ;
double drawString_length_kerned ( const char* str ) ;
void drawString_centered ( const char* str ) ;
void scaleFont ( double h, double aspect = 0.8 );

class PlatformFactory { 
protected:
   static PlatformFactory *_instance;
public:

   PlatformFactory() {}
   static PlatformFactory *Instance();
   DirScannerImp*  DirScan();

};

double GetTime(void);

int fgetline(char**buf, int *size, FILE *fp);

double interp ( int a,     int b,      double w );
double interp ( double a,  double b,   double w );


enum fmode { FMODE_CLOSED, FMODE_FREAD, FMODE_FWRITE, FMODE_NREAD, FMODE_NWRITE, FMODE_NUM };
#define CBUFSIZ 2048

class fmBuffer { 

protected:
   fmode    _mode;
   string   _buffer;
   char     _line[CBUFSIZ];
   int      _pos;
   bool     _ready;
public:

   fmBuffer() : _mode(FMODE_CLOSED) 
   {  
      _buffer.erase(); 
      _buffer.reserve(CBUFSIZ); 
      _line[0] = '\0';
   }

   fmBuffer( fmode m ) : _mode(m) 
   {  
      _buffer.erase(); 
      _buffer.reserve(CBUFSIZ);
      _line[0] = '\0';
   }

   void  flush();

   void  smode( fmode m )  { _mode = m; }  
   void  writefile(char* w);
   int   readfile(char* r);

   void  netrecv(char * address, int port); //TCP IP
   void  netsend(char * address, int port); //TCP IP

   void  close();
   
   int      getline(char ** sc, int * sz);
   int      getline(string &s);
   char*    in();
   char*    data()  { flush(); return (char*)_buffer.c_str(); }

};

int tokenize(char *buf, char * tok, char ***args, int * size );

enum udp_stat { UDP_NOINIT, UDP_UNBOUND, UDP_BOUND, UDP_READY, UDP_ERROR, UDP_NUM };
enum tcp_stat { TCP_NOINIT, TCP_UNBOUND, TCP_BOUND, TCP_LISTENING, TCP_ERROR } ;
enum tcp_role { TCP_CLIENT, TCP_SERVER };

#define TCPMESGNUM 32
#define TCPBUFSIZE 1024
#define TCPMAXPACKETS 64

class TCP_Connection {

protected:

#if defined(__PLATFORM_WIN32__)
   WSADATA        _wsda;        // Structure to store info
#endif

   tcp_role       _role;
   tcp_stat       _status;
   SOCKET         _local;
   SOCKADDR_IN    _local_addr;
   SOCKET         _remote;
   SOCKADDR_IN    _remote_addr;
   int            _readpos;
   int            _writepos;
   char           _inbuf[TCPBUFSIZE+1];
   char *         _messages[TCPMESGNUM];
   int            _mesgsize[TCPMESGNUM];
   void           _connect_mesg();
   void           _message_cb();
public:

   void  init();

   virtual int    start_server ( int port );
   virtual void   server_loop();
   virtual int    start_client ( char * host, int port );
   virtual void   client_loop();
   
   //   void  close();
   bool  hasData();
   int   getData  (char ** c, int bufsize);
   int   sendData (char *c,   int size );
   //callback for listeners....
};

class UDP_Receiver { 

protected:

#if defined(__PLATFORM_WIN32__)
   WSADATA        _wsda;        // Structure to store info
#endif
   
   SOCKET         _s;
   SOCKADDR_IN    _addr;
   SOCKADDR_IN    _remote_addr;
   int            _mesgLen;     
   SOCKLEN_T      _iRemoteAddrLen;            // Contains the length of remote_addr, passed to recvfrom
   char           _szAddress[64];
   int            _port;
   udp_stat       _status;

public:

   UDP_Receiver() { 
      _port = 6448; 
      _status = UDP_NOINIT; 
   }
   ~UDP_Receiver() { }
   void init();

   void bind_to_port(int port);
   int recv_next(char *buffer, int size);
   
   void close_sock();
   
};


class UDP_Transmitter { 
protected:
#if defined(__PLATFORM_WIN32__)
   WSADATA        _wsda;        // Structure to store info
#endif
   SOCKET         _s;
   SOCKADDR_IN    _addr;
   SOCKADDR_IN    _remote_addr;
   int            _mesgLen;     
   SOCKLEN_T      _iRemoteAddrLen;            // Contains the length of remte_addr, passed to recvfrom
   char           _szAddress[64];
   int            _host;
   int            _port;
   udp_stat       _status;

public:

   UDP_Transmitter() { 
      _port = 6449; 
      _status = UDP_NOINIT; 
   }
   ~UDP_Transmitter() {}
   void  init();

   void  set_host(char * hostaddress, int port);
   int   send(char *buffer, int size);
   udp_stat status(void) { return _status; } 
   void  close_sock();
   
};


#endif // _PLATFORM_H_INCLUDED_
