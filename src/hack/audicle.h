//-----------------------------------------------------------------------------
// name: audicle.h
// desc: interface for audicle
//
// authors: Ge Wang (gewang@cs.princeton.edu)
//          Perry R. Cook (prc@cs.princeton.edu)
//          Philip Davidson (philipd@cs.princeton.edu)
//          Ananya Misra (amisra@cs.princeton.edu)
// date: Autumn 2004
//-----------------------------------------------------------------------------
#ifndef __AUDICLE_H__
#define __AUDICLE_H__

#include "audicle_face.h"
#include <vector>




//-----------------------------------------------------------------------------
// name: class Audicle
// desc: ...
//-----------------------------------------------------------------------------
class Audicle
{
public: // how to get the single audicle instance
    static Audicle * instance( );

public: // initialization and cleanup
	t_CKBOOL init( );
	t_CKBOOL shutdown( );

public:
    enum { UP = 0, DOWN = 1, LEFT = 2, RIGHT = 3, BACK = 4 , CUR = 5};
    enum { NO_FACE = 0xffffffff };

public: // look interface
    t_CKUINT look( t_CKUINT dir, t_CKUINT n = 1 );
    t_CKUINT look_here( );
    t_CKUINT look_from( t_CKUINT i, t_CKUINT dir ) const;     

public: // move interface
    AudicleFace * face( );
    AudicleFace * face( t_CKUINT index );
    AudicleFace * move( t_CKUINT dir, t_CKUINT n = 1 );
    AudicleFace * move_to( t_CKUINT index );
    AudicleFace * move_to( AudicleFace * face );
    
    
public: // face interface
    t_CKUINT add( AudicleFace * face );
    t_CKBOOL swap( t_CKUINT a, t_CKUINT b );
    t_CKBOOL remove( AudicleFace * face );
    t_CKBOOL remove( t_CKUINT index );
    
protected: // constructors and destructor
    Audicle( );
    ~Audicle( );
    
protected: // instance
    static Audicle * our_instance;
    
protected: // data
    
    std::vector<AudicleFace *> m_faces;
    std::vector< std::vector<int> > m_matrix;
    t_CKUINT m_current;
    t_CKUINT m_last;
    
    t_CKBOOL m_switching;
    t_CKUINT m_switch_which;
    t_CKFLOAT m_switch_start; //time
    t_CKFLOAT m_switch_last_face;
    
    t_CKBOOL m_init;
};




#endif
