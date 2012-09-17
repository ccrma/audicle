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
// name: coaudicle_avatar.cpp
// desc: ...
//
// authors: Philip Davidson (philipd@cs.princeton.edu)
//          Ge Wang (gewang@cs.princeton.edu)
//          Perry R. Cook (prc@cs.princeton.edu)
//          Ananya Misra (amisra@cs.princeton.edu)
// date: Autumn 2005
//-----------------------------------------------------------------------------
#include "coaudicle_avatar.h"
#include "audicle_ui_windowing.h"
#include "coaudicle_ui_coding.h"
//avatar classes



float hat( float f, float freq=1.0) { 
    //f 1.0 - 0.0 
    float ramp = f * freq;
    float rs   = 2 * ( ramp - floor(ramp) );
    float hat  = ( rs < 1.0 ) ? rs : 2.0 - rs;
    return hat;
}

float ramp( float f, float time=1.0 ) { 
    return min ( 1.0 ,  hat(1.0-f) / time );
}

float peak( float f, float mid ) {
    mid = 1.0 - mid; //our value is decreasing...
    return ( f < mid ) ? f / mid : ( 1.0 - f ) / (1.0-mid);  
}


CoAvatar::CoAvatar() :
    m_agent(NULL),
    m_agent_tmp(NULL),
    m_skin( NULL ),
    m_session(NULL),
    m_wm(NULL) 
    {}



// Look, these two functions automatically assure
// that neither m_agent nor m_skin are null!  hurrah!

Agent * 
CoAvatar::agent() {  // only call after GL init!!!
    if ( m_agent ) {
        if ( m_agent_tmp ) return m_agent_tmp;
        return m_agent;
    }
    //initialize
    m_agent = new Agent();
    skin()->set_walk( AGENT_LEGS_JOINTED);

    if ( m_agent_tmp ) return m_agent_tmp;
    return m_agent;
}

void
CoAvatar::push_agent( Agent * a) 
{ 
    agent();//make sure he's around before we assign;
    if ( m_agent_tmp != NULL) { 
        EM_log (CK_LOG_SYSTEM, "error! agent push is fake!" );
    }
    m_agent_tmp = a; 
}

void
CoAvatar::pop_agent( ) { 
    m_agent_tmp = NULL; 
}

Skin *
CoAvatar::skin() { 
    if ( m_skin ) return m_skin;
    m_skin = agent()->skin;
    return m_skin;
}

void
CoAvatar::pre_render() { 
    glPushAttrib ( GL_LIGHTING_BIT );
    glEnable(GL_COLOR_MATERIAL);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 1.0f);
    glColorMaterial(GL_FRONT_AND_BACK, GL_SPECULAR);
    glColor3f(0.8f, 0.8f, 0.8f);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
}

void 
CoAvatar::render_windowmanager() { 
    //windows should know to change rendering method
    if ( !wm() ) return ;
}

void
CoAvatar::apply_window_transform() { 

    //get a view out front where we'll project
    //the user's windows, gfx environment. 

    //this will lock to the body-center
    //for fanciness, we'll allow the user's view-point
    //to follow his cursor position? 

    float env_forward_offset = 10.0; 
    float env_height = 10.0 ; //center of our view.
    float env_scale  = 0.5 * env_forward_offset ; //scaling of view

    GLdouble vmtx[16];

    Vec3D up( 0, 1, 0 );
    up.fillMatrix( vmtx, vel(), pos());
    glMultMatrixd(vmtx);
    /*
    glBegin(GL_LINES);

    glColor3d ( 1,0,0 );
    glVertex3d( 0, 0, 0 );
    glVertex3d( 0, 0, env_forward_offset ); 
    glColor3d ( 0,1,0 );
    glVertex3d( 0, 0, env_forward_offset );
    glVertex3d( 0, env_height, env_forward_offset );
    glEnd(); //GL_LINES
    */

    glTranslated ( 0, env_height, env_forward_offset );
    glRotated ( 180, 0, 1, 0 );
    /*
    glColor3d( 0, 1, 1 );
    glBegin(GL_LINES);
    glVertex3d( 0, 0, 0 );
    glVertex3d( 1, 0, 0 );

    glColor3d( 1, 0, 1 );
    glVertex3d( 0, 0, 0 );
    glVertex3d( 0, 1, 0 );
    glEnd();
    */
    glScaled( env_scale, env_scale, env_scale );


}

void
CoAvatar::post_render() {

    glDisable(GL_COLOR_MATERIAL);
    glPopAttrib();

}

void 
CoAvatar::render() { 
    pre_render();
    skin()->draw_agent(agent());
    skin()->draw_bindle(agent());
    post_render();
}

void
CoAvatar::render_character() { 
    pre_render();
    skin()->draw_agent(agent());
    post_render();

}

void
CoAvatar::render_bindle() { 
    pre_render();
    skin()->draw_bindle(agent());
    post_render();
}

void
CoAvatar::render_halo() { 
    pre_render();
    skin()->draw_halo(agent());
    post_render();
}

void
CoAvatar::render_sign() { 
//  pre_render();
//  skin()->draw_sign(agent());
//  post_render();
}

Point3D& 
CoAvatar::pos() { return agent()->pos; } 

Vec3D& 
CoAvatar::vel() { return agent()->vel; } 

void
CoAvatar::animate ( t_CKFLOAT dtime ) { 
    agent()->update(dtime);
}

void CoAvatar::animate_state( t_CKFLOAT dtime )
{
    //update user pos,
    //possibly from their session data
    vel() =  Point3D ( 0, 0, 0.0 );
    pos() += vel() * dtime;

    agent()->calc_feet(Point3D(0,0,0));
}


Agent::Agent()
{
    init();
}

t_CKFLOAT arand( ) { return rand() / (t_CKFLOAT)RAND_MAX ; }

t_CKFLOAT Rand () { return arand(); } 

t_CKFLOAT Rand_in( t_CKFLOAT a, t_CKFLOAT b )
{ 
    return (t_CKFLOAT)a + Rand() * (t_CKFLOAT)(b - a); 
} 


Closet* Agent::_closet = NULL;

t_CKUINT   Agent::_dAgentStyle = AGENT_NORMAL;
bool   Agent::_debug = false;


double Agent::_speedmul         = 1.0; //0.5
double Agent::_leglen         = 3.0; //0.5
double Agent::_kneelevel      = 0.55; //2.7
double Agent::_steprate       = 0.5; //0.5
double Agent::_walkheight     = 2.7; //2.7
double Agent::_feet_up        = 1.5; //2.7
double Agent::_stepcenter     = -0.15; //-0.15
double Agent::_bounce_base    = 0.15; //0.15
double Agent::_bounce_mag     = 0.8;  //0.8
double Agent::_wobble         = 10.0;

double Agent::_dcomplexity    = 6.0;

void Agent::init(){
    
   int i;

   SELF = 0;
   blink = false;
   sincelastBlink = 0;
   inbounds = false;
   width = Rand_in(0.0, 0.8) + 0.2f;
   step = 0.0f;
   spinAngle = -150.0;

//   spinAngle = Rand_in(0.0, 360.0);
   
//   up = Point3D ( Rand_in(-1,1), Rand_in( 0, 1 ), Rand_in(-1, 1) );
   up= Point3D( 0, 1, 0 );
   up.normalize();
   skin = closet()->getSkin("default");

   changeskin = -1;
    
    headangle=0.0f;
    moveSpeed   = Rand_in(6.0, 9.0);

    _l_eye_scale = Point3D(Rand_in(1.1,0.8),Rand_in(1.1,0.8),1);
    _r_eye_scale = Point3D(Rand_in(1.1,0.8),Rand_in(1.1,0.8),1);

    l_armangle  =   90.0f;
    r_armangle  =   90.0f;
    swingspeed  =   Rand_in( 5.0, 9.0);
    swingphase  =   0; // EAT_PI * 2.0  * myRandf(1.0);
    


    lean_back_time = 1.0;
    arms_up_time = 0.5;
    arm_up_time = 0.5;
    head_bang_time = 2.0;
	walk_tall_time = 2.0;
    jump_time = 2.0;

    lean_back = 0;
    arms_up = 0;
    arm_up = 0;
    head_bang = 0;
    jump = 0;
	walk_tall = 0;

	height_mul = 1.0;
    shoulder_rotate = 0;
    bend_forward = 0;
    bend_side = 0;
    waist_turn = 0;
    jump_height = 0;
    arm_swing_mag = 20.0;
   
    holding_sign=   false;
    holding_tv  =  false;

   speed = moveSpeed;
    sign_tex    =   0;

    for ( i=0; i < 3; i++ ) { 
        defcol[i]=0;
        dresscol[i]=0;
    }
    defcol[3]=1;
    dresscol[3]=1;
    skincol[3]=1;

//    hsl2rgb( Rand_in ( 0.15, 0.25 ) , Rand_in ( 0.0, 0.3 ) , Rand_in ( 0.4, 1.0 ) , skincol[0],skincol[1], skincol[2] );  
    skincol = Color4D( 0.95, 0.95, 0.80, 1.0 );
    
    dresscol = Color4D( 1,1,1,1 );
//    pos = Point3D ( Rand_in(-100.0, 100 ) , Rand_in( 0, 20.0) , Rand_in(-100, 100.0) );
    pos = Point3D( 0, 0, 0 );
   //pos.printnl();
    yvel = 0;
    yfrc = 0;
   /*
   //physics bot???
   base = new Particle( cy->field->crowdmesh, pos, Point3D(0,0,0), 10, TC );
   cy->field->crowdmesh->new_vert(base);
   head = (Particle*)base->sprout_edge();
   head->mass() *= 0.05; //0.15
   upe = head->shared_edge(base);
   upe->rest_len() = 10;

   head->pos() = base->pos() \
                  + Point3D ( 0, 10, 0) \
                  + Point3D( Rand_in(-1, 1),Rand_in(-1, 1),Rand_in(-1, 1)) * 5;
   head->vel() *= 0;
   base->vel() *= 0;
   */

   vel = Point3D (sin(spinAngle*EAT_PI/180.0) * moveSpeed, 0, cos(spinAngle*EAT_PI/180.0) * moveSpeed);
   
   frc = Point3D (0,0,0);
   
   rhip = Point3D (  0.66 , 0 , 0);
   lhip = Point3D ( -0.66 , 0 , 0);

   left_down = true;

   rlastdown = 0;
   llastdown = 0;

   lfoot     = lhip + pos;
   rfoot     = rhip + pos;

   llastpos  = lfoot;
   rlastpos  = rfoot;

   rdir = vel;
   ldir = vel;

   rlastdir = rdir;
   llastdir = ldir;

   def_rgb[0]   = Rand_in( 0.60, 0.80 );
   def_rgb[2]   = def_rgb[1] = def_rgb[0];
   
   rgb = def_rgb;


   /*   
   following= NULL;
   num_followers=0;
   foll_size= 1;
   followers= new Agent*[foll_size];
   for ( i=0; i < foll_size; i++ ) followers[i] = NULL;
   */
}


