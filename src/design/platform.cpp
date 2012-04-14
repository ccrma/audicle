#include "platform.h"
#include "dirscan.h"


bool cursorOn = true;

#define TABSTOP 4

extern GLuint RenderMode = GL_RENDER;
extern uint * PickBuffer = (uint *) malloc ( PICK_BUFFER_SIZE * sizeof(uint) );

void drawString ( const char * str ) { 
  int n = strlen(str);
  for ( int i = 0; i < n ; i++ ) { 
    glutStrokeCharacter(GLUT_STROKE_ROMAN, str[i] );
  }
}

void drawString_mono ( const char * str ) { 
    int n = strlen(str);
    for ( int i =0; i < n ; i++ ) {
        glutStrokeCharacter(GLUT_STROKE_MONO_ROMAN, str[i] );
    } 
} 

double drawString_length ( const char* str ) { 
   return glutStrokeLength( GLUT_STROKE_MONO_ROMAN, (const unsigned char* ) str );
}

double drawString_length_kerned ( const char* str ) { 
   return glutStrokeLength( GLUT_STROKE_ROMAN, (const unsigned char* ) str );
}


void drawString_centered ( const char* str ) { 
    int x = glutStrokeLength(GLUT_STROKE_ROMAN, (const unsigned char*) str);
    glTranslated( -x*0.5, 0, 0);
    drawString(str);
}

void scaleFont(double h, double aspect) { 
   glScaled( h * .01 * aspect, h * 0.01, 1 );
}


double GetTime(void){
#ifdef __PLATFORM_WIN32__
	struct _timeb t;
	_ftime(&t);
	return t.time+t.millitm/1000.0;
#else
	struct timeval t;
	gettimeofday(&t,NULL);
	return t.tv_sec+(double)t.tv_usec/1000000;
#endif
}


//struct multiFile { 
//	string path;
//	struct multiFile * next;
//	multiFile () { path=""; next=NULL; }
//}
//bool platformOpenSingleDialog ( );
//bool platformOpenMultiDialog ( );
//bool platformSaveDialog ( );

PlatformFactory* PlatformFactory::_instance = NULL;

PlatformFactory*
PlatformFactory::Instance() { 

   if ( !_instance ) 
      _instance = new PlatformFactory();

   return _instance;

}

DirScannerImp*
PlatformFactory::DirScan() { 

#ifdef __PLATFORM_WIN32__
    fprintf(stderr, "windows dir scanner\n");
    return new winDirScanner();
#else
#ifdef __PLATFORM_MACOSX__
    fprintf(stderr, "mac dir scanner\n");
    return new MacOSXDirScanner();
#else
    fprintf(stderr, "*nix dir scanner\n");
    return new UnixDirScanner();
#endif
#endif

}

double interp (int a, int b, double w ) { 
   return (double)a*(1.0-w) + (double)b*w;
}

double interp ( double a, double b, double w ) { 
   return a*(1.0-w) + b*w;
}

int fgetline(char**buf, int *size, FILE *fp)
{
  int len;
  int c;

  if (fp == NULL)
    return -1;

  if (*buf == NULL) {
    if (*size == 0) 
      *size = BUFSIZ;
    
    if ((*buf = (char*)malloc(*size)) == NULL)
      return -1;
  }

  if (feof(fp))
    return -1;
    
  len = 0;
  while ((c = getc(fp)) != EOF && c != '\n') {
    if (len+1 >= *size) {
      if ((*buf = (char*)realloc(*buf, *size += BUFSIZ)) == NULL) return -1;
    }
    (*buf)[len++] = c;
  }

  if (len == 0 && c == EOF)
    return -1;
    
  (*buf)[len] = '\0';
    
  return len;

} /* fgetline */


void
fmBuffer::netrecv(char * addr, int port ) { 
   //empty
   fprintf(stderr, "fmBuffer::netrecv : no imp!\n");
}

void
fmBuffer::netsend(char * addr, int port ) { 
   //empty
   fprintf(stderr, "fmBuffer::netsend : no imp!\n");
}

int
fmBuffer::readfile(char* m) { 
   if ( _mode != FMODE_FREAD) return 1;
   char * s = new char[256];
   int sz = 256;
   _buffer.erase();
   FILE * f = fopen(m , "r");
   string nl = "\n";
   if ( f ) { 
      while ( fgetline(&s, &sz, f) != EOF ) {   
         _buffer += s;
         _buffer += nl;
      }
      _ready = true;
      _pos = string::npos;
      fclose(f);
      return 0;
   }
   else return 1;
}


void
fmBuffer::writefile(char*m) { 
   
   if ( _mode != FMODE_FWRITE ) return;
   flush();
   FILE * f = fopen(m , "w");
   if ( f ) { 
      fprintf(f, "%s", _buffer.c_str() );
      fclose(f);
   }
}

int
fmBuffer::getline(string &s) { 

   if ( !_ready ) return EOF;
   if ( _pos == (int)string::npos  ) _pos = 0; 
   int end = _buffer.find("\n", _pos);

   if ( end == (int)string::npos) { 
      _ready = false;
      s = _buffer.substr(_pos);
      _pos = string::npos;
   }
   else { 
      int len = end - _pos;
      s = _buffer.substr(_pos, len);
      _pos = end+1;
   }
   return s.length();

}



