#ifndef AUDICLE_BASE_INCLUDED
#define AUDICLE_BASE_INCLUDED

#include "sys.h"
#include "texteditor.h"
#include "audicle_coding.h"


//gl callbacks
void Init(void);
void Draw(void);

void IdleFunction(void);
void Reshape(int width, int height);

void Keyboard(unsigned char,int,int);
void SpecialKeys(int,int,int);
void Mouse(int,int,int,int);
void Motion(int,int);
void DepressedMotion(int,int);


//helpers
void pixToNDC( int x, int y ) ;
void checkGLErr() ;

void launch_audicle_process();
void ckinf_new_editor_window();

#endif