Closet * 
Agent::closet() { 
    if ( _closet ) return _closet; 
    check_gl_err();
    _closet = new Closet();
    check_gl_err();
    fillCloset();
    check_gl_err();
    return _closet;
}

void
Agent::fillCloset() { 
    _closet->addSkin(  new DefSkin(), "default" ); 
    check_gl_err();
}


/* 
void Agent::add_follower(Agent* f) { 
    //only grow follower_array
    //fprintf ( stderr,"add follower\n");
    while ( num_followers+1 > foll_size ) {
        foll_size *= 2;
        Agent** tmp = new Agent*[foll_size];
        //copy into new array
        for ( int i = 0 ; i < num_followers ; i++ ) tmp[i] = followers[i];
        delete [] followers;
        followers = tmp;
    }
    followers[num_followers++] = f;
    f->following = this;

}

void Agent::remove_follower(Agent* f) { 
    //fprintf( stderr, "remove follower\n" );
    int i;
     f->following = NULL;
    for ( i = 0 ; i < num_followers && followers[i] != f; i++) ;
    if  ( i < num_followers ) followers[i] = followers[--num_followers]; 
    
}

void Agent::remove_all_followers() { 
    //fprintf( stderr, "remove all followers\n");
    for ( int i = num_followers-1; i >=0 ; i-- ) remove_follower ( followers[i] );
}

*/

void Agent::fix_state() {

        //set vars consistent
        //moveSpeed = sqrt ( vel[0]*vel[0] + vel[2]*vel[2] );
        
        

}

void Agent::print_pov(FILE*fp) { 
      fprintf(fp, "<%f, %f, %f>,", pos[0], pos[1], pos[2]);
      fprintf(fp, "\t<%f, %f, %f>,", vel[0], vel[1], vel[2]);
      fprintf(fp, "\t<%f, %f, %f>,", rgb[0], rgb[1], rgb[2] );
      fprintf(fp, "\t%f," , spinAngle);
      fprintf(fp, "\t%f," , headangle);
      fprintf(fp, "\t%f," , swingphase);
      fprintf(fp, "\t%f," , r_armangle);
      fprintf(fp, "\t%f," , l_armangle);
      fprintf(fp, "\t%d," , holding_sign);
      fprintf(fp, "\t%d," , sign_tex);
      fprintf(fp, "\t%f," , sign_width);
      fprintf(fp, "\t%f," , sign_height);
      fprintf(fp, "\n");
}
void Agent::printme(FILE*fp) 
{ 
         fprintf(fp, "<agent>\n");
         fprintf(fp, "\t<id>%ld</id>\n", (t_CKUINT)this); //yeah, so what?
         fprintf(fp, "\t<pos>%f %f %f</pos>\n", pos[0], pos[1], pos[2]);
         fprintf(fp, "\t<vel>%f %f %f</vel>\n", vel[0], vel[1], vel[2]);
         fprintf(fp, "\t<color>%f %f %f</color>\n", rgb[0], rgb[1], rgb[2] );
         fprintf(fp, "\t<sign>%d</sign>\n", holding_sign);
     //         fprintf(fp, "\t<following>%u</following>\n", (unsigned int)following); //yeah, so what
         if ( holding_sign ) {
            fprintf(fp, "\t<sign>\n");
            fprintf(fp, "\t\t<signtex>%d</signtex>\n", sign_tex);
            fprintf(fp, "\t\t<sign_width>%f</sign_width>\n", sign_width);
            fprintf(fp, "\t\t<sign_height>%f</sign_height>\n", sign_height);
            fprintf(fp, "\t</sign>\n");
         }
         fprintf(fp, "\t<tv>%d</tv>\n" , holding_tv);
         fprintf(fp, "\t<headangle>%f</headangle>\n" , headangle);
         fprintf(fp, "\t<r_armangle>%f</r_armangle>\n" , r_armangle);
         fprintf(fp, "\t<l_armangle>%f</l_armangle>\n" , l_armangle);
         fprintf(fp, "\t<swingphase>%f</swingphase>\n" , swingphase);
         fprintf(fp, "</agent>\n");
}



bool Agent::test_collision(Agent * b, double dt ) { 
            //dt 6.0;

            Point3D isectpt;
            Point3D target      = pos    + vel    * dt; 
                Point3D b_target    = b->pos + b->vel * dt; 
            Point3D steer;
       
            if(_debug) { 
                  glColor4d(0,0,1, 0.5);
                  glBegin(GL_LINES);
                  glVertex3dv(pos.data());
                  glVertex3dv(target.data());
                  glVertex3dv(b->pos.data());
                  glVertex3dv(b_target.data());
                  glEnd();
            }

            //this isn't right.. we just check the lines themselves.
            //should really make a box out of each and test all the lines of
            //the boxes

            if (  Point3D::isect2d        (pos, target, b->pos, b_target, isectpt )    \
               || pos.nearest_to_segment  (b->pos, b_target, &isectpt) < 7             \
               || b->pos.nearest_to_segment(pos, target, &isectpt )  < 7               \
               || target.nearest_to_segment  (b->pos, b_target, &isectpt) < 7             \
               || b_target.nearest_to_segment(pos, target, &isectpt )  < 7               \
               // || Point3D::isect2d_rad ( target, b_target, 8, isectpt          ) 
               ) { 
 
               
               if(_debug) { 
                     glPointSize(12);
                     glColor4d(1,0,0, 0.8);
                     glBegin(GL_POINTS);
                     glVertex3dv(isectpt.data());
                     glEnd();
               }

               //time for each to reach intersect point

               // sqrt(x2)/sqrt(y2)  = sqrt ( x2 / y2 );
               
               //this doesnt account for direction, but the line segments we test above
               //are from current (forward) velocity

               //intersect means that from a _forward intersects with from b _forward

               double ta   =  sqrt ( (isectpt - pos   ).len2()/vel.len2() );
                    double tb   =  sqrt ( (isectpt - b->pos).len2()/b->vel.len2() );
                    
//               double dt   =   fabs(ta-tb);

                    //if ( dt < 2.0 ) { 

                        //okay, we have a collision ( both cross intersection point within 1 sec)

                       //treat them by who is the first to reach crossing point
                    
               Agent * first = (ta     <   tb  ) ? this : b;
                    Agent * last  = (first ==  this ) ? b : this;

               Point3D ltof  = first->pos - last->pos;
               
               double first_t = ( first == this ) ?  ta : tb ;                  
               double last_t  = ( last  == this ) ?  ta : tb ;
                  //reaction time...

               first->avoid( last->pos,   last->pos,   ltof,   first_t  );
               last ->avoid( first->pos,  first->pos, -ltof,   last_t   );
   
               /*
                  double last_t = ( last == this ) ? ta : tb ; 
                  
                  //if one doesn't see the other ( 180 degree ),
                  //it won't anticipate a collision, and cannot avoid
                  
                  //last should always see this happening.

                  bool first_reacts = ( (-ltof).dot_xz( first->vel )  > 0 );
                  bool last_reacts  = ( ( ltof).dot_xz( last->vel  )  > 0 );
                        
                  
                  if ( last_reacts ) { //does last see the collision?
                     
                     reaction_time     = ( last == this ) ?  ta : tb ;
                          reaction_time     = max ( 0, reaction_time - ref);
                  
                     if ( reaction_time > 0 ) { 

                        steer = Point3D ( - last->vel[2], 0,  last->vel[0] ) ;
                        //steer points away
                        if ( steer.dot_xz( isectpt - last->pos ) > 0 ) steer *= -1;

                        //change direction away from intersection point
                        double f = 2;
                        //max ( 0,  1 + 0.5 * ( 4.0 - reaction_time ));
                        if ( _debug ) {
                              glColor4d(1,1,0, 0.4);
                              glBegin(GL_LINES);
                              glVertex3dv( last->pos.data());
                              glVertex3dv((last->pos+steer*f).data());
                              glEnd();
                        }
                        last->frc += steer * f;

                     }
                        }

                  //first might not see if last is 'behind' them..
                  if ( first_reacts ) { //does first see this collision?
                     
                     reaction_time = ( first == this ) ? ta : tb ;
                          reaction_time = max ( 0, reaction_time - ref);
                    
                     if ( reaction_time > 0 ) {
                        
                        steer = Point3D ( - first->vel[2], 0, first->vel[0] );
                        //steer points away from intersection
                               if ( steer.dot_xz( isectpt - first->pos ) > 0 ) steer *= -1; 

                        //change direction away from the other
                        //to head them off
                        double f = 2; //max ( 0, 1 + 0.5 * ( 4.0 - reaction_time )) ;                   
                        first->frc += steer * f;
                     
                     }
                  }
               */
                    //}
                }

   return true;
}

bool Agent::avoid(Point3D spot, Point3D cause, Vec3D away, double time ) { 

   if ( vel.dot( cause - pos ) < 0 ) return false; //not seen!
   
   double stopvsturn = 0.33;

   Point3D fresp;
   
   //time until the event happens

   if( time < stopvsturn ) {  //stop
      fresp = vel * -0.2;
   }
   else {                     //turn  
      Vec3D turn = Point3D (-vel[2] , 0, vel[0]);
      //turn away from this!;
      if ( turn.dot(away) < 0 ) turn *= -1;
      double fm = 0.3 + 0.3 * max ( 0, 3.0 - time * 1.2 ) ;

      fresp = turn * fm;
   }

   if ( _debug ) {
      glBegin(GL_LINES);
      glColor4d(1,1,0, 0.4);
      glVertex3dv( pos.data());
      glVertex3dv((pos+fresp).data());
      glColor4d(0,1,1, 0.7); 
      glVertex3dv( (pos +Point3D(0,0.2,0)).data());
      glVertex3dv( (spot+Point3D(0,0.2,0)).data());
      glColor4d(0,1,0, 0.3); 
      glVertex3dv( (spot+Point3D(0,0.2,0)).data());
      glVertex3dv( (cause+Point3D(0,0.2,0)).data());
      glEnd();
   }

   frc += fresp;

   return true;
                 
}



void 
Agent::use_skin(Skin * s) { 
   skin = s;
   if ( s) { 
      rhip = s->rhip();
      lhip = s->lhip();
   }
   else { 
      rhip = Point3D (  0.66 , 0 , 0);
      lhip = Point3D ( -0.66 , 0 , 0);
   }
}

