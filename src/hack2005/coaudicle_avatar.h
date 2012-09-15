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
// name: coaudicle_avatar.h
// desc: ...
//
// authors: Philip Davidson (philipd@cs.princeton.edu)
//          Ge Wang (gewang@cs.princeton.edu)
//          Perry R. Cook (prc@cs.princeton.edu)
//          Ananya Misra (amisra@cs.princeton.edu)
// date: Autumn 2005
//-----------------------------------------------------------------------------
#ifndef __COAUDICLE_AVATAR_H_INCLUDED__
#define __COAUDICLE_AVATAR_H_INCLUDED__

#include "coaudicle.h"
#include "audicle_gfx.h"
#include "audicle_geometry.h"

#include "rgbhsl.h"

t_CKFLOAT arandf();
t_CKFLOAT Rand(); 
t_CKFLOAT Rand_in ( t_CKFLOAT a, t_CKFLOAT b ); 

float hat ( float x, float frq );
//renderer for an Agent 
//avatar for co-audicle users
class Agent; 

class Skin { 

protected:

   //   std::vector < MeshSet > _meshes;

   //this are here in no particular order
   //for the derived classes.

   // GL display lists

   GLuint _body_dlist;
   GLuint _head_dlist;
   GLuint _arm_dlist;
   GLuint _armupper_dlist;
   GLuint _armlower_dlist;
   GLuint _eye_dlist;
   GLuint _leg_dlist;
   GLuint _legupper_dlist;
   GLuint _leglower_dlist;
   GLuint _foot_dlist;
   GLuint _knee_dlist;

   GLuint _screen_dlist;

   GLuint _pole_dlist;
   GLuint _sign_dlist;
   GLuint _tv_dlist;
   GLuint _tvscreen_dlist;

   GLuint _bindle_stripe_tex;


   GLdouble mmtx[16]; //current body transform 

   //add rendering parameters - arm height, head, etc....

   Point3D _body_scale;
   Point3D _torso_scale;
   Point3D _shoulder_base;
   Point3D _hand_base; //hand(pole)'s center rel to shoulder
   Point3D _sign_base; //sign's center rel to pole. 
   double  _axis_distance;
   Point3D _arm_scale;
   Point3D _rhip;
   Point3D _lhip;

   Point3D _head_pos;
   Point3D _foot_pos;
   Point3D _foot_scale;
   Point3D _eye_pos; //relative to head pos!
   Point3D _leg_scale;

   bool    _built;

   int     _walkmode;

public:

   Skin () : 
      _body_dlist(0),
      _head_dlist(0),
      _arm_dlist(0),
      _armupper_dlist(0),
      _armlower_dlist(0),
      _eye_dlist(0),
      _leg_dlist(0),
      _legupper_dlist(0),
      _leglower_dlist(0),
      _foot_dlist(0),
      _pole_dlist(0),
      _sign_dlist(0),
      _tv_dlist(0),
      _tvscreen_dlist(0),
      _screen_dlist(0),
      _walkmode(0)
   {
   } 
   virtual ~Skin() {}
   virtual void load_meshes(char * mapfile);
   double       complexity();
   Point3D      l_eye_scale(Agent * ag);
   Point3D      r_eye_scale(Agent * ag);
   virtual Point3D lhip() { return _lhip;} 
   virtual Point3D rhip() { return _rhip;} 
   //virtual Mesh* mesh(char *name);
   virtual void build_skin();
   virtual void draw_dummy();
   virtual void draw_agent(Agent * ag);
   virtual void draw_body(Agent * ag);
   virtual void draw_arms(Agent * ag);
   virtual void draw_legs(Agent * ag);
   virtual void draw_head(Agent * ag);
   
   virtual void draw_shadow(Agent * ag);
   virtual void draw_bindle(Agent * ag);
   virtual void draw_halo  (Agent * ag);

   virtual void set_walk( int i );

};


// skin with identifying info 