int
fmBuffer::getline(char** sc, int * sz) { 

   if ( !_ready ) return EOF;
   
   if ( _pos == (int)string::npos  ) _pos = 0; 
   
   int end = _buffer.find("\n", _pos);

   int len = ( end == (int)string::npos ) ? _buffer.length() - _pos : end - _pos;
   
   if ( len > *sz ) { 
      if ( ( *sc = (char*)realloc( *sc, len+1 )) == NULL ) { 
         fprintf(stderr,"reallocfailed\n");
         return EOF;
      }
   }

   if ( end == (int)string::npos) { 
      _ready = false;
      strcpy(*sc, _buffer.substr(_pos).c_str() );
      _pos = string::npos;
   }
   else { 
      strcpy(*sc, _buffer.substr(_pos, len).c_str());
      (*sc)[len] = '\0';
      _pos = end+1;
   }

   return len;

}

void
fmBuffer::flush() { 
   _buffer += _line;
   _line[0] = '\0';
}

char*
fmBuffer::in() { 
   flush(); // line into buffer   
   return (char*) _line; 
}


int
tokenize(char * buf, char *tok, char***args, int *size) { 
   //buf must persist after this function is called!
   int i =0;
   char * t = strtok(buf, tok);
   while ( t != NULL ) { 
      while ( i+1 > *size ) { 
         *size *= 2; 
         *args = (char**) realloc(*args, *size * sizeof(char*) ); 
      }
      (*args)[i] = t;
      i++;
      t = strtok(NULL, tok);
   }
   return i;  
}

void 
TCP_Connection::init() {

#if defined(__PLATFORM_WIN32__)
   WSAStartup(MAKEWORD(1,1), &_wsda);
#endif

   printf("Creating socket...");
   _local = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

   if ( _local <= SOCKET_ERROR ) { 
#if defined(__PLATFORM_WIN32__)
      fprintf(stderr, "socket init failed with %d\n", WSAGetLastError());
#else
      fprintf(stderr, "socket init failed with %d\n", _local);
#endif
      _status = TCP_ERROR;
      return;
   }

   _status = TCP_UNBOUND;
   
}

int  
TCP_Connection::start_server(int port) { 
   _local_addr.sin_family=AF_INET; //Address family
   _local_addr.sin_addr.s_addr=INADDR_ANY; //Wild card IP address
   _local_addr.sin_port=htons((u_short)port); //port to use
   
   //bind to socket
   if ( bind ( _local, (sockaddr*)&_local_addr, sizeof(_local_addr) ) != 0 ) { 
      fprintf(stderr, "TCP_Connection::start_server ) cannot bind socket!\n" );
      return 0;
   }
   
   if ( listen( _local, 10 ) != 0 ) { 
      fprintf(stderr, "TCP_Connection::start_server ) listen failed!\n" );
      return 0;
   }
   
   server_loop();
   return 1;
}

void
TCP_Connection::server_loop() { 

    SOCKLEN_T flen = sizeof(_remote_addr);
    SOCKLEN_T rlen = 0;
    while ( true ) { 

      _remote= accept(_local, (sockaddr*)&_remote_addr, &flen);

      fprintf(stderr, "connection from %s\n", inet_ntoa(_remote_addr.sin_addr) ); 
      
      //hello
      send(_remote, "veldt:hello", 12, 0 );
      
      //receive request
      while ( ( rlen = recv(_remote, _inbuf, TCPBUFSIZE, 0 )) != 0 ) { 
         _inbuf[rlen] = '\0';
         fprintf(stderr, "received : %s\n", _inbuf );
      }
      //fulfill request
      send(_remote, "PaylOaD!", 9, 0);

#if defined(__PLATFORM_WIN32__)
      closesocket(_remote);
      WSACleanup();
#else
      close ( _remote );
#endif


    }
}

int
TCP_Connection::start_client(char * hostaddr, int port ) { 

   printf("gathering server info...");

   _remote_addr.sin_addr.s_addr = inet_addr(hostaddr);

   if(_remote_addr.sin_addr.s_addr == INADDR_NONE) // The address wasn't in numeric
   {
      struct hostent *host = gethostbyname(hostaddr); // Get the IP address of the server
      if(host == NULL)
      {
         printf("TCP::error\nUnknown host: %s\n", hostaddr);
         return 1;
      }
      memcpy(&_remote_addr.sin_addr, host->h_addr_list[0], host->h_length);
      printf("OK\n");
   } 
   else printf("..set\n");   

   _remote_addr.sin_family = AF_INET;
   _remote_addr.sin_port = htons(port);

   client_loop();

   return 0;
}

void
TCP_Connection::client_loop() { 
   if ( connect(_local, (sockaddr*)&_remote_addr, sizeof(_remote_addr)) != 0 ) { 
      return;
   }

   while ( true ) { 

   }


}



