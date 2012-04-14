//-----------------------------------------------------------------------------
// file: audicle_stats.h
// desc: ...
//
// author: Ge Wang (gewang@cs.princeton.edu)
//         Perry R. Cook (prc@cs.princeton.edu)
//         Philip Davidson (philipd@cs.princeton.edu)
//         Ananya Misra (amisra@cs.princeton.edu)
// date: Autumn 2004
//-----------------------------------------------------------------------------
#ifndef __AUDICLE_STATS_H__
#define __AUDICLE_STATS_H__

#include "chuck_def.h"
#include "util_thread.h"

#include <map>
#include <queue>
#include <string>
using namespace std;


// we are tracking things
#define __AUDICLE_STAT_TRACK__

// forward reference
class Chuck_VM;
class Chuck_VM_Shred;
struct Shred_Data;
struct Shred_Time;


struct ShredActivation
{
    t_CKTIME when;
    t_CKUINT cycles;
    
    ShredActivation( t_CKTIME a, t_CKUINT b ) { when = a; cycles = b; }
};




//-----------------------------------------------------------------------------
// name: class Shred_Stat
// desc: ...
//-----------------------------------------------------------------------------
class Shred_Stat
{
public:
    // instructions computed
    t_CKUINT cycles;
    // shred id
    t_CKUINT id;
    // parent
    t_CKUINT parent;
    // current state, 0 = inactive, 1 = active, 2 = wait, 3 = deleted
    t_CKUINT state;
    // reference (could be pointing to garbage - see state)
    Chuck_VM_Shred * shred_ref;
    // number of activations
    t_CKUINT activations;
    // average control rate
    t_CKFLOAT average_ctrl;
    // average cycles
    t_CKFLOAT average_cycles;
    // spork time
    t_CKTIME spork_time;
    // active time
    t_CKTIME active_time;
    // wake time
    t_CKTIME wake_time;
    // free time
    t_CKTIME free_time;
    // name
    string name;
    // owner
    string owner;
    // source
    string source;
    
    // ctrl rate calculation
    queue<t_CKDUR> diffs;
    // number of diffs
    t_CKUINT num_diffs;
    // total diffs
    t_CKDUR diff_total;
    
    // exe per activation
    queue<t_CKUINT> act_cycles;
    // total
    t_CKUINT act_cycles_total;
    // last
    t_CKUINT last_cycles;
    
    // children
    vector<Shred_Stat *> children;
    void get_sporked( vector<Shred_Stat *> & out );

    vector<ShredActivation> activationss;
    void get_activations( vector<ShredActivation> & out );
    
    // mutex
    XMutex mutex;
    
    // audicle info
    Shred_Data * data;
    Shred_Time * time;

public:
    Shred_Stat() { this->clear(); num_diffs = 8; data = NULL; time = NULL; }
    void clear()
    { id = 0; parent = 0; state = 0; cycles = 0; activations = 0;
      average_ctrl = 0.0; spork_time = 0.0; active_time = 0.0; wake_time = 0.0;
      free_time = 0.0; name = "no name"; owner = "none"; source = "nowhere";
      while( diffs.size() ) diffs.pop(); diff_total = 0.0;
      while( act_cycles.size() ) act_cycles.pop(); act_cycles_total = 0;
      last_cycles = 0; children.clear(); }
};




//-----------------------------------------------------------------------------
// name: class AudicleStats
// desc: ...
//-----------------------------------------------------------------------------
class AudicleStats
{
public:
    static AudicleStats * instance();

public:
    void set_vm_ref( Chuck_VM * _vm ) { vm = _vm; } 

public:
    void add_shred( Chuck_VM_Shred * shred );
    void activate_shred( Chuck_VM_Shred * shred );
    void advance_time( Chuck_VM_Shred * shred, t_CKTIME to );
    void deactivate_shred( Chuck_VM_Shred * shred );
    void remove_shred( Chuck_VM_Shred * shred );

public:
    Shred_Stat * get_shred( t_CKUINT id )
    { mutex.acquire(); Shred_Stat * s = shreds[id]; mutex.release(); return s; }
    void get_shreds( vector<Shred_Stat *> & out, map<Shred_Stat *, Shred_Stat *> & d );
    static t_CKBOOL activations_yes;

protected:
    AudicleStats();
    ~AudicleStats();
    
    static AudicleStats * our_instance;
    
protected:
    Chuck_VM * vm;
    map<t_CKUINT, Shred_Stat *> shreds;
    vector<Shred_Stat *> done;
    XMutex mutex;
};




#endif
