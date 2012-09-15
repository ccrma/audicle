
#ifndef __COAUDICLE_AVATAR_H_INCLUDED__
#define __COAUDICLE_AVATAR_H_INCLUDED__

#include "coaudicle.h"
#include "audicle_gfx.h"
#include "audicle_geometry.h"

#include "rgbhsl.h"

t_CKFLOAT arandf();
t_CKFLOAT Rand(); 
t_CKFLOAT Rand_in ( t_CKFLOAT a, t_CKFLOAT b ); 


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

   GLuint _screen_dlist;

   GLuint _pole_dlist;
   GLuint _sign_dlist;
   GLuint _tv_dlist;
   GLuint _tvscreen_dlist;



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
   virtual void draw_body(Agent * ag,GLdouble *mmtx);
   virtual void draw_arms(Agent * ag);
   virtual void draw_legs(Agent * ag,GLdouble *mmtx);
   virtual void draw_head(Agent * ag);
   virtual void draw_shadow(Agent * ag);
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
   void addSkin(Skin * skin, char * name) { 
     hanger * s = new hanger;
     s->skin = skin;
     strcpy(s->name, name );
     _skins.push_back(s);
   }
   void next_skin() { if ( _skins.size() ) _cur_skin = (_cur_skin+1)%_skins.size(); } 
   Skin * curskin() { if ( _skins.size() ==  0 ) return NULL; else return skin(_cur_skin); } 
   Skin * skin(int i) { return _skins[i]->skin; } 
   Skin * getSkin( char * n )  {
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


   static GLuint _body_dlist;
   static GLuint _eye_dlist;
   static GLuint _head_dlist;
   static GLuint _arm_dlist;
   static GLuint _hat_dlist;
   static GLuint _pole_dlist;
   static GLuint _sign_dlist;
   static GLuint _tv_dlist;
   static GLuint _screen_dlist;

   static bool   _lists_inited;

   Point3D       _r_eye_scale;
   Point3D       _l_eye_scale;

public:

   static t_CKUINT   _dAgentStyle; 
   static bool   _debug; 
   static Closet * closet(); 
   static void init_gl();
   friend class Skin;

   //   Crowd* cy;
   Skin * skin;
   Color4D rgb;
   Color4D def_rgb;
   Color4D dresscol;
   Color4D defcol;
   Color4D skincol;

   bool inbounds;

   int  changeskin;
   bool blink;
   double sincelastBlink;
   
   Point3D pos, lastpos;
   Point3D vel;



   bool left_down;      //which foot is down

   double stride_fore;  //how far forward a step is taken
   double stride_back;  //how far back a step is lifted


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
   //Agent*	following;
   //int   num_followers;
   //Agent**	followers;
   //int foll_size;
   
   float speed;

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
   void calc_feet(Point3D xlate);
   void init();
   //void remove_all_followers();
   //   void remove_follower(Agent* follower);
   //void add_follower   (Agent* follower);
   void fix_state();
   void update(double timeStep);
   void draw();

   void draw_debug();
   void draw_legs(GLdouble * mmtx);
   void draw_body(GLdouble * mmtx);
   void draw_arms();
   void draw_head();

   void printme(FILE* fp);
   void print_pov(FILE* fp);
   //   virtual  gtype isa()    { return AGENT;  }
};

class CoAvatar { 
public:
    Agent * m_agent; 
    CoSession * m_session;
    CoServer * m_server;
    CoWindowManager m_wm;
    void render();
};

#endif //__COAUDICLE_AVATAR_H_INCLUDED__
