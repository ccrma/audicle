//-----------------------------------------------------------------------------
// name: audicle_event.h
// desc: interface for audicle event
//
// authors: Ge Wang (gewang@cs.princeton.edu)
//          Perry R. Cook (prc@cs.princeton.edu)
//          Philip Davidson (philipd@cs.princeton.edu)
//          Ananya Misra (amisra@cs.princeton.edu)
// date: Autumn 2004
//-----------------------------------------------------------------------------
#ifndef __AUDICLE_EVENT_H__
#define __AUDICLE_EVENT_H__

#include "audicle_def.h"
#include "audicle_geometry.h"   // naughty

//----------------------------------------------------------------------------
// name: class AudicleEvent
// desc: ...
//----------------------------------------------------------------------------

enum ae_event_type { ae_event_AUDICLE, ae_event_INPUT, ae_event_VM_MESG }; 


class AudicleEvent
{
public:
    AudicleEvent() { this->clear(); }
    virtual ~AudicleEvent() { }

public:
    void clear();

public:
    t_CKUINT type;
    t_CKUINT message;
    t_CKUINT param1;
    t_CKUINT param2;
    t_CKUINT param3;
    t_CKUINT param4;
    t_CKTIME timestamp;
    void *   data;
};

enum ae_input_type {    ae_input_NONE=0, ae_input_MOUSE, ae_input_MOTION, \
                        ae_input_KEY, ae_input_SPEC, ae_input_NTYPES \
                        };


struct InputEvent {

    ae_input_type   type;
    Point2D         pos;
    int             button;
    int             state;
    int             key;
    double          time;
    t_CKUINT        mods;
    t_CKUINT *      stack;
    t_CKUINT        size;


    InputEvent();
    InputEvent( ae_input_type itype, Point2D ipt, int ibutton, int istate);    
    InputEvent( ae_input_type itype, Point2D ipt);
    InputEvent( ae_input_type itype, Point2D ipt, int ikey );
    void clear();

    void setMods( t_CKUINT imods )                      { mods  = imods; } 
    void setStack( t_CKUINT * istack, t_CKUINT isize )  { stack = istack; size = isize; }
    Point2D p() { return pos; } 
    
    void popStack( ); 
    void fprint( FILE * fd) const; 
    t_CKUINT curID( void ) const;
    bool checkID( t_CKUINT id ) const;
};




struct InputState {

    InputEvent event;
	bool isDepressed;
	bool leftDown;
	bool midDown;
	bool rightDown;

	Point2D dPos;
	Point2D lastPos;
	Point2D lastClick;

	char lastKey;
	int  lastSpec;
	bool ctrlDown;
	bool altDown;
	bool shiftDown;

    double dtime;
    double lasttime;

    InputState();

    void setEvent ( const InputEvent &e);
    void setEvent ( const InputEvent &e , Point2D local_pt);

    void setMouse ( const InputEvent &e );
	void setMouse ( Point2D pos, int button=-1, int state=-1 );

    void setKey  ( const InputEvent &e ) { keyState(e); lastKey = e.key; }
	void setKey  ( char c ) { keyState(); lastKey = c;  }

    void setSpec ( const InputEvent &e ) { keyState(e); lastSpec = e.key; }
    void setSpec ( int c  ) { keyState(); lastSpec = c; }

    void setTime ( double t ) { dtime = t - lasttime; lasttime = t; }
	
    void keyState( const InputEvent &e ); 
    void keyState();
};



#endif
