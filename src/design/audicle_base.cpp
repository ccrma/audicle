#include "audicle_base.h"

//globals
WindowManager * wm;
CodeWindowHandler * wh;
int cur_vp[4];
double hsize;
double vsize;
int mousex;
int mousey;
Point2D cur_pt;
Point2D pick_pt;
Point2D last_pt;

Point3D p1 ;
Point3D p2 ;
Point3D p3 ;
Point3D* pv[3] = { &p1, &p2, &p3 };


void launch_audicle_process() { 

   cursorOn = true;

#ifdef __PLATFORM_MACOSX__ 
   char* cwd = getcwd(NULL,0);
#endif
   int gfoo = 1; char *gbar[] = {"audicle"};

   glutInit               ( &gfoo, gbar);


#ifdef __PLATFORM_MACOSX__
   chdir(cwd);
   free (cwd);
#endif
   
   glutInitDisplayMode    ( GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH | GLUT_STENCIL); // | GLUT ALPHA 
   glutInitWindowSize     ( 640, 480);
   glutInitWindowPosition ( 32,32);       
   
   glutCreateWindow       ( "audicle" );
   
   Init();
	
   wm = new WindowManager();
   wm->setHandler( ( wh=new CodeWindowHandler() ) );
   //glutFullScreen();
   glutMainLoop();

}

void die() { 
	//not very classy
  exit(0);
}

void Init( void )
{

  glutDisplayFunc ( Draw );     
  glutReshapeFunc ( Reshape);  
  glutKeyboardFunc( Keyboard );
  glutSpecialFunc ( SpecialKeys );  
  glutMouseFunc   ( Mouse );          
  glutMotionFunc  ( DepressedMotion );
  glutPassiveMotionFunc ( Motion );  
  glutVisibilityFunc ( NULL );      
  glutIdleFunc( IdleFunction );    


#if (GLUT_MACOSX_IMPLEMENTATION >= 2 ) 
  glutWMCloseFunc ( die );
#endif

  GLdouble far_z = 1900.0;
  GLdouble near_z = 0.1;

  glEnable (GL_POINT_SMOOTH);
  glEnable (GL_LINE_SMOOTH);
  glHint (GL_LINE_SMOOTH_HINT, GL_NICEST);
  glEnable (GL_BLEND);
  glColorMask(true, true, true, true);
  glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  glPolygonOffset(  1.0, 1.0 );
  glLineWidth (1);
   
  //glEnable (GL_POLYGON_SMOOTH);
  glDisable (GL_DEPTH_TEST);
    
  glClearDepth(far_z);
  glClearColor(1.0 ,1.0 , 1.0 ,0.f);
  glClearStencil(0);

  glSelectBuffer( PICK_BUFFER_SIZE, (GLuint*) PickBuffer );

  glGetIntegerv(GL_VIEWPORT, (GLint *) cur_vp );


  glViewport( 0,0, 640, 480);
  
  hsize = 1.00;
  vsize = 1.33;

  glMatrixMode( GL_PROJECTION );
  glLoadIdentity();
 
  glOrtho(-hsize, hsize, -vsize, vsize, -10, 10 );
  
  //this is silly as all hell
#if defined(__PLATFORM_WIN32__)
  ShowCursor(false);
#endif
}

void Draw( void ) { 

    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();

    if ( RenderMode == GL_SELECT ) {     
        gluPickMatrix( mousex, cur_vp[3] - mousey , PICK_TOLERANCE, PICK_TOLERANCE, (GLint*)cur_vp );
        glInitNames();
        glPushName( 0xffffffff ); /* a strange value */
    }

    glOrtho(-hsize, hsize, -vsize, vsize, -10, 10 );

    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    glMatrixMode ( GL_MODELVIEW );

	if ( wm ) wm->draw();
        
    if ( RenderMode == GL_RENDER  && DragManager::instance()->mode() == drag_is_Holding ) { 
        glColor4d ( 0, 0.3, 0.3, 0.8 );
        glLineWidth ( 2.0 );
        p1 = cur_pt;
        p2 = cur_pt + ( last_pt - cur_pt ) * 2.0 ;
        p3 = pick_pt;
        Point3D tmp;
        glBegin(GL_LINE_STRIP);
        for ( double ri = 0 ; ri <= 1.0 ; ri += 0.025 ) { 
            tmp = tmp.bezier_interp_pt ( (Point3D **)&pv, 3, ri );
            glVertex2d (tmp[0], tmp[1] );
        }
        glEnd();
    }



    glFlush();

    if( RenderMode == GL_RENDER ) { 
        glutSwapBuffers();     
    }

	checkGLErr();
}


void checkGLErr() { 
   GLenum errCode;
   const GLubyte *errString;

   while ((errCode = glGetError()) != GL_NO_ERROR ) { 
     errString = gluErrorString(errCode);
     fprintf(stderr, " OpenGL Error : %d\t%s\n" , errCode , errString ); 
   }
}

void IdleFunction(void){

    glutPostRedisplay();
#ifdef __PLATFORM_WIN32__
    Sleep ( 30 );
#else
    usleep ( 30000 );
#endif
}

