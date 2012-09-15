//-----------------------------------------------------------------------------
// name: audicle.cpp
// desc: interface for audicle
//
// authors: Ge Wang (gewang@cs.princeton.edu)
//          Perry R. Cook (prc@cs.princeton.edu)
// date: 2/16/2004
//-----------------------------------------------------------------------------
#include "audicle.h"


// face transitions
const int g_transitions[6][5] = { { 4, 5, 3, 1, 2 },
                                  { 4, 5, 0, 2, 3 },
                                  { 4, 5, 1, 3, 0 },
                                  { 4, 5, 2, 0, 1 },
                                  { 2, 0, 3, 1, 5 },
                                  { 0, 2, 3, 1, 4 } };
Audicle * Audicle::our_instance = NULL;



//-----------------------------------------------------------------------------
// name: instance( )
// desc: ...
//-----------------------------------------------------------------------------
Audicle * Audicle::instance( )
{
    if( !our_instance )
    {
        our_instance = new Audicle;
        if( !our_instance )
        {
            fprintf( stderr, "[audicle]: really really bad error: out of memory\n" );
            assert( FALSE );
        }
    }
    
    return our_instance;
}



//-----------------------------------------------------------------------------
// name: Audicle()
// desc: ...
//-----------------------------------------------------------------------------
Audicle::Audicle()
{
    m_current = NO_FACE;
    
    m_switching = FALSE;
    m_switch_which = NO_FACE;
    m_switch_start = 0.0;
    m_switch_last_face = NO_FACE;
    
    m_init = FALSE;
}




//-----------------------------------------------------------------------------
// name: ~Audicle()
// desc: ...
//-----------------------------------------------------------------------------
Audicle::~Audicle()
{
    // do nothing for now
    m_init = FALSE;
}




//-----------------------------------------------------------------------------
// name: init()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL Audicle::init( )
{
    if( m_init )
        return TRUE;

    m_init = TRUE;
    return TRUE;
}




//-----------------------------------------------------------------------------
// name: shutdown()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL Audicle::shutdown( )
{
    if( !m_init )
        return TRUE;
        
    m_init = FALSE;
    
    return TRUE;
}




//-----------------------------------------------------------------------------
// name: look()
// desc: ...
//-----------------------------------------------------------------------------
t_CKUINT Audicle::look( t_CKUINT dir, t_CKUINT n )
{
    t_CKUINT current = m_current;
    
    if( n && dir >= 4 ) return NO_FACE;

    while( n-- )
        current = g_transitions[current][dir];

    return current;
}




//-----------------------------------------------------------------------------
// name: look_here()
// desc: ...
//-----------------------------------------------------------------------------
t_CKUINT Audicle::look_here()
{
    return m_current;
}




//-----------------------------------------------------------------------------
// name: look_from
// desc: ...
//-----------------------------------------------------------------------------
t_CKUINT 
Audicle::look_from( t_CKUINT i, t_CKUINT dir ) const { 
    return g_transitions[i][dir];
}




//-----------------------------------------------------------------------------
// name: face()
// desc: ...
//-----------------------------------------------------------------------------
AudicleFace * Audicle::face()
{
    if( m_current == NO_FACE ) return NULL;
    
    return m_faces[m_current];
}




//-----------------------------------------------------------------------------
// name: face()
// desc: ...
//-----------------------------------------------------------------------------
AudicleFace * Audicle::face( t_CKUINT index )
{
    if( index == NO_FACE || index >= m_faces.size() ) return NULL;
    
    return m_faces[index];
}




//-----------------------------------------------------------------------------
// name: move()
// desc: ...
//-----------------------------------------------------------------------------
AudicleFace * Audicle::move( t_CKUINT dir, t_CKUINT n )
{
    t_CKUINT dest = look( dir, n );
    if( dest == NO_FACE ) return NULL;
    if( dest == m_current ) return face();
    
    if( m_current != NO_FACE ) m_faces[m_current]->on_deactivate();
    m_current = dest;
    m_faces[m_current]->on_activate();
    
    return face();
}




//-----------------------------------------------------------------------------
// name: move_to()
// desc: ...
//-----------------------------------------------------------------------------
AudicleFace * Audicle::move_to( t_CKUINT index )
{
    t_CKUINT dest = index;
    if( dest == NO_FACE || dest >= m_faces.size() ) return NULL;
    if( dest == m_current ) return face();

    if( m_current != NO_FACE ) m_faces[m_current]->on_deactivate();
    m_current = dest;
    m_faces[m_current]->on_activate();

    return m_faces[m_current];
}




//-----------------------------------------------------------------------------
// name: move_to()
// desc: ...
//-----------------------------------------------------------------------------
AudicleFace * Audicle::move_to( AudicleFace * face )
{
    t_CKUINT dest = face->id();
    if( dest == NO_FACE || dest >= m_faces.size() || face != m_faces[dest] )
        return NULL;
    if( dest == m_current ) return face;

    if( m_current != NO_FACE ) m_faces[m_current]->on_deactivate();
    m_current = dest;
    m_faces[m_current]->on_activate();
    
    return face;
}




//-----------------------------------------------------------------------------
// name: add()
// desc: ...
//-----------------------------------------------------------------------------
t_CKUINT Audicle::add( AudicleFace * face )
{
    t_CKUINT id = face->id();
    if( id != NO_FACE ) return NO_FACE;
    
    m_faces.push_back( face );
    face->set_id( m_faces.size() - 1 );

    if( m_faces.size() == 1 )
    {
        m_current = face->id();
        face->on_activate();
    }

    return face->id();
}




//-----------------------------------------------------------------------------
// name: swap()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL Audicle::swap( t_CKUINT a, t_CKUINT b )
{
    if( a == NO_FACE || a >= m_faces.size() || b == NO_FACE || b >= m_faces.size() )
        return FALSE;
        
    AudicleFace * f = m_faces[a];
    m_faces[a] = m_faces[b];
    m_faces[b] = f;
    
    return TRUE;
}