struct hanger { 
   char name[256];
   Skin * skin;
};

// holds a collection of skins

class Closet  {
protected:
   std::vector < hanger * > _skins;
   int               _cur_skin;
 public:
   Closet() { _skins.clear(); _cur_skin = 0; }
   void addSkin(Skin * skin, const char * name) { 
     hanger * s = new hanger;
     s->skin = skin;
     strcpy(s->name, name );
     _skins.push_back(s);
   }
   void next_skin() { if ( _skins.size() ) _cur_skin = (_cur_skin+1)%_skins.size(); } 
   Skin * curskin() { if ( _skins.size() ==  0 ) return NULL; else return skin(_cur_skin); } 
   Skin * skin(int i) { return _skins[i]->skin; } 
   Skin * getSkin( const char * n )  {
     for ( t_CKUINT i=0; i < _skins.size(); i++ ) { 
       if ( strcmp ( n, _skins[i]->name ) == 0 ) return skin(i);
     }
     return NULL;
   }
   //Skin * randSkin() { return _skins[iRand(_skins.size())]->skin; } 
};

//simple, default agent renderer 

class DefSkin : public Skin { 
 public:
  DefSkin() : Skin() { Skin::build_skin(); }
  virtual ~DefSkin() {}
  
};

//need meshes - we'll punt for now. 

/*
class MeshBot : public Skin { 

public:
   virtual void draw_head(Agent * ag);
   virtual void build_skin(char * c = NULL);   
   MeshBot(char * c = NULL) : Skin() { build_skin(c); }
   virtual ~MeshBot() {}
};

class ToonBot : public MeshBot { 
public:
   virtual void draw_body(Agent * ag, GLdouble * mmtx);
   virtual void draw_head(Agent * ag);
   ToonBot (char * c = NULL) : MeshBot(c) {}
   virtual ~ToonBot() {}
};
*/


enum  {AGENT_NORMAL, AGENT_SANTA, AGENT_KITTY, AGENT_TV_HEAD, AGENT_TV_DONLY, AGENT_STYLE_NUM }; 
enum { AGENT_LEGS_SIMPLE, AGENT_LEGS_JOINTED, AGENT_LEGS_NONE } ;

//class Agent does what's needed. moves around & walks & things. 
enum { AGENT_MOVING, AGENT_STOPPING, AGENT_STOPPED };

class Agent { 

private:

   static Closet* _closet; 
   static void fillCloset();

   static double _leglen;      //3.0
   static double _kneelevel; //0.5
   static double _steprate; //0.5
   static double _walkheight; //2.7
   static double _feet_up;
   static double _stepcenter; //-0.15
   static double _bounce_base; //0.15
   static double _bounce_mag;  //0.8
   static double _speedmul;
   static double _wobble;

   static double _dcomplexity;

   Point3D       _r_eye_scale;
   Point3D       _l_eye_scale;

public:
   // for holding pointer
   t_CKINT SELF;

   // Crowd * cy;
   Skin * skin;
   
   //COCOUSERSTATE
   
   Point3D home;   
   Point3D pos, lastpos;
   Point3D vel;
   
   Color4D rgb;
   Color4D dresscol;
   
   std::string message;
   
   //

   Color4D def_rgb;
   Color4D defcol;
   Color4D skincol;
   
   static t_CKUINT _dAgentStyle; 
   static bool _debug; 
   static Closet * closet(); 
   friend class Skin;

   bool inbounds;

   int  changeskin;
   bool blink;
   double sincelastBlink;
   
   bool left_down;      //which foot is down

   double stride_fore;  //how far forward a step is taken
   double stride_back;  //how far back a step is lifted

   GLdouble transform[16];

   Point3D rfoot;       //current pos(global);
   Point3D lfoot;       //current pos(global);

   Point3D rdir;
   Point3D ldir;

   Point3D rhip;        //where attaches to body
   Point3D lhip;        //where attaches to body
   