void Agent::calc_body_transform( int walkmode ) { 

   Point3D body_base = pos;

   if ( walkmode != AGENT_LEGS_NONE ) {
      double bounce =  1.0 * ( _bounce_base +                                                       \
                               _bounce_mag * max(0.0, (speed-moveSpeed)/moveSpeed ) )   \
                                                * fabs(sin( (stepphase - 0.25) * EAT_PI )); 
      Point3D vpos = Point3D(0, bounce + _walkheight * height_mul, 0 );
      body_base += vpos;
   }

   //((ag->head->pos()-ag->base->pos())* 0.1 ).fillMatrix(mmtx, ag->vel, body_base, SQUASH_VOLUME);
   Point3D(0,1,0).fillMatrix(transform, vel, body_base, SQUASH_VOLUME);
    
}

void Agent::calc_feet(Point3D xlate) {
   
   Vec3D left  = Vec3D(  vel[2], 0 , -vel[0]).norm();
   Vec3D fwd   = vel.norm();
   Vec3D up    = Vec3D(0,_feet_up,0);

   //simple cyclic feets

   /*
   lfoot =  pos \
            + left   * ( lhip[0] - 0.2) \
            + fwd    * ( lhip[2] + cos(swingphase) )  \
            + up     * max (0, -sin(swingphase)) ;
   rfoot =  pos \
            + left   * ( rhip[0] + 0.2 ) \
            + fwd    * ( rhip[2] + cos(swingphase+EAT_PI) ) \
            + up     * max (0, -sin(swingphase+EAT_PI)) ;

   stepphase = swingphase / EAT_PI;

   */

   //do something fancy schmancy!
   //for fancy footwork!

// Point3D tvel = vel;
   Point3D tvel = vel * _speedmul;

   double speed    = tvel.length();

   stride_fore = _stepcenter + _steprate * speed * 0.5 * height_mul;
   stride_back = _stepcenter - _steprate * speed * 0.5 * height_mul;

   double plant_k =  stride_fore;
   double raise_k =  stride_back;
   
   
   if ( left_down ) { 
      lfoot += xlate;
      //left is down, right is recovering
      Vec3D foot_rel = lfoot-pos;
      double vpos = fwd.dot(foot_rel);
      
      double w = ( vpos - llastdown ) / ( raise_k - llastdown );
      //w varies from 0-just planted - to 1 - must lift!
      Point3D r_target = fwd * ( rhip[2] + plant_k ) + left * ( rhip[0] + 0.2 );  //!relative
      
      double cw = min(1, max(0, w)); //clamp!
      Vec3D r_rel = rlastpos.interp(r_target, cw ) + up * (1.0-cw) * sin ( cw * EAT_PI );
      
      rfoot = pos + r_rel;
      rdir  = rlastdir.interp(fwd, cw);

      if ( w > 1.0 ) { 
         left_down = false;
         rlastdown = fwd.dot(r_rel);
         if ( rlastdown == raise_k ) rlastdown += 1.0;
         llastpos = lfoot - pos;
         llastdir = ldir;
      }
      stepphase = w;
   }

   else { 
      //right down, left is recovering
      rfoot += xlate;
      Vec3D foot_rel = rfoot-pos;
      double vpos = fwd.dot(foot_rel);
      
      double w = ( vpos - rlastdown ) / ( raise_k - rlastdown );
      //w varies from 0-just planted - to 1 - must lift!
      Point3D l_target = fwd * ( lhip[2] + plant_k ) + left * ( lhip[0] - 0.2 );  //!relative      

      double cw = min(1, max(0, w)); //clamp!
      Vec3D l_rel = llastpos.interp(l_target, cw ) + up * (1.0-cw) * sin ( cw * EAT_PI );
      lfoot = pos + l_rel;
      ldir  = rlastdir.interp(tvel, cw);

      if ( w > 1.0 ) { //complete step 
         left_down    = true;
         llastdown   = fwd.dot(l_rel);
         if ( llastdown == raise_k ) llastdown += 1.0;
         rlastpos    = rfoot - pos;
         rlastdir    = rdir;
      }

      stepphase = 1.0+w;
   }

   swingphase = (stepphase + 0.5) * EAT_PI;
}

void Agent::calc_secondary_frc() { 
  /*
   base->align(head, Point3D(0,1,0)); //face up
   head->drag_frc(0.02);
   upe->linear_springf(base, head); //length, force the head to tag along
   base->force() *= 0.0;
  */
}



void Agent::do_lean_back() { lean_back = 1.0; }
void Agent::do_arms_up() { arms_up = 1.0; }
void Agent::do_arm_up() { arm_up = 1.0; }
void Agent::do_head_bang() { head_bang = 1.0;}
void Agent::do_head_bang_once() { head_bang = 0.25;}
void Agent::do_walk_tall() { walk_tall = 1.0;}
void Agent::do_jump() { jump = 1.0;}


void Agent::pose_home() { 

    jump_height = 0;
    shoulder_rotate = 0;
    bend_forward = 0;
    bend_side = 0;
    waist_turn = 0;
    l_armangle = 120;
    r_armangle = 120;
    arm_swing_mag = 20.0;
	height_mul = 1.0;
}

void Agent::set_lean_back( float dt )
{
    lean_back_time = 2.0;
    if ( lean_back == 0 ) return;
    lean_back = max ( 0, lean_back - dt / lean_back_time );

    bend_forward += -20 * ramp ( lean_back, 0.1f );
    waist_turn   += 50 * peak ( lean_back, 0.2f );
    r_armangle   += -40 * peak ( lean_back, 0.2f );
}

void Agent::set_arms_up( float dt )
{
    arms_up_time = 3.0; 
    arms_up = max ( 0, arms_up - dt / arms_up_time );
    l_armangle += -100 * peak ( arms_up, 0.1f );
    r_armangle += -100 * peak ( arms_up, 0.1f );
}



void Agent::set_arm_up( float dt )
{
    arm_up_time = 3.0;
    arm_up = max ( 0, arm_up - dt / arm_up_time );
    r_armangle += -100 * peak ( arm_up, 0.1f );
}

void Agent::set_head_bang( float dt )
{
    head_bang = max ( 0, head_bang - dt / head_bang_time );
    float phase = head_bang * 4.0 * TWO_PI;
    headangle += 5*head_bang + -ramp(head_bang, 0.1f) * 20 * sin ( phase-0.3 );
    bend_forward += 10*head_bang + -20 * sin( phase );
 }

void Agent::set_jump( float dt )
{

    jump_time = 0.5;
    jump = max ( 0, jump - dt / jump_time );
    jump_height += 6 * peak ( jump, 0.5 );

}

void Agent::set_walk_tall( float dt ) { 
	// fprintf(stderr, "height_mul %f walktall %f\n", height_mul, walk_tall  ); 
    walk_tall_time = 2.0;
    walk_tall = max ( 0, walk_tall - dt / walk_tall_time );
	height_mul = 1.0 + 2.0 * min ( 1.0, 5.0 * peak( walk_tall, 0.5 ) );
}


void Agent::update(double dTimeStep){
  
  //animation
  
  dTimeStep *= _speedmul;

  double blinkpr;
  sincelastBlink += dTimeStep;
  if ( !blink ) { 
    //wait 4 secs, then within 6..
    blinkpr = 0.5 * max ( sincelastBlink - 4.0, 0 );
    if ( 1.0 - Rand()*Rand()*Rand() < blinkpr )  {
      blink = true;
      sincelastBlink=0;
      //fprintf(stderr, "blink\n");
    }
  }
  else { 
    //wait 0.05 sec, then within 0.125
    blinkpr = 8 * max ( sincelastBlink - 0.05, 0 );
    if ( 1.0 - Rand()*Rand()*Rand() < blinkpr ) { 
      blink = false;
      sincelastBlink=0;
      //fprintf(stderr, "open\n");
    }
  }
  
  //        up += ( Point3D (0,1,0) - up ) * 0.02;
  //        up.normalize();
  

  //color
  /*
    if ( following != NULL ) { 
    rgb += ( following->rgb - rgb ) * 0.5 * dTimeStep; 
    }
    else { 
    
    rgb += ( def_rgb - rgb ) * 0.5 * dTimeStep; 
    }
  */
  
  speed = vel.length();
  if ( speed > moveSpeed*2 ) {      
      vel *= moveSpeed*2/speed ;
  }
  else if ( speed > moveSpeed ) { 
      frc += vel * 3*(moveSpeed-speed)/speed ;
  }
    //    else { 
    //      if ( _dAgentStyle != AGENT_TV_HEAD ) frc += vel * 1*(moveSpeed-speed)/speed ;
    //}
    
  
  
  
  // headangle += (myRandf(6)-3.0) * dTimeStep ;
  
  headangle  = 0 + 30 * (speed-moveSpeed)/moveSpeed ;
  //  if ( _dAgentStyle == AGENT_TV_HEAD && holding_sign) { headangle = 10.0; } 
  swingphase += 1.3 * speed * dTimeStep;
    
  if ( swingphase > (100.0 * EAT_PI ) ) swingphase -= (float)(30.0 * EAT_PI);
  


  pose_home();

  set_lean_back(dTimeStep);
  set_head_bang(dTimeStep);
  set_arm_up(dTimeStep);
  set_arms_up(dTimeStep);
  set_walk_tall(dTimeStep);
  set_jump(dTimeStep);

  //update physics
  
  /*
  if ( _dAgentStyle == AGENT_TV_HEAD ) 
    { 
      double fmag = frc.length();
      if ( holding_sign ) { 
    frc /= 1.0 + 10.0*(float)num_followers; //massive
    if ( num_followers > 3 ) frc += vel * -0.15; 
      }
      if ( speed < 0.05 && fmag < 5.0 ) { 
    //static friction   
      }
      else { 
    vel[0] += frc[0] * dTimeStep;
    vel[2] += frc[2] * dTimeStep;
        
    pos[0] += vel[0] * dTimeStep;
    pos[2] += vel[2] * dTimeStep;
      }
    }
  */
  //  else 
  //{ 
  vel[0] += frc[0] * dTimeStep;
  vel[2] += frc[2] * dTimeStep;
  
  pos[0] += vel[0] * dTimeStep;
  pos[2] += vel[2] * dTimeStep;
      //}
  
  
  
  double ground = 0; //sin ( pos[2] * 0.01 ) + 8 * sin ( pos[2] * 0.02 + pos[1] * 0.03 );
  //gravities
  yfrc = 0;
  if ( pos[1] > ground + 0.02 ) yfrc += -96.0;
  yfrc += yvel * -0.01;
  
  yvel += yfrc * dTimeStep;
  
  pos[1] += yvel * dTimeStep;
  
  if ( pos[1] < ground ) { 
    pos[1] = ground;
    if ( yvel < 0 ) yvel *= -0.3;
  }
  

  //base->pos() = pos;
   
 
  if ( changeskin >= 0 ) { 
    use_skin( _closet->curskin() );
    changeskin = -1;
  }
  headangle = min( 30, max(-30, headangle));
  headangle = ( headangle > 30  ) ?  30: headangle ; 
  headangle = ( headangle < -30 ) ? -30: headangle ; 
  Point3D check = pos;
  calc_feet(pos-check);

}