void Reshape (int width, int height)
{

  glViewport(0, 0, width, height); 
  glGetIntegerv(GL_VIEWPORT, (GLint*)cur_vp);

  vsize = ( cur_vp[3] == 0 ) ? 1 : (double)cur_vp[3]/480.0;
  double aspect = ( cur_vp[3] == 0 ) ? 1 : (double)cur_vp[2]/(double)cur_vp[3]; 

  hsize = aspect * vsize;
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity(); 
  glOrtho(-hsize, hsize, -vsize, vsize, -10, 10 );
  wm->root()->setPos( Point2D(-hsize, vsize) );
  wm->root()->resize( hsize* 2.0, vsize * 2.0 );
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

}

void pixToNDC( int x, int y ) { 

   Point2D tmp_pt = cur_pt;
   cur_pt[0] = hsize * 2.0 * ( (double)x/(double)cur_vp[2] - 0.5 );
   cur_pt[1] = vsize * 2.0 * ( (double)(cur_vp[3]-y)/(double)cur_vp[3] - 0.5 );
   if ( !( cur_pt == tmp_pt ) ) last_pt = tmp_pt; 

}

void Mouse( int button, int state, int x, int y )
{
    mousex = x;
    mousey = y;

    pixToNDC(x,y);

        //set IDS


    RenderMode= GL_SELECT;
    glRenderMode( RenderMode );
    
    Draw();
    //return to normal
    RenderMode= GL_RENDER;
    int nHit = glRenderMode( RenderMode );
    

    uint n = 0 ;
    int oNum = 0;
    uint * pickstack = NULL;
    for ( int i = 0; i < nHit; i++ ) { 
        oNum = PickBuffer[n++];
        uint zMin = PickBuffer[n++];
        uint zMax = PickBuffer[n++];
        pickstack = PickBuffer + n;
//      fprintf(stderr, "stack %d objs, %d %d\n", oNum ,zMin, zMax );
        for ( int j = 0; j < oNum ; j++ ) { 
//          fprintf(stderr, "item %d\n", PickBuffer[n++]);
            n++;
            }
    }
    
    DragManager *  d = DragManager::instance();
    d->setmode( ( state == GLUT_DOWN ) ? drag_is_Picking : drag_is_Dropping );

    InputEvent ev = InputEvent ( input_MOUSE, cur_pt, button, state );
    ev.setMods( glutGetModifiers() );

    uint * tstack = (uint*) malloc ( oNum * sizeof(uint) );
    memcpy ( (void*)tstack, (void*)pickstack, oNum * sizeof(uint) );
    ev.setStack( tstack, oNum );
    ev.time = GetTime();

    if ( wm ) wm->handleEvent( ev );        


    free ( tstack );

    if ( d->mode() == drag_is_Holding ) pick_pt = cur_pt;

    if ( state == GLUT_UP ) { //release an unhandled drag
        
        if ( d->object() != NULL && d->mode() != drag_is_Empty ) { 
            
            //drop the object...
            switch ( d->type() ) { 
            case drag_CodeRevision:
                wh->winFromRev ( (CodeRevision*) d->object() );                
                break;
            }
            d->setmode ( drag_is_Empty );
            d->setobject  ( NULL, drag_None );
        }
        
    }

    

}

void Motion( int x, int y) {
    mousex = x;
    mousey = y;

#ifdef __PLATFORM_MACOSX__
    if ( 0 < x && x < cur_vp[2] && 0 < y && y < cur_vp[3] ) { 
        if ( cursorOn ) { 
            HideCursor();
            cursorOn = false;
        }
    }
    else { 
        if ( !cursorOn ) { 
            ShowCursor();
            cursorOn = true;
        }
    }
#endif

    pixToNDC(x,y);

    InputEvent ev = InputEvent ( input_MOTION, cur_pt );
    ev.time = GetTime();
    if ( wm ) wm->handleEvent( ev );        

}

void DepressedMotion( int x, int y ){  
   
    mousex = x;
    mousey = y;

    pixToNDC(x,y);
    
    InputEvent ev = InputEvent ( input_MOTION, cur_pt );
    ev.time = GetTime();
    if ( wm ) wm->handleEvent( ev );        

}

void Keyboard( unsigned char c, int x, int y )
{
   pixToNDC(x,y);

    
   InputEvent ev = InputEvent ( input_KEY, cur_pt, c ); 
   ev.setMods( glutGetModifiers() );
   ev.time = GetTime();
   if ( wm ) wm->handleEvent( ev );     
   
//   if ( wm ) wm->handleKey( c, cur_pt );
}

void SpecialKeys( int key, int x, int y )
{
   pixToNDC(x,y);
   InputEvent ev = InputEvent ( input_SPEC, cur_pt, key ); 
   ev.setMods( glutGetModifiers() );
   ev.time = GetTime();
   if ( wm ) wm->handleEvent( ev );        
//   if ( wm ) wm->handleSpec( key, cur_pt);  
}
