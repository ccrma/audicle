

/*****************************
 * CO AUDICLE SURPRISE
 *
 *
 *************************************/

#ifndef __COAUDICLE_H_DEFINED__
#define __COAUDICLE_H_DEFINED__

// threads
#include "audicle_def.h"
#include "audicle_geometry.h"

// server can point to your own session on your own Audicle,
// or to another server entirely . 
/*

(Co)Audicle -- ( Thread IPC ) -- Chuck 
CoAudicle -- Session -( Thread IPC )- Server -- Chuck
CoAudicle -- Session -(NETWORK)- -- Server -- Chuck 


CoAudicle - user interaction & control
          - creates/manages different sessions ( asks tracker ) 
          - renders a view of the session(s)
          - interacts with user buffers
	  - render buffers of other users

Session   - represents a connection to a particular Server
	  - sends updated info about session to server
          - 'owns' buffers being edited by user
	  - 'caches' buffers edited by other users
	  - maintains local cache of the other user-sessions on the server
	  - maintains local chat logs
	  * thread synchronizes to server
	  
	  USER SIDE ( AUDICLE )
	  ------------------------

	  ( NETWORK ? )

	  ------------------------
	  SERVER SIDE ( CHUCK )

Server    - accepts connections from user sessions
          - accepts & broadcasts user info to sessions
          - accepts & broadcasts buffer info to sessions
	  - send code from sessions to chuck 
	  - streams audio from chuck to sessions
	  - 'advertises' via tracker

Chuck     - audio computation, thread management, 
          - receives buffers from the session to code
	  - report compile/syntax errors on buffers
	  - report VM state
	  - report VM namespace/classes ?

	  - machine control messages
          - send statistics to server 
	  - stream audio to server
	   

*/

//defines for pre-link 
class CoSession;
class CoUser;
class CoChat;
class CoWindow; 
class CoWindowManager;
class CoShred;
class CoBindle;
class CoResource; 


class CoSessionManager { 
public:
    virtual CoSessionManager * instance();
    virtual CoSession * new_session();
    virtual std::vector < CoSession * >& sessions();
    virtual CoSession * cur_session();
};

// Session needs to run a sync thread to server


// interface only - server runs on this machine or remote. 
// CoAudicle manages between local/remote experience 

// on the server itself, should we have yet another version? 
 
class CoServer { 
public:
    virtual CoSession * sessions(); // connected sessions 
    virtual std::string   address();
    virtual CoChat * chat();
    virtual std::vector < CoResource * >& resources( int i );
    virtual void restart();
    virtual void connect( CoSession * );
};

// Either the local user, or someone on a different server. 

class CoSession {
public:
    virtual CoUser   * user();
    virtual CoServer * server();
    virtual CoChat   * chat();
    virtual CoWindowManager * wm();
    virtual CoShred * shreds();
    virtual bool local();

    virtual void start ( CoServer * );
    virtual unsigned int state(); // new, starting, connected, disconnected, idle, exited 
};

class CoWindowManager{ 
public:
    virtual CoWindow * current();
    virtual std::vector < CoWindow * > windows();
};

class CoShred { 
public:
    virtual int server();
    virtual int id();
};

class CoUser { 
public:
    virtual std::string username(); 
    virtual unsigned int id(); // uuid? 
    virtual std::string address();
    virtual CoServer * home();
    virtual CoBindle * bindle();    
};

class CoBuffer; 


class CoWindow { 
public:
    virtual Point2D location();
    virtual Point2D size();
    virtual CoBuffer * front();
    virtual std::vector < CoBuffer * > buffers(); //aka revisions
};

class CoBuffer { 
public:
    virtual std::string name();
    virtual std::string path();
    virtual std::string content();
};


class CoResource { 
public:
    virtual std::string name();
    virtual std::string location();
    virtual std::string type();
    virtual unsigned int   size();
    virtual char * data();
};

class CoBindle { 
public: 
    virtual std::vector < CoResource * >& resources();
};

class CoChatMesg { 
public:
    virtual unsigned int   num();
    virtual unsigned int   time();
    virtual std::string user();
    virtual std::string data();
};

class CoChat { 
public: 
    virtual CoServer server();
    virtual std::vector < CoChatMesg * > messages();
};



// renderer for session ( perhaps this gets #defined elsewhere. 

class CoSessionRenderer { 
public:
    virtual CoSession*& session();
    virtual void render();
};

/*
//session to server 

//a server actively manages the machine. 
single user - session - no public server - direct connect

user - directsession
     - localsession 
     - remotesession 


server - localuser
       - remoteuser
        //
       
single user - shared server - develops cross link 

single user - remote server - session w/ connection 

single user - view remote server .  

server is transparent to the 
*/

#endif