void Agent::draw_debug() { 
    if ( skin ) { 
        skin->draw_dummy();
    }
}

void Agent::draw() {
    if ( skin ) 
        skin->draw_agent(this);
    return;
}

double Skin::complexity() { return Agent::_dcomplexity; }
Point3D Skin::l_eye_scale(Agent * ag) { return ag->_l_eye_scale; }
Point3D Skin::r_eye_scale(Agent * ag) { return ag->_r_eye_scale; }

/*
void Agent::draw() {

    if ( skin ) { skin->draw_agent(this); return ; } 

    double bounce =  1.0 * ( _bounce_base + _bounce_mag * max(0.0, (speed-moveSpeed)/moveSpeed ) ) * fabs(sin( (stepphase - 0.25) * EAT_PI )); 
    Point3D vpos = Point3D(0, bounce+_walkheight, 0 );
    Point3D body_base = pos + vpos;


    GLdouble mmtx[16];

    //    ((head->pos()-base->pos())* 0.1 ).fillMatrix(mmtx, vel, body_base, SQUASH_VOLUME);
    Vec3D up( 0, 1, 0 );
    up.fillMatrix(mmtx, vel, body_base, SQUASH_VOLUME); 
    //mult by mmtx
    draw_body(mmtx);

    //use mmtx for legs
    draw_legs(mmtx);


}
*/

struct StrPair { 
      char key[256];
      char val[256];
};



void Skin::load_meshes(char * mapfile) { 
  /*

   if ( !mapfile ) { fprintf(stderr, "loadmeshes:: mapfile must be specified\n" ); return; }

   VECTOR(StrPair) meshmap;
   VECTOR(StrPair) childlist;

   char meshfile[256];
   
   meshfile[0] = '\0';

   char *r = strrchr(mapfile, '/');
   if (r) { 
      strcpy(meshfile, mapfile);
      meshfile[(r-mapfile)+1] = '\0';
   }
   
   FILE * mapf = fopen( mapfile, "r");

   if (mapf) { 
      //read list from itemmaps;s
      int     tokn = 8;
      char** toks = (char**) malloc ( sizeof(char*) * tokn);
    
      int databsize = 512;
      char * databuf = new char[databsize];
      while ( fgetline(&databuf, &databsize, mapf) != EOF ) {    
          int n = tokenize(databuf, " \t", &toks, &tokn);
         if ( n < 0 ) continue;
         if ( toks[0][0] == '#' ) continue;
         if ( n > 1 ) {
            if ( strcmp(toks[0],"meshfile") == 0 ) { 
                 strcat (meshfile, toks[1]);
              }
            else if ( strcmp(toks[0], "walkmode" ) == 0  ) { 
               if      ( strcmp(toks[1], "simple")   == 0 ) _walkmode = AGENT_LEGS_SIMPLE;
               else if ( strcmp(toks[1], "jointed")  == 0 ) _walkmode = AGENT_LEGS_JOINTED;
               else if ( strcmp(toks[1], "none")     == 0 ) _walkmode = AGENT_LEGS_NONE;
            }
            else { 
               StrPair pr;
               meshmap.push_back(pr);
                 strncpy (meshmap.back().key, toks[0], 255);
               strncpy (meshmap.back().val, toks[1], 255);
               if ( n > 3 ) { 
                  if ( strcmp ( toks[2], "parent" ) == 0 ) { 
                     StrPair ch;
                     childlist.push_back(ch);
                     strncpy ( childlist.back().key, toks[3], 255);
                     strncpy ( childlist.back().val, toks[0], 255);
                  }
               }
            }
         }
      }
      fclose(mapf);
      free(toks);

   }
   else {
      fprintf(stderr, "loadmeshes:: mapfile %s could not be opened\n", mapfile ); 
      return; 
   }

   _meshes.clear();
   t_CKUINT i;
   for ( i=0; i < meshmap.size(); i++ ) { 
      Mesh * m = new Mesh();
      MeshSet ms;
      m->load_mesh_vrml_block(meshfile, meshmap[i].val);
      _meshes.push_back(ms);
      strcpy(_meshes.back().name, meshmap[i].key);
      _meshes.back().m = m;
      //fprintf (stderr,  "search %s  :: p: %d\te: %d\tf: %d\n", meshmap[i].val, m->nverts(), m->nedges(), m->nfaces() );
   }

   for ( i=0; i < childlist.size(); i++ ) { 
      Mesh * parent = mesh ( childlist[i].key );
      Mesh * child  = mesh ( childlist[i].val );
      if ( parent && child ) parent->add_child(child);
      else fprintf(stderr,"error - %s %s must both exist!\n", childlist[i].key, childlist[i].val);
   }

  */


   //stats
   /*
   for ( int a = 0 ; a < _meshes.size(); a++ ) { 
      Mesh* m = _meshes[a].m;
      fprintf (stderr,  "mesh :%s: \n\tp: %d\te: %d\tf: %d\n", _meshes[a].name, m->nverts(), m->nedges(), m->nfaces() );
      fprintf (stderr,  "center :\n");
      m->center().printnl();
      fprintf (stderr,  "xform:\n");
      m->xform().printnl();
   }
   */

}


//Mesh * 
//Skin::mesh(char * name) { 
  //   for ( t_CKUINT i = 0; i < _meshes.size(); i++ ) { 
  //    if ( strcmp ( _meshes[i].name, name ) == 0 ) return _meshes[i].m;
  // }
  // return NULL;
//}

