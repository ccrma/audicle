#include "coaudicle_avatar.h"

//avatar classes


Agent::Agent() { 
     init();
}

t_CKFLOAT 
arand( ) { return rand() / (t_CKFLOAT)RAND_MAX ; }

t_CKFLOAT 
Rand () { return arand(); } 

t_CKFLOAT 
Rand_in ( t_CKFLOAT a, t_CKFLOAT b ) { 
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

bool   Agent::_lists_inited   = false;

GLuint Agent::_body_dlist     = 0;
GLuint Agent::_head_dlist     = 0;
GLuint Agent::_eye_dlist      = 0;
GLuint Agent::_arm_dlist      = 0;
GLuint Agent::_hat_dlist      = 0;
GLuint Agent::_pole_dlist     = 0;
GLuint Agent::_sign_dlist     = 0;
GLuint Agent::_tv_dlist       = 0;
GLuint Agent::_screen_dlist   = 0;

void Agent::init(){
	
   int i;


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
	moveSpeed	= Rand_in(6.0, 9.0);

	_l_eye_scale = Point3D(Rand_in(1.1,0.8),Rand_in(1.1,0.8),1);
    _r_eye_scale = Point3D(Rand_in(1.1,0.8),Rand_in(1.1,0.8),1);

	l_armangle	=	90.0f;
	r_armangle	=	90.0f;
	swingspeed	=	Rand_in( 5.0, 9.0);
	swingphase	=	0; // EAT_PI * 2.0 	* myRandf(1.0);
	
	holding_sign=	false;
	holding_tv	=  false;

   speed = moveSpeed;
	sign_tex	=	0;

	for ( i=0; i < 3; i++ ) { 
		defcol[i]=0;
		dresscol[i]=0;
	}
	defcol[3]=1;
	dresscol[3]=1;
	skincol[3]=1;
    hsl2rgb( Rand_in ( 0.15, 0.25 ) , Rand_in ( 0.0, 0.3 ) , Rand_in ( 0.4, 1.0 ) , skincol[0],skincol[1], skincol[2] );
	
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

   def_rgb[0]	= Rand_in( 0.60, 0.80 );
   def_rgb[2]	= def_rgb[1] = def_rgb[0];
   
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
    _closet = new Closet();
    fillCloset();
    return _closet;
}

void
Agent::fillCloset() { 
    _closet->addSkin(  new DefSkin(), "default" ); 
}

void 
Agent::init_gl() { 
   if ( _lists_inited ) return;
   _lists_inited = true;

   long int div = (long int ) ( _dcomplexity * 4 );
	
   int i;

   glLoadIdentity();
	//body dimension
	
	float body_h = 10;
	float body_r1 = 3.0;
	float body_r2 = 3.6;
	
	//_head_dlist = dLists[1];

   GLUquadricObj *qobj = gluNewQuadric();


   _head_dlist = glGenLists(1);
	
	
   glNewList(_head_dlist, GL_COMPILE);
	gluSphere(qobj, 5, div, div);
	glEndList();


   _eye_dlist = glGenLists(1);

   glNewList(_eye_dlist, GL_COMPILE);
	gluSphere(qobj, 5, (int)(div* 0.6), (int)(div* 0.6));
   glEndList();


   gluDeleteQuadric(qobj);

   _body_dlist = glGenLists(1);
   glNewList(_body_dlist, GL_COMPILE);
   
   
   glBegin(GL_TRIANGLE_FAN );
   glNormal3d ( 0.0, 1.0, 0.0 );
   glVertex3d ( 0.0, body_h, 0.0 ); 
   for  (i = 0; i <= div; i++ ) { 
     float ang = EAT_PI * 2.0 * i/div;
     glVertex3d ( body_r1 * cos(ang), 	body_h, body_r1 * sin(-ang) ); 
   }
   glEnd();
   
   glNormal3d ( 0.0, -1.0, 0.0 );
   glBegin(GL_TRIANGLE_FAN );
   glVertex3d ( 0.0, 0.0, 0.0 ); 
   for  (i = 0; i <= div; i++ ) { 
     float ang = EAT_PI * 2.0 * i/div;
     glVertex3d ( body_r2 * cos(-ang), 	0.0, 	body_r2 * sin(ang) ); 
   }
   glEnd();
	
   
   glBegin(GL_QUAD_STRIP );
   for  ( i = 0; i <= div; i++ ) { 
     float ang = EAT_PI * 2.0 * (double)i/(double)div;
     glNormal3d ( cos(ang), 0.0, sin(ang) );
     //glTexCoord2d ( i/div, 0 );
     glVertex3d ( body_r2 * cos(ang), 	0.0,	body_r2 * sin(ang) ); 
     //glTexCoord2d ( i/div, 1 );
     glVertex3d ( body_r1 * cos(ang), 	body_h,	body_r1 * sin(ang) ); 
		}
   glEnd();
   
   
   
   glEndList();
   

   //cone hat
   //was dlist6
   _hat_dlist = glGenLists(1);	
	glNewList(_hat_dlist, GL_COMPILE);
	
	glNormal3d ( 0.0, -1.0, 0.0 );
	glBegin(GL_TRIANGLE_FAN );
	glVertex3d ( 0.0, 0.0, 0.0 ); 
	for  (i = 0; i <= div; i++ ) { 
	  float ang = EAT_PI * 2.0 * i/div;
	  glVertex3d ( body_r2 * cos(-ang), 	0.0, 	body_r2 * sin(ang) ); 
	}
	glEnd();
	//glBindTexture(GL_TEXTURE_2D, textures[0]);
	//glEnable(GL_TEXTURE_2D);
	
	glBegin(GL_QUAD_STRIP );
	for  ( i = 0; i <= div; i++ ) { 
	  float ang = EAT_PI * 2.0 * (double)i/(double)div;
	  glNormal3d ( cos(ang), 0.0, sin(ang) );
	  //glTexCoord2d ( i/div, 0 );
	  glVertex3d ( body_r2 * cos(ang), 	0.0,	body_r2 * sin(ang) ); 
	  //glTexCoord2d ( i/div, 1 );
	  glVertex3d ( 0, 	body_h,	0 ); 
	}
	glEnd();
	
	
	//glDisable(GL_TEXTURE_2D);
	
	glEndList();
	
	
	//arm dimensions
	float arm_h 	= 5.0;
	float arm_r1 	= 1.5;
	float arm_r2 	= 2.0;
	

	_arm_dlist = glGenLists(1);
	glNewList(_arm_dlist, GL_COMPILE);
	
	glBegin(GL_TRIANGLE_FAN );
	glNormal3d ( 0.0, -1.0, 0.0 );
	glVertex3d ( 0.0, 0.0, 0.0 ); 
	for  (i = 0; i <= div; i++ ) { 
		float ang = EAT_PI * 2.0 * i/div;
		glVertex3d ( arm_r1 * cos(ang), 	0.0,	arm_r1 * sin(-ang) ); 
	}
	glEnd();
	glBegin(GL_TRIANGLE_FAN );
	glNormal3d ( 0.0, 1.0, 0.0 );
	glVertex3d ( 0.0, arm_h, 0.0 ); 
	for  (i = 0; i <= div; i++ ) { 
		float ang = EAT_PI * 2.0 * i/div;
		glVertex3d ( arm_r2 * cos(-ang), 	arm_h, 	arm_r2 * sin(ang) ); 
	}
	glEnd();
	glBegin(GL_QUAD_STRIP );
	for  (i = 0; i <= div; i++ ) { 
		float ang = EAT_PI * 2.0 * i/div;
		glNormal3d ( cos(ang), 0.0, sin(ang) );             
		glVertex3d ( arm_r2 * cos(ang), 	arm_h, 	arm_r2 * sin(ang) ); 
		glVertex3d ( arm_r1 * cos(ang), 	0.0,	arm_r1 * sin(ang) ); 
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
		glVertex3d ( pole_rad * cos(ang), 	pole_height, 	pole_rad * sin(ang) ); 
		glVertex3d ( pole_rad * cos(ang), 	0.0,	pole_rad * sin(ang) ); 
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
	
   _screen_dlist = glGenLists(1);	
	glNewList(_screen_dlist, GL_COMPILE);
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

		   glNormal3d(	 0,	-1,		0		);
		   
		   glVertex3d( -1,	-1,		0		);
		   glVertex3d( -1,	-1,		-1.5	);
		   glVertex3d(  1,	-1,		0		);
		   glVertex3d(  1,	-1,		-1.5	);
		   
		   glNormal3d(	 1,	 0,		0		);
		   
		   glVertex3d(  1,	1,		0		);
		   glVertex3d(  1,	0.6,	-1.5	);

		   glNormal3d(	 0,	 1,		0		);

		   glVertex3d( -1,	1,		0		);
		   glVertex3d( -1,	0.6,	-1.5	);

		   glNormal3d(	-1,	 0,		0		);

		   glVertex3d( -1,	-1,		0		);
		   glVertex3d( -1,	-1,		-1.5	);
		
		glEnd();

      glBegin(GL_QUADS); 
		
         glNormal3d(	 0,	0,		-1.0 );
		   
         glVertex3d( -1, -1,		-1.5 );
		   glVertex3d(  1, -1,		-1.5 );
		   glVertex3d(  1,  0.6,	-1.5 );		
		   glVertex3d( -1,  0.6,	-1.5 );	
		   
         glNormal3d(  0,  0,     1.0 );
         
         glVertex3d( -1, -1,	   0  );	
         glVertex3d(  1, -1,	   0  );	
         glVertex3d(  1,  1,     0  );
         glVertex3d( -1,  1,     0  );
		   
      glEnd();
		
      
      glBegin(GL_LINES);
		   glVertex3d( 0,		0.7,	-1);
		   glVertex3d( 0.5,	1.8,	-1.2);
		   glVertex3d( 0,		0.7,	-1);
		   glVertex3d( -0.7,	1.6,	-0.8);	
		glEnd();
	glEndList();


   fprintf(stderr, "Agent :: gl_init()\n");
   fprintf(stderr, "Agent :: -body- %d\n",   _body_dlist);
   fprintf(stderr, "Agent :: -arm- %d\n",    _arm_dlist);
   fprintf(stderr, "Agent :: -head- %d\n",   _head_dlist);
   fprintf(stderr, "Agent :: -hat- %d\n",    _hat_dlist);
   fprintf(stderr, "Agent :: -tv- %d\n",     _tv_dlist);
   fprintf(stderr, "Agent :: -screen- %d\n", _screen_dlist);
   fprintf(stderr, "Agent :: -pole- %d\n",   _pole_dlist);
   fprintf(stderr, "Agent :: -sign- %d\n",   _sign_dlist);
   
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
         fprintf(fp, "\t<id>%u</id>\n", (t_CKUINT)this); //yeah, so what?
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

   stride_fore = _stepcenter + _steprate * speed * 0.5;
   stride_back = _stepcenter - _steprate * speed * 0.5;

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

void Agent::update(double dTimeStep){
  
  //animation
  
  dTimeStep *= _speedmul;

  /*  
  if ( sign_aspect != cy->dAspect[sign_tex] || 
       sign_area   != cy->dSignArea             ) 
    { 
      sign_aspect  = cy->dAspect[sign_tex];
      sign_area    = cy->dSignArea;
      sign_height 	= sqrt( sign_area / sign_aspect );
      sign_width 	= sign_area / sign_height;
    }
  */
  
  l_armangle = 120;
  r_armangle = 120;
  

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
  if ( speed > 0 ) { 
    if ( speed > moveSpeed*2 )		vel *= moveSpeed*2/speed ;
    else if ( speed > moveSpeed ) frc += vel * 3*(moveSpeed-speed)/speed ;
    //    else { 
    //      if ( _dAgentStyle != AGENT_TV_HEAD ) frc += vel * 1*(moveSpeed-speed)/speed ;
    //}
  }
  
  
  // headangle += (myRandf(6)-3.0) * dTimeStep ;
  
  headangle  = 0 + 30 * (speed-moveSpeed)/moveSpeed ;
  //  if ( _dAgentStyle == AGENT_TV_HEAD && holding_sign) { headangle = 10.0; } 
  swingphase += 1.3 * speed * dTimeStep;
	
  if ( swingphase > (100.0 * EAT_PI ) ) swingphase -= (30.0 * EAT_PI);
  
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

void Agent::draw_arms() {

      //right arm

      double armheight = 6;
      double armout    = 0.5;

      /*
      if ( _dAgentStyle >= AGENT_TV_HEAD ) { 
	armheight = 11;
	armout    = 4;
      }
      */
      
      glPushMatrix();
      
      
      glColor3dv  	( skincol.data() );
      glTranslatef	( 0, armheight , 0 );
      
      
      glRotatef	( l_armangle, 0 ,0, 1);
      if ( holding_tv) { 
	glRotatef	( 10.0 * cos(swingphase), 1, 0, 0 );
      }
      else	glRotatef	( 20.0*cos(swingphase), 1, 0, 0 );
      
      glTranslatef 	( 0, armout, 0 );
      glPushMatrix();
      glScaled ( 0.5, 1, 0.5 );
      
      //glScaled ( 0.7, 1.2, 0.7 );
      glCallList 	( _arm_dlist );
      glPopMatrix();
      
      if (holding_sign && _dAgentStyle < AGENT_TV_HEAD) { 
	glPushMatrix();
	glTranslatef ( 12,0,0 );
	
	//glRotatef( -l_armangle+10, 0, 0, 1);
	glRotatef( -l_armangle + 5, 0, 0, 1);
	glTranslatef ( 0,-15, 0);
	glColor3f (0.4, 0.3, 0.1 );
	glCallList(_pole_dlist);
	glPushMatrix();
	
	glTranslatef( 0, 25 , 0.5 );
	//                            glScaled ( 18.0, 12.0, 1.0);
	
	
	glScaled ( sign_width, sign_height, 1.0);
	glColor3f(1.0, 1.0, 1.0);

	//FIX THIS
	
	//glEnable(GL_TEXTURE_2D);
	//glBindTexture(GL_TEXTURE_2D, cy->ftex[sign_tex]);
	
	glCallList(_sign_dlist);
	
	//glDisable(GL_TEXTURE_2D);
	
	glTranslatef(0 , 0, -0.25 );
	
	//	glColor3dv(cy->dCol_sb.data());
	glCallList(_sign_dlist);
	
	glPopMatrix();
	glPopMatrix();
      }
      
      if ( holding_tv && _dAgentStyle != AGENT_TV_HEAD) { 
	glPushMatrix();
	glTranslatef ( 12,0,0 );
	
	//glRotatef( -l_armangle+10, 0, 0, 1);
	glRotatef( -l_armangle + 5, 0, 0, 1);
	glTranslatef ( 0,-15, 0);
	
	glPushMatrix();
	
	glColor3f (0.4, 0.3, 0.1 ); 
	glScaled(1, 1.2, 1 );
	glCallList(_pole_dlist);
	glPopMatrix();
	
	glPushMatrix();
	
	glTranslatef( 0, 40 , 3 );
	//                            glScaled ( 18.0, 12.0, 1.0);
	glScaled ( 6, 6, 6);
	
	glColor3f(0.2, 0.2, 0.2);
	glCallList(_tv_dlist);
	
	glColor3f(1.0, 1.0, 1.0);
	glEnable(GL_TEXTURE_2D);
	//						glBindTexture(GL_TEXTURE_2D, cy->tvtex);
	glEnable(GL_BLEND);
	//	glBindTexture(GL_TEXTURE_2D, cy->field->videotextures[cy->field->textureSq]->tex());
	glPushMatrix();
	glScaled(0.8, 0.8, 0.8);
	glCallList(_screen_dlist);
	glPopMatrix();
	
	glDisable(GL_TEXTURE_2D);
	
	
	glPopMatrix();
	glPopMatrix();
      }
      glPopMatrix();
	

		//left arm
		glPushMatrix();
			glColor3dv	(skincol.data());
			glScalef 	( -1, 1, 1);
			glTranslatef	( 0, armheight , 0 );
			glRotatef	( r_armangle, 0 ,0, 1);
			glRotatef	( -20.0*cos( swingphase ), 1, 0, 0 );
			glTranslatef 	( 0, armout, 0 );
         glPushMatrix();
            glScaled ( 0.5, 1, 0.5 );
            //glScaled ( 0.7, 1.2, 0.7  );
			   glCallList 	( _arm_dlist);
		   glPopMatrix();
		glPopMatrix();
}

void Agent::draw_body(GLdouble * mmtx) {

   glPushMatrix();

     
      glMultMatrixd ( mmtx );


      //body
      
      float sc = 0.5;
      
      
      glScalef ( sc,sc,sc);
      
      
      if ( _dAgentStyle == AGENT_SANTA ) 
	{ 
	  if ( holding_sign ) glScalef(1.2, 1.2, 1.2);
	  else glScalef ( 0.8, 0.8, 0.8 );
	}
      
      /*
      if ( holding_sign ) { 
	glColor3dv  ( cy->field->col_faces.data());
      } 
      else {
	glColor3dv  ( cy->field->col_edges.data());
      }
      */

		//buttons
		if ( _dAgentStyle == AGENT_SANTA ) 
		{ 
		   glCallList ( _body_dlist );
      	glPushMatrix();
				glTranslatef	( 0	, 3.0,	3.4	);
				glScalef			( 0.1	, 0.1,	0.1	);
				glColor3f		( 1.0 , 1.0, 1.0	);
				glCallList(_eye_dlist);
			glPopMatrix();
		
			glPushMatrix();
				glTranslatef	( 0	, 5.0,	3.2	);
				glScalef			( 0.1	, 0.1,	0.1	);
				glColor3f		( 1.0 , 1.0, 1.0	);
				glCallList(_eye_dlist);
			glPopMatrix();
			
			glPushMatrix();
				glTranslatef	( 0	, 7.0,	3.1	);
				glScalef			( 0.1	, 0.1,	0.1	);
				glColor3f		( 1.0 , 1.0, 1.0	);
				glCallList(_eye_dlist);
			glPopMatrix();
			// end buttons		
		}

      else { 
         glPushMatrix();
         glScaled   ( 0.9, 1.0, 0.7 );
         if ( _dAgentStyle >= AGENT_TV_HEAD ) glScaled ( 1, 0.5, 1);
         glCallList ( _body_dlist );
         glPopMatrix();
      }

		
		draw_head();

      draw_arms();



    glPopMatrix();


      //

   
      //draw body 


}

void Agent::draw_head() {
//head
  glPushMatrix();
  
  //face

  glTranslatef 	( 0, 12, 0 );
  glRotated 	( headangle, 1, 0, 0);
  glColor3dv  ( skincol.data());
  
  //  if ( _dAgentStyle != AGENT_TV_HEAD && _dAgentStyle != AGENT_TV_DONLY) 
  //  { 
      //NORMAL HEAD
      glCallList 	( _head_dlist );
      
      //eyes
      
      glPushMatrix();
      glColor3f 	( 0.1, 0.1, 0.1 );
      glTranslatef( 2, 0, 4.5 );
      glScalef 	( 0.15, 0.15, 0.15 );
      if ( blink ) glScalef(1.2, 0.5, 1.2);
      glCallList(_eye_dlist);
      glPopMatrix();
      
      glPushMatrix();                    
      glColor3f 	( 0.1, 0.1, 0.1 );
      glTranslatef( -2, 0, 4.5 );
      glScalef 	( 0.15, 0.15, 0.15 );
      if ( blink ) glScalef(1.2, 0.5, 1.2);
      glCallList(_eye_dlist);
      glPopMatrix();
      //  }
      /*
	else 
	{  // TV HEAD!
      
	glPushMatrix();
      
	glTranslatef( 0, -8.0, 5.5 );
	double hsize =  6.0 + num_followers * 0.5;
	//                            glScaled ( 18.0, 12.0, 1.0);
	glScaled ( hsize, hsize, hsize);
	glTranslatef( 0, 1.0, 0.0 );
	glColor3f(0.2, 0.2, 0.2);
	glCallList(_tv_dlist);
      
	glColor3f(1.0, 1.0, 1.0);
	glEnable(GL_TEXTURE_2D);
	//						glBindTexture(GL_TEXTURE_2D, cy->tvtex);
	glEnable(GL_BLEND);
	//					glBindTexture(GL_TEXTURE_2D, cy->field->videotextures[cy->field->textureSq]->tex());
	if ( holding_sign ) glBindTexture(GL_TEXTURE_2D, sign_tex);
	glPushMatrix();
	glScaled(0.8, 0.8, 0.8);
	glCallList(_screen_dlist);
	glPopMatrix();
      
	glDisable(GL_TEXTURE_2D);
      
      
	glPopMatrix();
      
	}
      */

      if ( _dAgentStyle == AGENT_KITTY ) {  
			
	//whiskers
	glPushMatrix();
	glLineWidth(2.0);
	glBegin(GL_LINES);

	glColor3f ( 0.0, 0.0, 0.0);
	glVertex3d ( 0,	-1.8,	4.3 );
	glVertex3d ( 4.0, -1.0,	4.3 );
			
	glVertex3d ( 0,	-1.8,	4.3 );
	glVertex3d ( 5.0, -1.8,	4.3 );


	glVertex3d ( 0,	-1.8,	4.3 );
	glVertex3d ( 4.0, -2.6,	4.3 );

	glVertex3d ( 0,	-1.8,	4.3 );
	glVertex3d ( -4.0, -1.0,	4.3 );
			
	glVertex3d ( 0,	-1.8,	4.3 );
	glVertex3d ( -5.0, -1.8,	4.3 );


	glVertex3d ( 0,	-1.8,	4.3 );
	glVertex3d ( -4.0, -2.6,	4.3 );

	glEnd();

	glPopMatrix();
		
	//ears
	glPushMatrix();
					
	/*
	  if ( holding_sign ) { 
	  glColor3dv  ( cy->field->col_faces.data());
	  } 
	  else {
	  glColor3dv  ( cy->field->col_edges.data());
	  }
	*/

					
	glPushMatrix();
	glRotatef	( 35, 0, 0, 1 );
	glTranslatef( 0, 3.0, 0 );
	glScalef			( 0.8, 0.5, 0.2 );
	glCallList(_hat_dlist);
						
	glPopMatrix();
				
	glPushMatrix();
	glRotatef	( -40, 0, 0, 1 );
	glTranslatef( 0, 3.0, 0 );
	glScalef			( 0.8, 0.5, 0.2 );
	glCallList(_hat_dlist);
						
	glPopMatrix();	
	glPopMatrix();
			

      }

      glPopMatrix();
      //end head 
}




void Agent::draw_legs(GLdouble * mmtx) {
    Matrix bmtx;
    bmtx.readgl(mmtx);

    Point3D righthip = bmtx.multPosition(rhip);
    Point3D lefthip  = bmtx.multPosition(lhip);

    Color4D pcol; //(holding_sign)? cy->field->col_faces : cy->field->col_edges ; 

    double thighlen = _leglen * _kneelevel;
    double calflen  = _leglen - thighlen;
    
    GLdouble fmtx[16];
    

    int walkmode = AGENT_LEGS_JOINTED;
    //left
    
    //position 
    
    //
    if ( walkmode == AGENT_LEGS_JOINTED ) { 


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


       if ( _debug ) { 
    
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

          (lefthip-lknee).fillMatrix(fmtx, vel, lknee, SQUASH_CONST);
          glPushMatrix();

            glMultMatrixd(fmtx);
            glTranslated( 0, -0.2, 0 );
            glScaled(0.12, 0.18, 0.16);
            //glColor3d(1,0,0);
            glCallList ( _body_dlist );

          glPopMatrix();

          (lknee-lfoot).fillMatrix(fmtx, vel, lfoot, SQUASH_CONST);
          glPushMatrix();

            glMultMatrixd(fmtx);
            glScaled(0.18, 0.12, 0.2);
            //glColor3d(1,0,0);
            glCallList ( _body_dlist );

          glPopMatrix();

         
          //right

          glColor3dv(pcol.data());

          (righthip-rknee).fillMatrix(fmtx, vel, rknee, SQUASH_CONST);
          glPushMatrix();

            glMultMatrixd(fmtx);
            glTranslated( 0, -0.2, 0 );
            glScaled(0.12, 0.18, 0.16);
            //glColor3d(1,0,0);
            glCallList ( _body_dlist);

          glPopMatrix();

          (rknee-rfoot).fillMatrix(fmtx, vel, rfoot, SQUASH_CONST);
          glPushMatrix();

            glMultMatrixd(fmtx);
            glScaled(0.18, 0.12, 0.2);
            //glColor3d(1,0,0);
            glCallList ( _body_dlist );

          glPopMatrix();
       }
    }
    
    else if ( walkmode == AGENT_LEGS_SIMPLE ) { 

       if ( _debug ) { 
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
          (lefthip-lfoot).fillMatrix(fmtx, vel, lfoot, SQUASH_CONST);
          glPushMatrix();
            glMultMatrixd(fmtx);
            glScaled(0.3, 0.12, 0.3);
            //glColor3d(0,1,0);
            glCallList ( _body_dlist  );

          glPopMatrix();

          (righthip-rfoot).fillMatrix(fmtx, vel, rfoot, SQUASH_CONST);
          glPushMatrix();

            glMultMatrixd(fmtx);
            glScaled(0.3, 0.12, 0.3);
            //glColor3d(1,0,0);
            glCallList ( _body_dlist  );

          glPopMatrix();

       }
    }

    //FEET
    //draw them dancin' shoes!

    if ( walkmode != AGENT_LEGS_NONE ) { 
       glColor3d( 0.2, 0.2, 0.2);
    
       //left foot 
       Point3D(0,1,0).fillMatrix(fmtx, ldir, lfoot , SQUASH_IDENT );
       glPushMatrix(); 
         glMultMatrixd(fmtx);
         if( stepphase > 1.0 ) { 
            double flop = 20 * sin ( (stepphase-1.0) * TWO_PI );
            glRotated( flop , 1, 0 , 0 );
         }
         glScaled ( 0.15, 0.08, 0.25 );
         glTranslated ( 0, 0.75, 3.0 );
         glCallList( _eye_dlist );
       glPopMatrix();

       //right foot 
       Point3D(0,1,0).fillMatrix(fmtx, rdir, rfoot , SQUASH_IDENT );
       glPushMatrix(); 
         glMultMatrixd(fmtx);
         //foot raised - flop a little 
         if( stepphase < 1.0 ) { 
            double flop = 20 * sin ( stepphase * TWO_PI );
            glRotated( flop , 1, 0 , 0 );
         }
         glScaled ( 0.15, 0.08, 0.25 );
         glTranslated ( 0, 0.75, 3.0 );
         glCallList( _eye_dlist );
       glPopMatrix();


    }

}

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

double Skin::complexity() { return Agent::_dcomplexity; }

Point3D Skin::l_eye_scale(Agent * ag) { return ag->_l_eye_scale; }
Point3D Skin::r_eye_scale(Agent * ag) { return ag->_r_eye_scale; }

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

   //fprintf(stderr, " skin: build \n" );
   glLoadIdentity();
	//body dimension
	   
   long int div = (long int ) ( complexity() * 4 );
	
   int i;
      


   _hand_base = Point3D ( 0 , 8 , 0 );
   _sign_base = Point3D ( 0 , 25, 0.5 );

   _walkmode = AGENT_LEGS_NONE;
    
   _body_scale = Point3D(0.5, 0.5, 0.5);

   _arm_scale = Point3D (0.8,1,0.8); //XXX HAKED? 

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
	float body_r1 = 3.0;
	float body_r2 = 3.6;

   _body_dlist = glGenLists(1);

	glNewList(_body_dlist, GL_COMPILE);
		glBegin(GL_TRIANGLE_FAN );
		glNormal3d ( 0.0, 1.0, 0.0 );
		glVertex3d ( 0.0, body_h, 0.0 ); 
		for  (i = 0; i <= div; i++ ) { 
			float ang = EAT_PI * 2.0 * i/div;
			glVertex3d ( body_r1 * cos(ang), 	body_h, body_r1 * sin(-ang) ); 
		}
		glEnd();
	
		glNormal3d ( 0.0, -1.0, 0.0 );
		glBegin(GL_TRIANGLE_FAN );
		glVertex3d ( 0.0, 0.0, 0.0 ); 
		for  (i = 0; i <= div; i++ ) { 
			float ang = EAT_PI * 2.0 * i/div;
			glVertex3d ( body_r2 * cos(-ang), 	0.0, 	body_r2 * sin(ang) ); 
		}
		glEnd();
	
	
		glBegin(GL_QUAD_STRIP );
		for  ( i = 0; i <= div; i++ ) { 
			float ang = EAT_PI * 2.0 * (double)i/(double)div;
			glNormal3d ( cos(ang), 0.0, sin(ang) );
			//glTexCoord2d ( i/div, 0 );
			glVertex3d ( body_r2 * cos(ang), 	0.0,	body_r2 * sin(ang) ); 
			//glTexCoord2d ( i/div, 1 );
			glVertex3d ( body_r1 * cos(ang), 	body_h,	body_r1 * sin(ang) ); 
		}
	   glEnd();

	glEndList();

   
   _leg_dlist = glGenLists(1);

   double leg_r1 = 0.3;
   double leg_r2 = 0.4;

   glNewList(_leg_dlist, GL_COMPILE);
		glBegin(GL_TRIANGLE_FAN );
		glNormal3d ( 0.0, -1.0, 0.0 );
		glVertex3d ( 0.0, 0.0, 0.0 ); 
		for  (i = 0; i <= div; i++ ) { 
			float ang = EAT_PI * 2.0 * i/div;
			glVertex3d ( leg_r1 * cos(ang), 	0.0, leg_r1 * sin(-ang) ); 
		}
		glEnd();
	
		glNormal3d ( 0.0, 1.0, 0.0 );
		glBegin(GL_TRIANGLE_FAN );
		glVertex3d ( 0.0, 1.0, 0.0 ); 
		for  (i = 0; i <= div; i++ ) { 
			float ang = EAT_PI * 2.0 * i/div;
			glVertex3d ( leg_r2 * cos(-ang), 	1.0, 	leg_r2 * sin(ang) ); 
		}
		glEnd();
	
	
		glBegin(GL_QUAD_STRIP );
		for  ( i = 0; i <= div; i++ ) { 
			float ang = EAT_PI * 2.0 * (double)i/(double)div;
			glNormal3d ( cos(ang), 0.0, sin(ang) );
			//glTexCoord2d ( i/div, 0 );
			glVertex3d ( leg_r2 * cos(ang), 	1.0,	leg_r2 * sin(ang) ); 
			//glTexCoord2d ( i/div, 1 );
			glVertex3d ( leg_r1 * cos(ang), 	0,	leg_r1 * sin(ang) ); 
		}
	   glEnd();

	glEndList();

   _legupper_dlist = _leg_dlist;
   _leglower_dlist = _leg_dlist;


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
	float arm_h 	= 5.0;
	float arm_r1 	= 1.5;
	float arm_r2 	= 2.0;

   _shoulder_base = Point3D (0, 6, 0);
   _axis_distance = 0.5;
	
   
	_arm_dlist = glGenLists(1);
	glNewList(_arm_dlist, GL_COMPILE);
	
	glBegin(GL_TRIANGLE_FAN );
	glNormal3d ( 0.0, -1.0, 0.0 );
	glVertex3d ( 0.0, 0.0, 0.0 ); 
	for  (i = 0; i <= div; i++ ) { 
		float ang = EAT_PI * 2.0 * i/div;
		glVertex3d ( arm_r1 * cos(ang), 	0.0,	arm_r1 * sin(-ang) ); 
	}
	glEnd();
	glBegin(GL_TRIANGLE_FAN );
	glNormal3d ( 0.0, 1.0, 0.0 );
	glVertex3d ( 0.0, arm_h, 0.0 ); 
	for  (i = 0; i <= div; i++ ) { 
		float ang = EAT_PI * 2.0 * i/div;
		glVertex3d ( arm_r2 * cos(-ang), 	arm_h, 	arm_r2 * sin(ang) ); 
	}
	glEnd();
	glBegin(GL_QUAD_STRIP );
	for  (i = 0; i <= div; i++ ) { 
		float ang = EAT_PI * 2.0 * i/div;
		glNormal3d ( cos(ang), 0.0, sin(ang) );             
		glVertex3d ( arm_r2 * cos(ang), 	arm_h, 	arm_r2 * sin(ang) ); 
		glVertex3d ( arm_r1 * cos(ang), 	0.0,	arm_r1 * sin(ang) ); 
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
		glVertex3d ( pole_rad * cos(ang), 	pole_height, 	pole_rad * sin(ang) ); 
		glVertex3d ( pole_rad * cos(ang), 	0.0,	pole_rad * sin(ang) ); 
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

	glNewList(_tv_dlist, GL_COMPILE);
	
		glBegin(GL_QUAD_STRIP);

		   glNormal3d(	 0,	-1,		0		);
		   
		   glVertex3d( -1,	-1,		0		);
		   glVertex3d( -1,	-1,		-1.5	);
		   glVertex3d(  1,	-1,		0		);
		   glVertex3d(  1,	-1,		-1.5	);
		   
		   glNormal3d(	 1,	 0,		0		);
		   
		   glVertex3d(  1,	1,		0		);
		   glVertex3d(  1,	0.6,	-1.5	);

		   glNormal3d(	 0,	 1,		0		);

		   glVertex3d( -1,	1,		0		);
		   glVertex3d( -1,	0.6,	-1.5	);

		   glNormal3d(	-1,	 0,		0		);

		   glVertex3d( -1,	-1,		0		);
		   glVertex3d( -1,	-1,		-1.5	);
		
		glEnd();

      glBegin(GL_QUADS); 
		
         glNormal3d(	 0,	0,		-1.0 );
		   
         glVertex3d( -1, -1,		-1.5 );
		   glVertex3d(  1, -1,		-1.5 );
		   glVertex3d(  1,  0.6,	-1.5 );		
		   glVertex3d( -1,  0.6,	-1.5 );	
		   
         glNormal3d(  0,  0,     1.0 );
         
         glVertex3d( -1, -1,	   0  );	
         glVertex3d(  1, -1,	   0  );	
         glVertex3d(  1,  1,     0  );
         glVertex3d( -1,  1,     0  );
		   
      glEnd();
      
      glBegin(GL_LINES);
		   glVertex3d( 0,		0.7,	-1);
		   glVertex3d( 0.5,	1.8,	-1.2);
		   glVertex3d( 0,		0.7,	-1);
		   glVertex3d( -0.7,	1.6,	-0.8);	
		glEnd();

	glEndList();

}

void Skin::set_walk( int i ) { _walkmode = i; }
void Skin::draw_agent(Agent * ag ) { 

   Point3D body_base;

   if ( _walkmode == AGENT_LEGS_NONE ) {
      body_base = ag->pos;
   }
   else { 
      double bounce =  1.0 * ( Agent::_bounce_base +                                                       \
                               Agent::_bounce_mag * max(0.0, (ag->speed-ag->moveSpeed)/ag->moveSpeed ) )   \
                                                * fabs(sin( (ag->stepphase - 0.25) * EAT_PI )); 
      Point3D vpos = Point3D(0, bounce+Agent::_walkheight, 0 );
      body_base = ag->pos + vpos;
   }


    GLdouble mmtx[16];

    
    //((ag->head->pos()-ag->base->pos())* 0.1 ).fillMatrix(mmtx, ag->vel, body_base, SQUASH_VOLUME);
    Point3D(0,1,0).fillMatrix(mmtx, ag->vel, body_base, SQUASH_VOLUME);
    //mult by mmtx
    draw_body(ag, mmtx);

    //use mmtx for legs
    draw_legs(ag, mmtx);

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

void Skin::draw_body(Agent *ag, GLdouble * mmtx ) { 


   glPushMatrix();

     
   glMultMatrixd ( mmtx );

   
   //body
   
   glScaled(_body_scale[0],_body_scale[1],_body_scale[2] );

   glRotated ( Agent::_wobble * 0.3 * cos ( ag->swingphase -0.5  ) , 0, 0, 1.0 );

   glColor3d( 1.0, 1.0 ,1.0 );
   
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
  glRotated 	( ag->headangle, 1, 0, 0);
  glColor3dv  ( ag->skincol.data());
  
  //NORMAL HEAD
  glCallList 	( _head_dlist );
  
  //eyes
  
  glPushMatrix();
  //glColor3f 	( 0.1, 0.1, 0.1 );
  glColor3f      (0,0,0);
  glTranslatef(  _eye_pos[0], _eye_pos[1], _eye_pos[2] );
  glScalef 	( 0.15, 0.15, 0.15 );
  if ( ag->blink ) glScalef(1.2, 0.5, 1.2);
  glCallList(_eye_dlist);
  glPopMatrix();
  
  glPushMatrix();                    
  //glColor3f 	( 0.1, 0.1, 0.1 );
  glColor3f      (0,0,0);
  glTranslatef( -_eye_pos[0], _eye_pos[1], _eye_pos[2] );
  glScalef 	( 0.15, 0.15, 0.15 );
  if ( ag->blink ) glScalef(1.2, 0.5, 1.2);
  glCallList(_eye_dlist);
  glPopMatrix();
  
  
  glPopMatrix();
}

void Skin::draw_arms(Agent *ag) { 

  //      Crowd * cy  =  ag->cy;
      bool holding_sign = ag->holding_sign;
      bool holding_tv   = ag->holding_tv;
        //right arm
	
		glPushMatrix();
			
		glColor3dv  	( ag->skincol.data() );
		glTranslatef	( _shoulder_base[0], _shoulder_base[1] , _shoulder_base[2] );
		
		glRotatef	( ag->l_armangle, 0 ,0, -1);
		
		if ( holding_tv) { 
		  glRotatef	( 10.0 * cos(ag->swingphase), 1, 0, 0 );
		}
		else	glRotatef	( 20.0*cos(ag->swingphase), 1, 0, 0 );
		
		glTranslatef 	( 0, _axis_distance, 0 );
		glPushMatrix();
		glCallList 	( _arm_dlist );
		glPopMatrix();
		
		if (holding_sign) { 
		  glPushMatrix();
		  
		  glTranslatef ( _hand_base[0],_hand_base[1],_hand_base[2] );
		  
		  glRotatef( ag->l_armangle + 5, 0, 0, 1);
		  glTranslatef ( 0, 0, 0);
		  glColor3f (0.4, 0.3, 0.1 );
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
		  
		  //		  glColor3dv(cy->dCol_sb.data());
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
		  glColor3f (0.4, 0.3, 0.1 ); 
		  glScaled(1, 1.2, 1 );
		  glCallList(_pole_dlist);
		  glPopMatrix();
		  
		  glPushMatrix();
		  
		  glTranslatef( 0, 40 , 3 );
		  glScaled ( 6, 6, 6);
		  
                  glColor3f(0.2, 0.2, 0.2);
		  glCallList(_tv_dlist);
		  
                  glColor3f(1.0, 1.0, 1.0);
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
			glColor3dv	(ag->skincol.data());
			glScalef 	( -1, 1, 1);
		   glTranslatef	( _shoulder_base[0], _shoulder_base[1] , _shoulder_base[2] );
			glRotatef	( ag->r_armangle, 0 ,0, -1);
			glRotatef	( -20.0*cos( ag->swingphase ), 1, 0, 0 );
			glTranslatef 	( 0, _axis_distance, 0 );
         glPushMatrix();
			   glCallList 	( _arm_dlist );
		   glPopMatrix();
		glPopMatrix();

}

void Skin::draw_legs(Agent *ag, GLdouble * mmtx ) { 

    Matrix bmtx;
    bmtx.readgl(mmtx);

    Point3D righthip = bmtx.multPosition(ag->rhip);
    Point3D lefthip  = bmtx.multPosition(ag->lhip);

    Color4D pcol(1.0,1.0,1.0,1.0);
    //    Color4D pcol = (ag->holding_sign)? cy->field->col_faces : cy->field->col_edges ; 

    double thighlen = Agent::_leglen * Agent::_kneelevel;
    double calflen  = Agent::_leglen - thighlen;
    
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
			glRotated 	( ag->headangle, 1, 0, 0);
			glColor3dv  ( ag->skincol.data());

      	//NORMAL HEAD
         glCallList 	( _head_dlist );  
         if ( _screen_dlist ) { 
            glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, cy->field->videotextures[cy->field->textureSq]->tex());
            glCallList 	( _screen_dlist );
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
//	float body_r1 = 3.0;
//	float body_r2 = 3.6;
	
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

//   float arm_h 	= 5.0;
//	float arm_r1 	= 1.5;
//	float arm_r2 	= 2.0;
	
   
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
		      glVertex3d ( pole_rad * cos(ang), 	pole_height, 	pole_rad * sin(ang) ); 
		      glVertex3d ( pole_rad * cos(ang), 	0.0,	pole_rad * sin(ang) ); 
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

		   glNormal3d(	 0,	-1,		0		);
		   
		   glVertex3d( -1,	-1,		0		);
		   glVertex3d( -1,	-1,		-1.5	);
		   glVertex3d(  1,	-1,		0		);
		   glVertex3d(  1,	-1,		-1.5	);
		   
		   glNormal3d(	 1,	 0,		0		);
		   
		   glVertex3d(  1,	1,		0		);
		   glVertex3d(  1,	0.6,	-1.5	);

		   glNormal3d(	 0,	 1,		0		);

		   glVertex3d( -1,	1,		0		);
		   glVertex3d( -1,	0.6,	-1.5	);

		   glNormal3d(	-1,	 0,		0		);

		   glVertex3d( -1,	-1,		0		);
		   glVertex3d( -1,	-1,		-1.5	);
		
		glEnd();

      glBegin(GL_QUADS); 
		
         glNormal3d(	 0,	0,		-1.0 );
		   
         glVertex3d( -1, -1,		-1.5 );
		   glVertex3d(  1, -1,		-1.5 );
		   glVertex3d(  1,  0.6,	-1.5 );		
		   glVertex3d( -1,  0.6,	-1.5 );	
		   
         glNormal3d(  0,  0,     1.0 );
         
         glVertex3d( -1, -1,	   0  );	
         glVertex3d(  1, -1,	   0  );	
         glVertex3d(  1,  1,     0  );
         glVertex3d( -1,  1,     0  );
		   
      glEnd();
		
      
      glBegin(GL_LINES);
		   glVertex3d( 0,		0.7,	-1);
		   glVertex3d( 0.5,	1.8,	-1.2);
		   glVertex3d( 0,		0.7,	-1);
		   glVertex3d( -0.7,	1.6,	-0.8);	
		glEnd();

	glEndList();

}

void ToonBot::draw_head(Agent * ag) { 
      Crowd * cy = ag->cy;

		glPushMatrix();
	
		//face

			glTranslatef ( _head_pos[0], _head_pos[1], _head_pos[2] );
			glRotated 	( ag->headangle, 1, 0, 0);
			glColor3dv  ( ag->skincol.data());

      	//NORMAL HEAD
         if ( vertex_program_mode ) { 
            if ( ag->holding_sign ) glColor3dv  ( cy->field->col_faces.data());
            else                    glColor3dv  ( cy->field->col_edges.data());
         }
         glCallList 	( _head_dlist );  
         
         if ( _screen_dlist ) { 
            glColor3dv  ( cy->field->col_faces.data());
            glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, cy->field->videotextures[cy->field->textureSq]->tex());
            glCallList 	( _screen_dlist );
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