   double  rlastdown;   //last placement ( relative )
   Point3D rlastpos;     //last lift      ( relative )
   Point3D rlastdir;

   double  llastdown;   //last placement ( relative )
   Point3D llastpos;     //last life      ( relative )
   Point3D llastdir;

   double stepphase;

   double yvel;
   double yfrc;

   Point3D frc; //steering forces
   Point3D up;

   //physical model - not yet!
   /*
   Particle * base;
   Particle * head;
   Edge * upe;
   */

   void pose_home();

   void do_lean_back( );
   void do_arms_up( );
   void do_arm_up( );
   void do_head_bang( );
   void do_head_bang_once( );
   void do_jump( );
   void do_walk_tall( );
   //void do_hip_shake();

   void set_lean_back( float dt );
   void set_arms_up( float dt);
   void set_arm_up( float dt);
   void set_head_bang( float dt );
   void set_jump ( float dt);
   void set_walk_tall( float dt );
   //void set_hip_shake( float dt );

   float lean_back;
   float arms_up;
   float head_bang;
   float arm_up;
   float jump;
   float walk_tall; 
   float hip_shake;


   float lean_back_time ;
   float arms_up_time ;
   float head_bang_time ;
   float arm_up_time ;
   float jump_time; 
   float walk_tall_time;
   float hip_shake_time;

   float jump_height;

   float height_mul;
   float shoulder_rotate; 
   float bend_forward ;
   float bend_side ;
   float waist_turn ;
   float arm_swing_mag;
  
   float angle;
   float moveSpeed;
   float headangle;

   float l_armangle;
   float r_armangle;
   
   bool holding_sign;
   bool holding_tv;
   int  sign_tex;
   float sign_aspect;
   float sign_area;
   float sign_width;
   float sign_height;
   //Agent* following;
   //int   num_followers;
   //Agent**    followers;
   //int foll_size;
   
   
   float speed;
   t_CKUINT movement; 
   float swingspeed;
   float swingphase;
   
   float width;
   float step;
   float spinAngle;
   
   //   Agent(Crowd *);

   Agent();
   virtual ~Agent(){};
   bool test_collision(Agent * b, double dt);
   bool avoid(Point3D loc, Point3D cause, Point3D away, double time);

   void use_skin(Skin * s);
   static void register_globals();
   void calc_secondary_frc();
   void calc_body_transform( int walkmode );
   void calc_feet(Point3D xlate);
   void init();
   //void remove_all_followers();
   //   void remove_follower(Agent* follower);
   //void add_follower   (Agent* follower);
   void fix_state();
   void update(double timeStep);
   void draw();

   void draw_debug();

   void printme(FILE* fp);
   void print_pov(FILE* fp);
   //   virtual  gtype isa()    { return AGENT;  }
};

class CoAvatar { 
public:
    
    CoAvatar();
    ClientWindowManager* & wm() { return m_wm; }
    Point3D&     pos();
    Point3D&     home() { return agent()->home; } 
    Vec3D&      vel();
    void apply_window_transform();
    void render_windowmanager();
    void pre_render();
    void render();
    void render_character();
    void render_bindle();
    void render_halo();
    void render_sign();
    void post_render();
    void animate( t_CKFLOAT dtime );
    void animate_state( t_CKFLOAT dtime );
    
    void moveTo( Point3D pos, Vec3D dir );
    void renderMessage() ;
    Agent *     agent();

    void push_agent( Agent * );
    void pop_agent();

    void set_agent(Agent* a);
    
protected:
        
    Skin  *     skin();
    Skin  *     m_skin;
    Agent *     m_agent; 
    Agent *     m_agent_tmp;
    CoSession * m_session;
    CoUser *    m_user;  //where the data comes from to render
//    CoServer *    m_server;
    ClientWindowManager * m_wm;
//    CoBindle *  m_bindle;
};

#endif //__COAUDICLE_AVATAR_H_INCLUDED__