void Skin::build_skin() { 
       
   long int div = (long int ) ( complexity() * 4 );
    
   int i;
    

   _hand_base = Point3D ( 0 , 8 , 0 );
   _sign_base = Point3D ( 0 , 25, 0.5 );

   _walkmode = AGENT_LEGS_NONE;
    
   _body_scale = Point3D(0.5, 0.5, 0.5);

   _arm_scale = Point3D (0.8,1,0.8); //XXX HAKED? 

   _leg_scale = Point3D (0.8,1,0.8);

   _torso_scale = Point3D (0.9, 1.0, 0.7 );

    //glTranslatef( -2, 0, 4.5 );
   _eye_pos = Point3D(2,0,4.5);
   _head_pos = Point3D(0,12,0);

   GLUquadricObj *qobj = gluNewQuadric();


    
   _head_dlist = glGenLists(1);

    glNewList(_head_dlist, GL_COMPILE);
    gluSphere(qobj, 5, div, div);
    glEndList();

   _eye_dlist = glGenLists(1);
    glNewList(_eye_dlist, GL_COMPILE);  
    gluSphere(qobj, 5, div/2, div/2);   
    glEndList();

    
   gluDeleteQuadric(qobj);


    float body_h = 10;
    float body_r1 = 3.0f;
    float body_r2 = 3.6f;

   _body_dlist = glGenLists(1);


    glNewList(_body_dlist, GL_COMPILE);
        glBegin(GL_TRIANGLE_FAN );
        glNormal3d ( 0.0, 1.0, 0.0 );
        glVertex3d ( 0.0, body_h, 0.0 ); 
        for  (i = 0; i <= div; i++ ) { 
            float ang = EAT_PI * 2.0 * i/div;
            glVertex3d ( body_r1 * cos(ang),    body_h, body_r1 * sin(-ang) ); 
        }
        glEnd();
    
        glNormal3d ( 0.0, -1.0, 0.0 );
        glBegin(GL_TRIANGLE_FAN );
        glVertex3d ( 0.0, 0.0, 0.0 ); 
        for  (i = 0; i <= div; i++ ) { 
            float ang = EAT_PI * 2.0 * i/div;
            glVertex3d ( body_r2 * cos(-ang),   0.0,    body_r2 * sin(ang) ); 
        }
        glEnd();
    
    
        glBegin(GL_QUAD_STRIP );
        for  ( i = 0; i <= div; i++ ) { 
            float ang = EAT_PI * 2.0 * (double)i/(double)div;
            glNormal3d ( cos(ang), 0.0, sin(ang) );
            //glTexCoord2d ( i/div, 0 );
            glVertex3d ( body_r2 * cos(ang),    0.0,    body_r2 * sin(ang) ); 
            //glTexCoord2d ( i/div, 1 );
            glVertex3d ( body_r1 * cos(ang),    body_h, body_r1 * sin(ang) ); 
        }
       glEnd();

    glEndList();


   _leg_dlist = glGenLists(1);

   double leg_r1 = 0.8;
   double leg_r2 = 1.0;

   glNewList(_leg_dlist, GL_COMPILE);
        glBegin(GL_TRIANGLE_FAN );
        glNormal3d ( 0.0, -1.0, 0.0 );
        glVertex3d ( 0.0, 0.0, 0.0 ); 
        for  (i = 0; i <= div; i++ ) { 
            float ang = EAT_PI * 2.0 * i/div;
            glVertex3d ( leg_r1 * cos(ang),     0.0, leg_r1 * sin(-ang) ); 
        }
        glEnd();
    
        glNormal3d ( 0.0, 1.0, 0.0 );
        glBegin(GL_TRIANGLE_FAN );
        glVertex3d ( 0.0, 1.0, 0.0 ); 
        for  (i = 0; i <= div; i++ ) { 
            float ang = EAT_PI * 2.0 * i/div;
            glVertex3d ( leg_r2 * cos(-ang),    1.0,    leg_r2 * sin(ang) ); 
        }
        glEnd();
    
    
        glBegin(GL_QUAD_STRIP );
        for  ( i = 0; i <= div; i++ ) { 
            float ang = EAT_PI * 2.0 * (double)i/(double)div;
            glNormal3d ( cos(ang), 0.0, sin(ang) );
            //glTexCoord2d ( i/div, 0 );
            glVertex3d ( leg_r2 * cos(ang),     1.0,    leg_r2 * sin(ang) ); 
            //glTexCoord2d ( i/div, 1 );
            glVertex3d ( leg_r1 * cos(ang),     0,  leg_r1 * sin(ang) ); 
        }
       glEnd();

    glEndList();

   _leglower_dlist = _leg_dlist;


   _legupper_dlist = glGenLists(1);

   glNewList(_legupper_dlist, GL_COMPILE);
        glBegin(GL_TRIANGLE_FAN );
        glNormal3d ( 0.0, -1.0, 0.0 );
        glVertex3d ( 0.0, 0.0, 0.0 ); 
        for  (i = 0; i <= div; i++ ) { 
            float ang = EAT_PI * 2.0 * i/div;
            glVertex3d ( leg_r1 * cos(ang),     0.0, leg_r1 * sin(-ang) ); 
        }
        glEnd();
    
        glNormal3d ( 0.0, 1.0, 0.0 );
        glBegin(GL_TRIANGLE_FAN );
        glVertex3d ( 0.0, 1.0, 0.0 ); 
        for  (i = 0; i <= div; i++ ) { 
            float ang = EAT_PI * 2.0 * i/div;
            glVertex3d ( leg_r1 * cos(-ang),    1.0,    leg_r1 * sin(ang) ); 
        }
        glEnd();
    
    
        glBegin(GL_QUAD_STRIP );
        for  ( i = 0; i <= div; i++ ) { 
            float ang = EAT_PI * 2.0 * (double)i/(double)div;
            glNormal3d ( cos(ang), 0.0, sin(ang) );
            //glTexCoord2d ( i/div, 0 );
            glVertex3d ( leg_r1 * cos(ang),     1.0,    leg_r1 * sin(ang) ); 
            //glTexCoord2d ( i/div, 1 );
            glVertex3d ( leg_r1 * cos(ang),     0,  leg_r1 * sin(ang) ); 
        }
       glEnd();

    glEndList();


   _knee_dlist = glGenLists(1);

   qobj = gluNewQuadric();
   glNewList(_knee_dlist, GL_COMPILE);
       gluSphere(qobj, leg_r1, div/2, div/2);
   glEndList();
   gluDeleteQuadric(qobj);


   _rhip = Point3D (  0.66 , 0 , 0);
   _lhip = Point3D ( -0.66 , 0 , 0);


   _foot_dlist     = _eye_dlist;
   //         glScaled ( 0.15, 0.08, 0.25 );
   //         glTranslated ( 0, 0.75, 3.0 );
//   _foot_scale     = Point3D(0.15, 0.08, 0.25);
   _foot_pos       = Point3D( 0, 0.75, 3.0);
   _foot_scale = Point3D ( 0.3, 0.16, 0.40 );
   _foot_pos       = Point3D( 0, 0.75, 2.0);
   //arm dimensions
    float arm_h     = 5.0;
    float arm_r1    = 1.5;
    float arm_r2    = 2.0;

   _shoulder_base = Point3D (0, 6, 0);
   _axis_distance = 0.5;
    
   
    _arm_dlist = glGenLists(1);
    glNewList(_arm_dlist, GL_COMPILE);
    
    glBegin(GL_TRIANGLE_FAN );
    glNormal3d ( 0.0, -1.0, 0.0 );
    glVertex3d ( 0.0, 0.0, 0.0 ); 
    for  (i = 0; i <= div; i++ ) { 
        float ang = EAT_PI * 2.0 * i/div;
        glVertex3d ( arm_r1 * cos(ang),     0.0,    arm_r1 * sin(-ang) ); 
    }
    glEnd();
    glBegin(GL_TRIANGLE_FAN );
    glNormal3d ( 0.0, 1.0, 0.0 );
    glVertex3d ( 0.0, arm_h, 0.0 ); 
    for  (i = 0; i <= div; i++ ) { 
        float ang = EAT_PI * 2.0 * i/div;
        glVertex3d ( arm_r2 * cos(-ang),    arm_h,  arm_r2 * sin(ang) ); 
    }
    glEnd();
    glBegin(GL_QUAD_STRIP );
    for  (i = 0; i <= div; i++ ) { 
        float ang = EAT_PI * 2.0 * i/div;
        glNormal3d ( cos(ang), 0.0, sin(ang) );             
        glVertex3d ( arm_r2 * cos(ang),     arm_h,  arm_r2 * sin(ang) ); 
        glVertex3d ( arm_r1 * cos(ang),     0.0,    arm_r1 * sin(ang) ); 
    }
    glEnd();
    glEndList();
    
    
    double pole_height = 30;
    double pole_rad = 0.3;
    //formerly dlist4
    
   _pole_dlist = glGenLists(1);
    glNewList(_pole_dlist, GL_COMPILE);
    
    glBegin(GL_QUAD_STRIP );
    for  (i = 0; i <= 4; i++ ) { 
        float ang = EAT_PI * 2.0 * i/4.0;
        glNormal3d ( cos(ang), 0.0, sin(ang) );             
        glVertex3d ( pole_rad * cos(ang),   pole_height,    pole_rad * sin(ang) ); 
        glVertex3d ( pole_rad * cos(ang),   0.0,    pole_rad * sin(ang) ); 
    }
    glEnd();
    glEndList();
    
    
    //double sign_height = 12;
    //double sign_width = 9;

   _sign_dlist = glGenLists(1); 
    glNewList(_sign_dlist, GL_COMPILE);
        glBegin(GL_QUAD_STRIP );
        //double aspect = sign_width/sign_height;
        glNormal3d ( 0, 0, 1 );    
        glTexCoord2d ( 0 , 0 );         
        glVertex3d ( -0.5, 0 , 0  ); 
        glTexCoord2d ( 1 , 0 );         
        glVertex3d ( 0.5, 0  , 0  ); 
        glTexCoord2d ( 0 , 1 );         
        glVertex3d ( -0.5, 1 , 0  ); 
        glTexCoord2d ( 1 , 1 );         
        glVertex3d ( 0.5, 1 , 0  ); 
        glEnd();
    glEndList();
    
   _tvscreen_dlist = glGenLists(1); 
    glNewList(_tvscreen_dlist, GL_COMPILE);
        glBegin(GL_QUADS);
        glTexCoord2d( 0, 0 );
        glVertex3d( -1, -1, 0.03 );

        glTexCoord2d( 1, 0 );
        glVertex3d(  1, -1, 0.03 );
        
        glTexCoord2d( 1, 1 );
        glVertex3d(  1,  1, 0.03 );
        
        glTexCoord2d( 0, 1 );
        glVertex3d( -1,  1, 0.03 );
        glEnd();
    glEndList();
    _tv_dlist = glGenLists(1);

    glNewList(_tv_dlist, GL_COMPILE);
    
        glBegin(GL_QUAD_STRIP);

           glNormal3d(   0, -1,     0       );
           
           glVertex3d( -1,  -1,     0       );
           glVertex3d( -1,  -1,     -1.5    );
           glVertex3d(  1,  -1,     0       );
           glVertex3d(  1,  -1,     -1.5    );
           
           glNormal3d(   1,  0,     0       );
           
           glVertex3d(  1,  1,      0       );
           glVertex3d(  1,  0.6,    -1.5    );

           glNormal3d(   0,  1,     0       );

           glVertex3d( -1,  1,      0       );
           glVertex3d( -1,  0.6,    -1.5    );

           glNormal3d(  -1,  0,     0       );

           glVertex3d( -1,  -1,     0       );
           glVertex3d( -1,  -1,     -1.5    );
        
        glEnd();

      glBegin(GL_QUADS); 
        
         glNormal3d(     0, 0,      -1.0 );
           
         glVertex3d( -1, -1,        -1.5 );
           glVertex3d(  1, -1,      -1.5 );
           glVertex3d(  1,  0.6,    -1.5 );     
           glVertex3d( -1,  0.6,    -1.5 ); 
           
         glNormal3d(  0,  0,     1.0 );
         
         glVertex3d( -1, -1,       0  );    
         glVertex3d(  1, -1,       0  );    
         glVertex3d(  1,  1,     0  );
         glVertex3d( -1,  1,     0  );
           
      glEnd();
      
      glBegin(GL_LINES);
           glVertex3d( 0,       0.7,    -1);
           glVertex3d( 0.5, 1.8,    -1.2);
           glVertex3d( 0,       0.7,    -1);
           glVertex3d( -0.7,    1.6,    -0.8);  
        glEnd();

    glEndList();

    GLubyte* rstripes = (GLubyte*) calloc( 3*8, sizeof(GLubyte) );
    
    for ( i =0; i < 24; i++ ) rstripes[i] = (GLubyte)255;
    rstripes[1] = rstripes[2] = rstripes[4] = rstripes[5] = (GLubyte)0;
    rstripes[7] = rstripes[8] = rstripes[1];
    glGenTextures( 1, &_bindle_stripe_tex );
    // EM_log ( CK_LOG_INFO ,"generating bindle texture id: %d", _bindle_stripe_tex );
    glBindTexture( GL_TEXTURE_1D,  _bindle_stripe_tex );
    glTexParameteri (GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); 
    glTexParameteri (GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glTexImage1D( GL_TEXTURE_1D, 
                  0, 
                  GL_RGB, 
                  8, 
                  0, 
                  GL_RGB, 
                  GL_UNSIGNED_BYTE, 
                  (GLvoid* ) rstripes 
                  );

    free (rstripes);

}

void Skin::set_walk( int i ) { _walkmode = i; }

void Skin::draw_agent(Agent * ag ) { 

   //calculate transform for the body position. 
   ag->calc_body_transform( _walkmode );

   glPushMatrix();
   glTranslated (0 , ag->jump_height, 0 );
   //draw body ( mmtx stored in agent )
   draw_body(ag);
   //use mmtx for legs
   draw_legs(ag);
   glPopMatrix();
   draw_shadow(ag);
}

void Skin::draw_shadow(Agent * ag) { 

   double i;
   glPushAttrib( GL_ENABLE_BIT );
   glEnable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
   

   glPushMatrix(); 
   
      glTranslated( ag->pos[0], ag->pos[1] + 0.08, ag->pos[2] );
      glScaled(4,1,4);
      glBegin(GL_TRIANGLE_FAN); 
      glColor4d(0, 0, 0, 0.6);
      glVertex3d( 0 , 0.0, 0 );
      glColor4d( 0, 0, 0, 0 );
      for ( i=0; i <= 12; i++) { 
         double th = TWO_PI * i/12.0;
         glVertex3d ( _body_scale[0] * cos ( th ), 0.00, _body_scale[2] * sin ( th ) );
      }
      glEnd();

   glPopMatrix();

/*
   glPushMatrix(); 
   
      glTranslated( ag->rfoot[0], 0, ag->rfoot[2] );
      glScaled(2-ag->rfoot[1], 1 , 2-ag->rfoot[1]);
      glBegin(GL_TRIANGLE_FAN); 
      glColor4d(0, 0, 0, 0.6);
      glVertex3d( 0 , 0.04, 0 );
      glColor4d( 0, 0, 0, 0 );
      for (  i=0; i <= 12; i++) { 
         double th = TWO_PI * i/12.0;
         glVertex3d ( _body_scale[0] * cos ( th ), 0.04, _body_scale[2] * sin ( th ) );
      }
      glEnd();

   glPopMatrix();

   glPushMatrix(); 
   
      glTranslated( ag->lfoot[0],   0, ag->lfoot[2] );
      glScaled    (3-ag->lfoot[1],  1, 3-ag->lfoot[1]);
      glBegin(GL_TRIANGLE_FAN); 
      glColor4d(0, 0, 0, 0.6);
      glVertex3d( 0 , 0.04, 0 );
      glColor4d( 0, 0, 0, 0 );
      for ( i=0; i <= 12; i++) { 
         double th = TWO_PI * i/12.0;
         glVertex3d ( _body_scale[0] * cos ( th ), 0.04, _body_scale[2] * sin ( th ) );
      }
      glEnd();

   glPopMatrix();

  */

   glPopAttrib();
}

void Skin::draw_body( Agent *ag ) { 

   glPushMatrix();

     
   glMultMatrixd ( ag->transform );
   //body
   
   glScaled(_body_scale[0],_body_scale[1],_body_scale[2] );

   glRotated ( ag->bend_forward, 1,0,0 );
   glRotated ( ag->bend_side + Agent::_wobble * 0.3 * cos ( ag->swingphase -0.5  ) , 0, 0, 1.0 );
   glRotated ( ag->waist_turn , 0,1,0 );
   
   glColor4dv( ag->dresscol.data() );
   
   glPushMatrix();
   glScaled   ( _torso_scale[0], _torso_scale[1], _torso_scale[2]);
   
   glCallList ( _body_dlist );
   
   glPopMatrix();
   
   draw_head(ag);
   draw_arms(ag);
   
   glPopMatrix();
   
}

void Skin::draw_dummy() { 
      //draw the static model.
  
  glCallList(_body_dlist);
  glCallList(_eye_dlist);
  glCallList(_head_dlist);
  glCallList(_arm_dlist);
  glCallList(_leg_dlist);

}

void Skin::draw_head(Agent *ag) { 
  //      Crowd * cy = ag->cy;
  
  glPushMatrix();
  
  //face
  
  glTranslatef ( _head_pos[0], _head_pos[1], _head_pos[2] );
  glRotated     ( ag->headangle, 1, 0, 0);
  glColor3dv  ( ag->skincol.data());
  
  //NORMAL HEAD
  glCallList    ( _head_dlist );
  
  //eyes
  
  glPushMatrix();
  //glColor3f   ( 0.1, 0.1, 0.1 );
  glColor3f      (0,0,0);
  glTranslatef(  _eye_pos[0], _eye_pos[1], _eye_pos[2] );
  glScalef( 0.15f, 0.15f, 0.15f );
  if ( ag->blink ) glScalef( 1.2f, 0.5f, 1.2f );
  glCallList(_eye_dlist);
  glPopMatrix();
  
  glPushMatrix();                    
  //glColor3f   ( 0.1, 0.1, 0.1 );
  glColor3f      (0,0,0);
  glTranslatef( -_eye_pos[0], _eye_pos[1], _eye_pos[2] );
  glScalef( 0.15f, 0.15f, 0.15f );
  if ( ag->blink ) glScalef( 1.2f, 0.5f, 1.2f );
  glCallList(_eye_dlist);
  glPopMatrix();
  
  
  glPopMatrix();
}

void Skin::draw_halo ( Agent * ag) { 

    glPushAttrib( GL_LIGHTING_BIT );
    glDisable(GL_LIGHTING);
    glPushMatrix();   
        glMultMatrixd ( ag->transform );
        //body   
        glScaled(_body_scale[0],_body_scale[1],_body_scale[2] );
        glRotated ( Agent::_wobble * 0.3 * cos ( ag->swingphase -0.5  ) , 0, 0, 1.0 );
        
        glPushMatrix();
        
            //face
            
            glTranslatef ( _head_pos[0], _head_pos[1], _head_pos[2] );
            glRotated   ( ag->headangle, 1, 0, 0);
            glColor3d ( 0.9, 0.9, 0 );
            
            //NORMAL HEAD
            glTranslatef( 0,6, 0 );

//          glScaled ( 0.2, 0.2, 0.2 );
//          glCallList  ( _head_dlist );

            glRotated ( 90, 1, 0, 0 );
            glScaled ( 4, 4, 2 );
            glutSolidTorus ( 0.1, 1.0, 6, 24 );

        
        glPopMatrix();

    glPopMatrix();
       
    glPopAttrib();
}


void Skin::draw_bindle( Agent * ag)
{ 
   //like drawing the sign..

   float pole_height = 0.85f;
   float pole_angle = -80.0f;
   float bindle_height = 25.0f;

   float bindle_size = 0.8f;
//   if ( bindle ) 
//       bindle_size = 0.7 + bindle->resources().size() * 0.05;
   float bindle_dist = bindle_size * 5.5;

   float arm_swing = ag->arm_swing_mag * cos(ag->swingphase); 

   float swing_amt = 30.0;
   float bindle_swing = cos( ag->swingphase + 1.0);


   glPushMatrix();   
       glMultMatrixd ( ag->transform );
       //body   
       glScaled(_body_scale[0],_body_scale[1],_body_scale[2] );
       glRotated ( Agent::_wobble * 0.3 * cos ( ag->swingphase -0.5  ) , 0, 0, 1.0 );

       //draw_arm 
       glPushMatrix();
                
           glTranslatef ( _shoulder_base[0], _shoulder_base[1] , _shoulder_base[2] );
           glRotatef    ( -ag->l_armangle, 0 ,0, 1);   
           glRotatef    ( arm_swing , 1, 0, 0 );
           glTranslatef     ( 0, _axis_distance, 0 );
   
           glPushMatrix();
   
               glTranslatef ( _hand_base[0] - 1.2 ,_hand_base[1] * 0.55, _hand_base[2] + 1.0 );

               glRotatef ( -0.8 * arm_swing - 30, 1,0,0);

               glRotatef( ag->l_armangle + 5, 0, 0, 1);
               glRotatef( pole_angle , 1, 0 , 0 );
               glTranslatef( 0, 0, 0);
               glColor3f( 0.6f, 0.4f, 0.1f );
               glTranslated ( 0, -7, 0 );
                
               glPushMatrix();
                glScaled( 1.0 , pole_height , 1.0 );
                glCallList(_pole_dlist);
               glPopMatrix();
               
               glPushMatrix();
                   Color4D splane( 0,0,0.3,0.6 );
                   glTranslatef( 0 ,pole_height * bindle_height , 0 );
                   glRotatef( -20 + 1.0 * arm_swing , 1.0 , 0 , 0 );
                   glRotatef( bindle_swing * swing_amt , 0, 1.0, 0 );
                   glTranslatef( 0, 0, -bindle_dist);
                   glScaled ( bindle_size, bindle_size, bindle_size );
                   glColor3f ( 1.0 , 1.0 ,1.0  );
                   glEnable(GL_TEXTURE_1D);
                   glEnable(GL_TEXTURE_GEN_S);
                   glTexGeni( GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR );
                   glTexGendv( GL_S, GL_OBJECT_PLANE, splane.data() );
                   glBindTexture( GL_TEXTURE_1D, _bindle_stripe_tex );
                   glCallList( _head_dlist );
                   glDisable(GL_TEXTURE_GEN_S);
                   glDisable(GL_TEXTURE_1D);

                //glCallList(_bindle_list);
                   
               glPopMatrix();
           glPopMatrix();
       glPopMatrix();   
   glPopMatrix();

}

void Skin::draw_arms(Agent *ag) { 

  //      Crowd * cy  =  ag->cy;
      bool holding_sign = ag->holding_sign;
      bool holding_tv   = ag->holding_tv;
        //right arm
    
        glPushMatrix();
            
        glColor3dv      ( ag->skincol.data() );
        glTranslatef    ( _shoulder_base[0], _shoulder_base[1] , _shoulder_base[2] );
        
        glRotatef   ( ag->l_armangle, 0 ,0, -1);
        
        if ( holding_tv) { 
          glRotatef ( 10.0 * cos(ag->swingphase), 1, 0, 0 );
        }
        else    glRotatef   ( 20.0*cos(ag->swingphase), 1, 0, 0 );
        
        glTranslatef    ( 0, _axis_distance, 0 );
        glPushMatrix();
        glCallList  ( _arm_dlist );
        glPopMatrix();
        
        if (holding_sign) { 
          glPushMatrix();
          
          glTranslatef ( _hand_base[0],_hand_base[1],_hand_base[2] );
          
          glRotatef( ag->l_armangle + 5, 0, 0, 1);
          glTranslatef( 0, 0, 0);
          glColor3f( 0.4f, 0.3f, 0.1f );
          glCallList(_pole_dlist);
          glPushMatrix();
          
          glTranslatef( _sign_base[0], _sign_base[1], _sign_base[2] );
          
          
          glScaled ( ag->sign_width, ag->sign_height, 1.0);
          glColor3f(1.0, 1.0, 1.0);
          
          glEnable(GL_TEXTURE_2D);
          //glBindTexture(GL_TEXTURE_2D, cy->ftex[ag->sign_tex]);
          
          glCallList(_sign_dlist);
          
          glDisable(GL_TEXTURE_2D);
          
          glTranslatef(0 , 0, -0.25 );
          
          //          glColor3dv(cy->dCol_sb.data());
          glColor3d ( 1.0 ,1.0 ,1.0);
          glCallList(_sign_dlist);
          
          glPopMatrix();
          glPopMatrix();
        }
        
        if ( holding_tv ) { 
          glPushMatrix();
          
          glTranslatef ( 12,0,0 );
          
          glRotatef( ag->l_armangle + 5, 0, 0, 1);
          glTranslatef ( 0,-15, 0);
          
          glPushMatrix();
          glColor3f( 0.4f, 0.3f, 0.1f ); 
          glScaled( 1, 1.2, 1 );
          glCallList(_pole_dlist);
          glPopMatrix();
          
          glPushMatrix();
          
          glTranslatef( 0, 40 , 3 );
          glScaled ( 6, 6, 6);
          
          glColor3f( 0.2f, 0.2f, 0.2f );
          glCallList(_tv_dlist);
          
          glColor3f( 1.0f, 1.0f, 1.0f );
          glEnable(GL_TEXTURE_2D);
          glEnable(GL_BLEND);
          //  glBindTexture(GL_TEXTURE_2D, cy->field->videotextures[cy->field->textureSq]->tex());
          glPushMatrix();
          glScaled(0.8, 0.8, 0.8);
          glCallList(_tvscreen_dlist);
          glPopMatrix();
          
          glDisable(GL_TEXTURE_2D);
          glPopMatrix();
          
          glPopMatrix();
        }
        glPopMatrix();
    

        //left arm
        glPushMatrix();
            glColor3dv  (ag->skincol.data());
            glScalef    ( -1, 1, 1);
           glTranslatef ( _shoulder_base[0], _shoulder_base[1] , _shoulder_base[2] );
            glRotatef   ( ag->r_armangle, 0 ,0, -1);
            glRotatef   ( -20.0*cos( ag->swingphase ), 1, 0, 0 );
            glTranslatef    ( 0, _axis_distance, 0 );
         glPushMatrix();
               glCallList   ( _arm_dlist );
           glPopMatrix();
        glPopMatrix();

}

void Skin::draw_legs(Agent *ag) { 

    Matrix bmtx;
    bmtx.readgl(ag->transform);

    Point3D righthip = bmtx.multPosition(ag->rhip);
    Point3D lefthip  = bmtx.multPosition(ag->lhip);

    Color4D pcol(1.0,1.0,1.0,1.0);
    //    Color4D pcol = (ag->holding_sign)? cy->field->col_faces : cy->field->col_edges ; 

    double thighlen = Agent::_leglen * Agent::_kneelevel * ag->height_mul;
    double calflen  = Agent::_leglen * ag->height_mul - thighlen;
    
    GLdouble fmtx[16];

    Point3D rfoot    = ag->rfoot;
    Point3D lfoot    = ag->lfoot;
    Vec3D   vel      = ag->vel;

    //left
    
    //position 
    
    //
    if ( _walkmode == AGENT_LEGS_JOINTED ) { 

       

       Point3D lknee;
       Point3D rknee;
    

       Vec3D straight = lfoot-lefthip;
       Vec3D snorm    = straight.norm();
       double sl      = straight.length();
       if ( sl > thighlen + calflen ) {      
          lknee = lefthip + straight * (thighlen)/(thighlen+calflen);  
       }
       else { 
          double xkpos          = ( ( sl*sl + thighlen * thighlen ) - calflen*calflen ) / (2 * sl) ;
          double xkrad          = sqrt ( thighlen * thighlen - xkpos * xkpos );
          Point3D xkc           = lefthip + straight.norm() * xkpos; //origin on straight segment
          Point3D xkvec         = ( vel - straight.project(vel) ).norm();
          lknee                 = xkc + xkvec * xkrad;
       }

         //reuse temps when doing right foot..
    
       straight = rfoot-righthip;
       sl       = straight.length();
       if ( sl > thighlen + calflen ) {      
          rknee = righthip + straight * (thighlen)/(thighlen+calflen);  
       }
       else { 
          double xkpos          = ( ( sl*sl + thighlen * thighlen ) - calflen*calflen ) / (2 * sl) ;
          double xkrad          = sqrt ( thighlen * thighlen - xkpos * xkpos );
          Point3D xkc           = righthip + straight.norm() * xkpos; //origin on straight segment
          Point3D xkvec         = ( vel - straight.project(vel) ).norm();
          rknee                 = xkc + xkvec * xkrad;
       }


       if ( Agent::_debug ) { 
    
          glBegin(GL_LINES);
             glColor3d(0,1,0);
             glVertex3dv(lefthip.data());
             glVertex3dv(lknee.data());
             glColor3d(1,1,0);
             glVertex3dv(lknee.data());
             glVertex3dv(lfoot.data());
  

             glColor3d(1,0,0);
             glVertex3dv(righthip.data());
             glVertex3dv(rknee.data());
             glColor3d(1,0,1);
             glVertex3dv(rknee.data());
             glVertex3dv(rfoot.data());
          glEnd();
    
       }
       else { 

          //left
          
          glColor3dv(pcol.data());

          (lknee-lefthip).fillMatrix(fmtx, vel, lefthip, SQUASH_CONST);
          glPushMatrix();

            glMultMatrixd(fmtx);
            glCallList ( _legupper_dlist );

          glPopMatrix();

          glPushMatrix();
            glTranslated( lknee[0], lknee[1], lknee[2] );
            glCallList ( _knee_dlist );
          glPopMatrix();

          (lfoot-lknee).fillMatrix(fmtx, vel, lknee, SQUASH_CONST);
          glPushMatrix();

            glMultMatrixd(fmtx);
            glCallList ( _leglower_dlist );

          glPopMatrix();

         
          //right

          glColor3dv(pcol.data());

          (rknee-righthip).fillMatrix(fmtx, vel, righthip, SQUASH_CONST);
          glPushMatrix();

            glMultMatrixd(fmtx);
            glScaled(-1,1,1);
            glCallList ( _legupper_dlist);

          glPopMatrix();

          glPushMatrix();
            glTranslated( rknee[0], rknee[1], rknee[2] );
            glCallList ( _knee_dlist );
          glPopMatrix();

          (rfoot-rknee).fillMatrix(fmtx, vel, rknee, SQUASH_CONST);
          glPushMatrix();

            glMultMatrixd(fmtx);
            glScaled(-1,1,1);
            glCallList ( _leglower_dlist );

          glPopMatrix();


       }
    }
    
    else if ( _walkmode == AGENT_LEGS_SIMPLE ) { 

       if ( Agent::_debug ) { 
          glColor3d(0,1,0);
          glBegin(GL_LINES);
          glVertex3dv(lefthip.data());
          glVertex3dv(lfoot.data());
          glEnd();
       
          glColor3d(1,0,0);
          glBegin(GL_LINES);
          glVertex3dv(righthip.data());
          glVertex3dv(rfoot.data());
          glEnd();
       }
       else { 

          glColor3dv( pcol.data());
          //left leg
          (lfoot-lefthip).fillMatrix(fmtx, vel, lefthip, SQUASH_CONST);
          glPushMatrix();
            glMultMatrixd(fmtx);
            glScaled( 2.5, 1, 2.5 );
            glCallList ( _leg_dlist  );

          glPopMatrix();

          (rfoot-righthip).fillMatrix(fmtx, vel, righthip, SQUASH_CONST);
          glPushMatrix();

            glMultMatrixd(fmtx);
            glScaled(-1, 1, 1 );
            glScaled( 2.5, 1, 2.5 );
            glCallList ( _leg_dlist  );

          glPopMatrix();

       }
    }

    //FEET
    //draw them dancin' shoes!
    double stepphase = ag->stepphase;
    if ( _walkmode != AGENT_LEGS_NONE ) { 
       glColor3d( 0.2, 0.2, 0.2);
    
       //left foot 
       
       Point3D(0,1,0).fillMatrix(fmtx, ag->ldir, lfoot , SQUASH_IDENT );
       glPushMatrix(); 
         glMultMatrixd(fmtx);
         if( stepphase > 1.0 ) { 
            double flop = 20 * sin ( (stepphase-1.0) * TWO_PI );
            glRotated( flop , 1, 0 , 0 );
         }
         glScaled(_body_scale[0],_body_scale[1],_body_scale[2] );
           glScaled( _foot_scale[0],_foot_scale[1],_foot_scale[2]);
         glTranslated( _foot_pos[0], _foot_pos[1], _foot_pos[2]);
         glCallList( _foot_dlist );
       glPopMatrix();

       //right foot 
       Point3D(0,1,0).fillMatrix(fmtx, ag->rdir, rfoot , SQUASH_IDENT );
       glPushMatrix(); 
         glMultMatrixd(fmtx);
         //foot raised - flop a little 
         if( stepphase < 1.0 ) { 
            double flop = 20 * sin ( stepphase * TWO_PI );
            glRotated( flop , 1, 0 , 0 );
         }
         glScaled(_body_scale[0],_body_scale[1],_body_scale[2] );
           glScaled( _foot_scale[0],_foot_scale[1],_foot_scale[2]);
         glTranslated( _foot_pos[0], _foot_pos[1], _foot_pos[2]);
         glCallList( _foot_dlist );
       glPopMatrix();


    }


}




/* 
 

//ToonBot and Meshbout out of commission until we add the Mesh
//class ( disabled in audicle_geometry.cpp ) 
//and find the skins ( at phil's house ) 


void MeshBot::draw_head(Agent * ag) { 
      Crowd * cy = ag->cy;

        glPushMatrix();
    
        //face

            glTranslatef ( _head_pos[0], _head_pos[1], _head_pos[2] );
            glRotated   ( ag->headangle, 1, 0, 0);
            glColor3dv  ( ag->skincol.data());

        //NORMAL HEAD
         glCallList     ( _head_dlist );  
         if ( _screen_dlist ) { 
            glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, cy->field->videotextures[cy->field->textureSq]->tex());
            glCallList  ( _screen_dlist );
            glDisable(GL_TEXTURE_2D);
         }
            //eyes
                    
         Point3D les = l_eye_scale(ag);
         Point3D res = r_eye_scale(ag);
         glColor3f      (1,1,1);
         glPushMatrix();
         glTranslatef(  _eye_pos[0], _eye_pos[1], _eye_pos[2] );
         glScaled( les[0], les[1], 1 );
         if ( ag->blink ) glScalef(1.2, 0.5, 1.2);
         glCallList(_eye_dlist);
         glPopMatrix();
         
         glPushMatrix();                    
         glTranslatef( -_eye_pos[0], _eye_pos[1], _eye_pos[2] );
         glScaled(-res[0], res[1], 1 );
         if ( ag->blink ) glScalef(1.2, 0.5, 1.2);
         glCallList(_eye_dlist);
         glPopMatrix();
         

         glPopMatrix();
}

void MeshBot::build_skin(char *c) { 

//   fprintf(stderr, "meshbot: build\n" );
   if ( c ) { 
      load_meshes(c);
   }
   else { 
      load_meshes("source_models/meshbot/meshy.map");
   }
   glLoadIdentity();
    //body dimension
       
//   long int div = (long int ) ( complexity() * 4 );
    
   int i;


   _body_scale = Point3D(0.5, 0.5, 0.5);
   _torso_scale = Point3D(1,1,1);
   
    
//   float body_h = 10;
//  float body_r1 = 3.0;
//  float body_r2 = 3.6;
    
    //_head_dlist = dLists[1];

   GLuint base_flags    = MESH_DRAW_NO_LISTS | POLY_VERT_SHADED;
   GLuint textured      = POLY_FACE_TCOORD | POLY_DRAW_TEXTURED ;
   GLuint mflags        = 0;
   _head_dlist = glGenLists(1);
    glNewList(_head_dlist, GL_COMPILE);
   if ( mesh("head") )  {
      mflags = (mesh("head")->hasTex())? base_flags | textured : base_flags;
      mesh("head")->draw( mflags );
   }
   glEndList();

   _eye_dlist = glGenLists(1);
    glNewList(_eye_dlist, GL_COMPILE);  
   if ( mesh("eye") )  {
      mflags = (mesh("eye")->hasTex())? base_flags | textured : base_flags;
       if ( mesh("eye") ) mesh("eye")->draw( mflags );
   }
   glEndList();

   if ( mesh("head") ) { 

      _head_pos = mesh("head")->center();
      if ( mesh("eye") ) { 
         _eye_pos = mesh("eye")->center() - mesh("head")->center();
      }
   }
  
//   m->load_mesh_vrml_block("source_meshes/meshy.wrl", "Cylinder" );
   _body_dlist = glGenLists(1);

    glNewList(_body_dlist, GL_COMPILE);
   if ( mesh("body") )  {
      mflags = (mesh("body")->hasTex())? base_flags | textured : base_flags;
       if ( mesh("body") ) mesh("body")->draw(mflags);
   }
   glEndList();
   
   _leg_dlist = glGenLists(1); 
   glNewList(_leg_dlist, GL_COMPILE );
   if ( mesh("leg") )  {
      mflags = (mesh("leg")->hasTex())? base_flags | textured : base_flags;
      if ( mesh("leg") ) mesh("leg")->draw(mflags);
   }
   glEndList();

   _legupper_dlist = glGenLists(1);
   glNewList(_legupper_dlist, GL_COMPILE);
   if ( mesh("leg_upper") )  {
      mflags = (mesh("leg_upper")->hasTex())? base_flags | textured : base_flags;
      mesh("leg_upper")->draw(mflags);
   }
   glEndList();

   _leglower_dlist = glGenLists(1);
   glNewList(_leglower_dlist, GL_COMPILE);
   if ( mesh("leg_lower") )  {
      mflags = (mesh("leg_lower")->hasTex())? base_flags | textured : base_flags;
      mesh("leg_lower")->draw(mflags);
   }
   glEndList();

   if ( mesh("leg") ) { 
      _rhip = mesh("leg")->center() - mesh("body")->center();
      _lhip = Point3D ( -_rhip[0], _rhip[1], _rhip[2]);
   }
   else if ( mesh("leg_upper") ) { 
      _rhip = mesh("leg_upper")->center() - mesh("body")->center();
      _lhip = Point3D ( -_rhip[0], _rhip[1], _rhip[2]);
   }
   else { 
      _rhip = Point3D (  0.66 , 0 , 0);
      _lhip = Point3D ( -0.66 , 0 , 0);
   }


   _foot_pos = Point3D(0,0,0);
   _foot_scale = Point3D(1,1,1);
    //arm dimensions
   
   _foot_dlist = glGenLists(1);
   glNewList(_foot_dlist, GL_COMPILE);
   if ( mesh("foot") )  {
      mflags = (mesh("foot")->hasTex())? base_flags | textured : base_flags;
      mesh("foot")->draw(mflags);
   }
   glEndList();

//   float arm_h    = 5.0;
//  float arm_r1    = 1.5;
//  float arm_r2    = 2.0;
    
   
    _arm_dlist = glGenLists(1);
    glNewList(_arm_dlist, GL_COMPILE);
   if ( mesh("arm") )  {
      mflags = (mesh("arm")->hasTex())? base_flags | textured : base_flags;
      mesh("arm")->draw(mflags);
   }
    glEndList();

   if ( mesh("arm") ) { 
      _shoulder_base = mesh("arm")->center() - mesh("body")->center();
   }

   _arm_scale  =  Point3D( 1,1,1);
   _axis_distance = 0.0;
    
    
    double pole_height = 30;
    double pole_rad = 0.3;
    //formerly dlist4
    
   _hand_base = Point3D ( 0 , 8 , 0 );
   _sign_base = Point3D ( 0 , 25, 0.5 );

   _pole_dlist = glGenLists(1);
    glNewList(_pole_dlist, GL_COMPILE);
      if ( mesh("pole") ) { 
         mflags = (mesh("pole")->hasTex())? base_flags | textured : base_flags;
         mesh("pole")->draw(mflags);
      }
      else { 
          glBegin(GL_QUAD_STRIP );
          for  (i = 0; i <= 4; i++ ) { 
              float ang = PI * 2.0 * i/4.0;
              glNormal3d ( cos(ang), 0.0, sin(ang) );             
              glVertex3d ( pole_rad * cos(ang),     pole_height,    pole_rad * sin(ang) ); 
              glVertex3d ( pole_rad * cos(ang),     0.0,    pole_rad * sin(ang) ); 
          }
          glEnd();
      }
    glEndList();

   if ( mesh("pole") && mesh("arm") ) { 
      _hand_base = mesh("pole")->center() - mesh("arm")->center() ;
   }
    
    
    //double sign_height = 12;
    //double sign_width = 9;
    
   _sign_dlist = glGenLists(1); 
    glNewList(_sign_dlist, GL_COMPILE);
   if ( mesh("sign") ) { 
      mflags = (mesh("sign")->hasTex())? base_flags | textured : base_flags;
      mesh("sign")->draw(mflags);
   }
   else { 
        glBegin(GL_QUAD_STRIP );
        //double aspect = sign_width/sign_height;
        glNormal3d ( 0, 0, 1 );    
        glTexCoord2d ( 0 , 0 );         
        glVertex3d ( -0.5, 0 , 0  ); 
        glTexCoord2d ( 1 , 0 );         
        glVertex3d ( 0.5, 0  , 0  ); 
        glTexCoord2d ( 0 , 1 );         
        glVertex3d ( -0.5, 1 , 0  ); 
        glTexCoord2d ( 1 , 1 );         
        glVertex3d ( 0.5, 1 , 0  ); 
        glEnd();
   }
    glEndList();
   if ( mesh("sign") && mesh("pole") ) { 
         _sign_base = mesh("sign")->center() - mesh("pole")->center() ;
   }
    
   if ( mesh("screen") ) { 
      _screen_dlist = glGenLists(1);    
       glNewList(_screen_dlist, GL_COMPILE);
         mflags = (mesh("screen")->hasTex())? base_flags | textured : base_flags;
         mesh("screen")->draw(mflags);
       glEndList();
   }


   _tvscreen_dlist = glGenLists(1); 
    glNewList(_tvscreen_dlist, GL_COMPILE);
      if ( mesh("tvscreen") ) { 
         mflags = (mesh("tvscreen")->hasTex())? base_flags | textured : base_flags;
         mesh("tvscreen")->draw(mflags);
      }
      else { 
           glBegin(GL_QUADS);
           glTexCoord2d( 0, 0 );
           glVertex3d( -1, -1, 0.03 );

           glTexCoord2d( 1, 0 );
           glVertex3d(  1, -1, 0.03 );
           
           glTexCoord2d( 1, 1 );
           glVertex3d(  1,  1, 0.03 );
           
           glTexCoord2d( 0, 1 );
           glVertex3d( -1,  1, 0.03 );
           glEnd();
      }
    glEndList();

    glNewList(_tv_dlist, GL_COMPILE);
    
        glBegin(GL_QUAD_STRIP);

           glNormal3d(   0, -1,     0       );
           
           glVertex3d( -1,  -1,     0       );
           glVertex3d( -1,  -1,     -1.5    );
           glVertex3d(  1,  -1,     0       );
           glVertex3d(  1,  -1,     -1.5    );
           
           glNormal3d(   1,  0,     0       );
           
           glVertex3d(  1,  1,      0       );
           glVertex3d(  1,  0.6,    -1.5    );

           glNormal3d(   0,  1,     0       );

           glVertex3d( -1,  1,      0       );
           glVertex3d( -1,  0.6,    -1.5    );

           glNormal3d(  -1,  0,     0       );

           glVertex3d( -1,  -1,     0       );
           glVertex3d( -1,  -1,     -1.5    );
        
        glEnd();

      glBegin(GL_QUADS); 
        
         glNormal3d(     0, 0,      -1.0 );
           
         glVertex3d( -1, -1,        -1.5 );
           glVertex3d(  1, -1,      -1.5 );
           glVertex3d(  1,  0.6,    -1.5 );     
           glVertex3d( -1,  0.6,    -1.5 ); 
           
         glNormal3d(  0,  0,     1.0 );
         
         glVertex3d( -1, -1,       0  );    
         glVertex3d(  1, -1,       0  );    
         glVertex3d(  1,  1,     0  );
         glVertex3d( -1,  1,     0  );
           
      glEnd();
        
      
      glBegin(GL_LINES);
           glVertex3d( 0,       0.7,    -1);
           glVertex3d( 0.5, 1.8,    -1.2);
           glVertex3d( 0,       0.7,    -1);
           glVertex3d( -0.7,    1.6,    -0.8);  
        glEnd();

    glEndList();

}

void ToonBot::draw_head(Agent * ag) { 
      Crowd * cy = ag->cy;

        glPushMatrix();
    
        //face

            glTranslatef ( _head_pos[0], _head_pos[1], _head_pos[2] );
            glRotated   ( ag->headangle, 1, 0, 0);
            glColor3dv  ( ag->skincol.data());

        //NORMAL HEAD
         if ( vertex_program_mode ) { 
            if ( ag->holding_sign ) glColor3dv  ( cy->field->col_faces.data());
            else                    glColor3dv  ( cy->field->col_edges.data());
         }
         glCallList     ( _head_dlist );  
         
         if ( _screen_dlist ) { 
            glColor3dv  ( cy->field->col_faces.data());
            glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, cy->field->videotextures[cy->field->textureSq]->tex());
            glCallList  ( _screen_dlist );
            glDisable(GL_TEXTURE_2D);
         }
            //eyes
                    
         Point3D les = l_eye_scale(ag);
         Point3D res = r_eye_scale(ag);
         glColor3f      (1,1,1);
            glPushMatrix();
                glTranslatef(  _eye_pos[0], _eye_pos[1], _eye_pos[2] );
            glScaled( les[0], les[1], 1 );
                if ( ag->blink ) glScalef(1.2, 0.5, 1.2);
                glCallList(_eye_dlist);
            glPopMatrix();

            glPushMatrix();                    
                glTranslatef( -_eye_pos[0], _eye_pos[1], _eye_pos[2] );
            glScaled(-res[0], res[1], 1 );
            if ( ag->blink ) glScalef(1.2, 0.5, 1.2);
                glCallList(_eye_dlist);
            glPopMatrix();
      

        glPopMatrix();
}

void ToonBot::draw_body(Agent * ag, GLdouble * mmtx) { 
      Crowd * cy = ag->cy;
      glPushMatrix();

     
      glMultMatrixd ( mmtx );

      //body

      glScaled(_body_scale[0],_body_scale[1],_body_scale[2] );
        
      if ( ag->holding_sign ) glColor3dv  ( cy->field->col_faces.data());
      else                    glColor3dv  ( cy->field->col_edges.data());

        
      glPushMatrix();

         glScaled   ( _torso_scale[0], _torso_scale[1], _torso_scale[2]);
    
         glCallList ( _body_dlist );
        
      glPopMatrix();
      
        draw_head(ag);
      draw_arms(ag);



   glPopMatrix();
}

*/




