//-----------------------------------------------------------------------------
// name: audicle_event.cpp
// desc: interface for audicle event
//
// authors: Ge Wang (gewang@cs.princeton.edu)
//          Perry R. Cook (prc@cs.princeton.edu)
//          Philip Davidson (philipd@cs.princeton.edu)
//          Ananya Misra (amisra@cs.princeton.edu)
// date: Autumn 2004
//-----------------------------------------------------------------------------
#include "audicle_event.h"
#include <memory.h>

#include "audicle_gfx.h"        // naughty

//-----------------------------------------------------------------------------
// name: clear()
// desc: ...
//-----------------------------------------------------------------------------
void AudicleEvent::clear()
{
    memset( this, 0, sizeof(*this) );
}


//InputEvent

InputEvent::InputEvent()
{ 
    clear();
}

InputEvent::InputEvent( ae_input_type itype, Point2D ipt, int ibutton, int istate)
{ 
    clear();
    type    = itype;
    pos     = ipt;
    button  = ibutton;
    state   = istate; 
}
    
InputEvent::InputEvent( ae_input_type itype, Point2D ipt)
{
    clear();
    type = itype;
    pos = ipt;
}

InputEvent::InputEvent( ae_input_type itype, Point2D ipt, int ikey )
{
    clear();
    type = itype;
    pos = ipt;
    key = ikey;
}

void 
InputEvent::clear() {        
    type    = ae_input_NONE;
    pos     = Point2D(0,0);
    button  = -1;
    state   = -1;
    key     = -1;
    time    = 0.0;
    mods = 0;
    stack   = NULL;
    size    = 0;
}


bool 
InputEvent::checkID(t_CKUINT id )  const   
{ 
    return ( id && id == curID()); 
}

t_CKUINT 
InputEvent::curID ( void )     const   
{ 
    return ( size ) ? *stack : 0 ; 
}

void 
InputEvent::popStack( ) 
{ 
    size  = ( size > 0  ) ? size-1  : 0; 
    stack = ( size > 0  ) ? stack+1 : NULL ; 
} 

void 
InputEvent::fprint( FILE * fd) const { 
    Point2D p = pos;
    switch ( type ) { 
        
    case ae_input_MOUSE: 
        fprintf( fd, "Mouse Event : (%f,%f) %d %d\n", p[0], p[1], button, state );
        break;
    case ae_input_MOTION: 
        fprintf( fd, "Motion Event : (%f,%f)\n", p[0], p[1] );
        break;
    case ae_input_KEY: 
        fprintf( fd, "Key Event : key %c (%d) - mods %d - (%f,%f)\n", key, key, mods, p[0], p[1]);
        break;
    case ae_input_SPEC: 
        fprintf( fd, "Spec Event : key (%d) - mods %d - (%f,%f)\n", key, mods, p[0], p[1]);
        break;
    }
}




InputState::InputState() : 
	isDepressed(false), 
		leftDown(false), 
		midDown(false), 
		rightDown(false), 
		altDown(false), 
		ctrlDown(false), 
		shiftDown(false), 
		lastPos(Point2D(0,0)),
		dPos(Point2D(0,0)),
		lastClick(Point2D(0,0)),
		lastKey('a'),
		lastSpec(0),
        dtime(0),
        lasttime(0)
	{}


void
InputState::setEvent ( const InputEvent &e ) 
{ 
    setEvent ( e, e.pos );    
}

void 
InputState::setEvent ( const InputEvent &e , Point2D local_pt) 
{ 
    event = e;
    event.pos = local_pt;
    setMouse  ( event );
    if ( e.type == ae_input_KEY ) setKey(event);
    if ( e.type == ae_input_SPEC ) setSpec(event);
//    setTime(event.time);
}


void 
InputState::setMouse ( const InputEvent &e ) 
{

    Point2D p = e.pos;
    dPos = p - lastPos;

    switch ( e.button ) { 
    case ae_input_LEFT_BUTTON:
        leftDown = ( e.state == ae_input_DOWN ) ? true: false;
        break;
    
    case ae_input_RIGHT_BUTTON:
        rightDown = ( e.state == ae_input_DOWN ) ? true : false;
        break;
    }

    isDepressed = ( leftDown | rightDown );
    if ( e.button >= 0 ) keyState ( e );
    if ( e.state == ae_input_DOWN ) lastClick = e.pos;
    lastPos = e.pos;

}

void 
InputState::setMouse ( Point2D pos, int button, int state ) { 

	dPos = pos - lastPos;

	switch ( button ) {
	case ae_input_LEFT_BUTTON:
		leftDown =  ( state == ae_input_DOWN ) ? true : false;
		break;
	case ae_input_MIDDLE_BUTTON:
		midDown =   ( state == ae_input_DOWN ) ? true : false;
		break;
	case ae_input_RIGHT_BUTTON:
		rightDown = ( state == ae_input_DOWN ) ? true : false;
		break;
	}

	isDepressed = ( leftDown | midDown | rightDown );
	if ( button >= 0 ) keyState();
	if ( state == ae_input_DOWN ) lastClick = pos;
	lastPos = pos;

}








    void 
    InputState::keyState( const InputEvent &e ) { 
		altDown     =  !!( e.mods & ae_input_ALT );
		shiftDown   =  !!( e.mods & ae_input_SHIFT );
		ctrlDown    =  !!( e.mods & ae_input_CTRL );
    }

    void 
    InputState::keyState() { 
		altDown     =  !!( glutGetModifiers() & ae_input_ALT    );
		shiftDown   =  !!( glutGetModifiers() & ae_input_SHIFT  );
		ctrlDown    =  !!( glutGetModifiers() & ae_input_CTRL   );
	}


