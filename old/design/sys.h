
#ifndef _SYS_H_INCLUDED_
#define _SYS_H_INCLUDED_


#include <cassert>
#include <cmath>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <cstdarg>
#include <iostream>
#include <iomanip>
#include <map>


#include <float.h>
#include <vector>
#include <string>
#include <sstream>
//vectors
using std::string;


//glut

#if defined (__PLATFORM_MACOSX__)

#include <OpenGL/gl.h>						// Header File For The OpenGL32 Library
#include <OpenGL/glu.h>						// Header File For The GLu32 Library
#include <GLUT/glut.h>

#else //non MAC

#ifdef __PLATFORM_WIN32__
#include <windows.h>
#endif

#include <GL/gl.h>						// Header File For The OpenGL32 Library
#include <GL/glu.h>						// Header File For The GLu32 Library
#include <GL/glut.h>
#endif

extern bool cursorOn;


//#include <GL/glaux.h>

#define VECTOR(type) std::vector < type >
#define MAKE_STRING( msg )  ( ((std::ostringstream&) (std::ostringstream() << msg )).str() )
#define MAKE_CSTRING( msg )  ( ((std::ostringstream&) (std::ostringstream() << msg )).str().c_str() )



#define SYS_RETURN 13


#ifdef __PLATFORM_WIN32__

//glut location
#define DM '\\'
#define SYS_BACKSPACE 8

//windows specific includes
#include <windows.h>

//file dialogs
#include <commdlg.h>

//internet
#include <winsock.h>

#define SOCKLEN_T int

//timing
#include <sys/timeb.h>

//threading stuff
#include <commctrl.h>			//Windows Common Control
#include <process.h>			//Include for Thread Functions
//end stuff


#else //non win32..either __PLATFORM_MACOSX__ or __PLATFORM_LINUX__

#if defined (__PLATFORM_MACOSX__) //mac specific
#include <AGL/agl.h>
#include <Carbon/Carbon.h>
#define SYS_BACKSPACE 127
#define SOCKLEN_T int

#endif

#if defined (__PLATFORM_LINUX__) 
#define SYS_BACKSPACE 8
#define SOCKLEN_T socklen_t

#endif 

#define DM '/'
#define BOOL int
#define SOCKET_ERROR -1

//internet
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <fcntl.h>
#include <memory.h>

#define SOCKET int
#define SOCKADDR_IN struct sockaddr_in

//timing
#include <sys/time.h>

#endif


//constants

#ifndef PI
#define PI 3.14159265358979f
#endif //PI

#ifndef TWO_PI
#define TWO_PI 6.28318530718f
#endif   //TWO_PI

#ifndef ROOTTWO
#define ROOTTWO 1.41421356237f
#endif   //ROOTTWO

typedef unsigned int  uint;
typedef unsigned char uchar;


#ifndef NOMINMAX

#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif

#endif  /* NOMINMAX */


//helpful keydefs for nonstandard keys

#define KEY_F1			1
#define KEY_F2			2
#define KEY_F3			3
#define KEY_F4			4
#define KEY_F5			5
#define KEY_F6			6
#define KEY_F7			7
#define KEY_F8			8
#define KEY_F9			9
#define KEY_F10			10
#define KEY_F11			11
#define KEY_F12			12

#define KEY_SPACE    ' '
#define KEY_BQ       '`'
#define KEY_ESCAPE   '\033'
#define KEY_RETURN   '\13'
#define KEY_TAB      '\t'

#define KEY_UPARROW     101
#define KEY_DOWNARROW   103
#define KEY_LEFTARROW   100
#define KEY_RIGHTARROW  102

#define KEY_PGUP     104
#define KEY_PGDN     105
#define KEY_HOME     106
#define KEY_END      107
#define KEY_INS      108

#define KEY_CTRL_A     1
#define KEY_CTRL_B     2
#define KEY_CTRL_C     3
#define KEY_CTRL_D     4
#define KEY_CTRL_E     5
#define KEY_CTRL_F     6
#define KEY_CTRL_G     7
#define KEY_CTRL_H     8
#define KEY_CTRL_I     9
#define KEY_CTRL_J     10
#define KEY_CTRL_K     11
#define KEY_CTRL_L     12
#define KEY_CTRL_M     13
#define KEY_CTRL_N     14
#define KEY_CTRL_O     15
#define KEY_CTRL_P     16
#define KEY_CTRL_Q     17
#define KEY_CTRL_R     18
#define KEY_CTRL_S     19
#define KEY_CTRL_T     20
#define KEY_CTRL_U     21
#define KEY_CTRL_V     22
#define KEY_CTRL_W     23
#define KEY_CTRL_X     24
#define KEY_CTRL_Y     25
#define KEY_CTRL_Z     26






#endif //_SYS_H_INCLUDED