void
UDP_Receiver::init() { 

   //open port 

#if defined(__PLATFORM_WIN32__)
   WSAStartup(MAKEWORD(1,1), &_wsda);
#endif

   printf("Creating socket...");
   _s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

   if ( _s <= SOCKET_ERROR ) { 
#if defined(__PLATFORM_WIN32__)
      fprintf(stderr, "socket init failed with %d\n", WSAGetLastError());
#else
       fprintf(stderr, "socket init failed with %d\n", _s);
#endif
       _status = UDP_ERROR;
      return;
   }


   _status = UDP_UNBOUND;

}


void
UDP_Receiver::bind_to_port(int port) { 

   // Fill in the interface information

   if ( port >= 0 ) _port = port;
   printf("Binding socket...");
   _addr.sin_family = AF_INET;
   _addr.sin_port = htons(_port);
   _addr.sin_addr.s_addr = INADDR_ANY;

   int ret = bind( _s, (struct sockaddr *) &_addr, sizeof(_addr));

   if(  ret <= SOCKET_ERROR )
   {
#if defined(__PLATFORM_WIN32__)
       fprintf(stderr, "socket bind failed with %d\n", WSAGetLastError());
#else
       fprintf(stderr, "socket bind failed with %d\n", ret);
#endif
      _status = UDP_ERROR;
      return;
   }

   printf("receiver bound to port %d\n", _port);
   _status = UDP_BOUND;

}

int
UDP_Receiver::recv_next(char * buffer, int bsize) { 

   if ( _status != UDP_BOUND ) 
   { 
      fprintf( stderr, "socket not bound!\n"); return 0; 
   }
   
   //printf("waiting for messages...");

   _iRemoteAddrLen = sizeof(_remote_addr);

   SOCKLEN_T ret = recvfrom(_s, buffer, bsize, 0, (struct sockaddr *) &_remote_addr, &_iRemoteAddrLen);


   if( ret <= SOCKET_ERROR )
   {
#if defined(__PLATFORM_WIN32__)
      printf("Error\nCall to recvfrom failed :: %d\n", WSAGetLastError());
#else
       fprintf(stderr, "Error\nCall to recvfrom failed :: %d\n", ret );
#endif
      return 0;
   }

   //printf(" ... packet size %d received\n", ret);
   _mesgLen = ret;        // Length of the data received
   buffer[_mesgLen] = '\0';     // Convert to cstring

//   printf("\"%s\" received from %s\n", buffer, inet_ntoa(_remote_addr.sin_addr));

   return ret;
}

void
UDP_Receiver::close_sock() {
    shutdown ( _s, 1);

#if defined(__PLATFORM_WIN32__)
   closesocket(_s);
   WSACleanup();
#else
   close ( _s );
#endif
}

void
UDP_Transmitter::init() { 

   //open port 

#if defined(__PLATFORM_WIN32__)
   WSAStartup(MAKEWORD(1,1), &_wsda);
#endif   
   _s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

   if ( _s <= 0  ) { 
#if defined(__PLATFORM_WIN32__)
       fprintf(stderr, "socket init failed with %d\n", WSAGetLastError());
#else
       fprintf(stderr, "socket init failed with %d\n", _s);
#endif
      _status = UDP_ERROR;
      return;
   }
   _status = UDP_READY;
}


void
UDP_Transmitter::set_host(char * hostaddress, int port) { 

   // Fill in the interface information

   struct hostent * host;
   _port = port;
   printf("gathering host information...");
   _addr.sin_family = AF_INET;
   _addr.sin_port = htons(_port);
   _addr.sin_addr.s_addr = inet_addr(hostaddress);

   if(_addr.sin_addr.s_addr == INADDR_NONE) // The address wasn't in numeric
   {
      host = NULL;
      host = gethostbyname(hostaddress); // Get the IP address of the server
      if(host == NULL)
      {
         printf("UDP_Transmitter::error\nUnknown host: %s\n", hostaddress);
         return;
      }
      memcpy(&_addr.sin_addr, host->h_addr_list[0], host->h_length);
      printf("OK\n");
   } 
   else printf("..set\n");
   _status = UDP_READY;
}


int
UDP_Transmitter::send(char * buffer, int bsize) { 

   if ( _status != UDP_READY ) { fprintf( stderr, "socket not bound!\n"); return 0; }
   
   int ret = sendto(_s, buffer, bsize, 0, (struct sockaddr *) &_addr, sizeof(_addr) );
   if(ret <= 0)
   {
#if defined(__PLATFORM_WIN32__)
       fprintf(stderr, "UDP_Transmitter::error\nCall to send failed :: %d\n", WSAGetLastError());
#else
       fprintf(stderr, "UDP_Transmitter::error\nCall to send failed :: %d\n", ret);
#endif
      return 0;
   }
   return ret;

}

void
UDP_Transmitter::close_sock() {
    

#if defined(__PLATFORM_WIN32__)
   shutdown ( _s, 1);
   closesocket(_s);
   WSACleanup();
#else
   close (_s);  
   shutdown ( _s, 1);
#endif

}
