#include "windowing.h"
//#include "image.h"
//write all that into a file


DragManager * DragManager::_instance = NULL;

DragManager::DragManager( )  
{ 
    _type       = drag_None;
    _object     = NULL;
}

DragManager* 
DragManager::instance( ) 
{ 
    if ( !_instance ) 
    { 
        _instance = new DragManager( );
        if ( !_instance ) { 
            fprintf( stderr, "[chuck] error: DragManager instance creation failed\n" );
        }   
    }
    return _instance;
}

IDManager * IDManager::_inst = NULL;

IDManager::IDManager( ) { 
    GLint bits = 8;
    glGetIntegerv ( GL_STENCIL_BITS, &bits );
    _sidnum = 1 << bits;
    _sids = ( bool*) malloc ( _sidnum * sizeof( bool) );
    for ( int i = 0; i < _sidnum; i++ ) { 
        _sids[i] = false;
    }
    _pickID = _sidnum; //pickIDs are separate from stencil ids
}

IDManager * 
IDManager::instance( ) { 
    if ( !_inst ) { 
        _inst = new IDManager( );
        if ( !_inst ) { 
            fprintf( stderr,"IDManager::could not create _instance;\n");
        }
    }
    return _inst;
}

int
IDManager::getStencilID( ) { 
    for ( int i = 1 ; i < _sidnum ; i++ ) { 
        if ( _sids[i] == false ) {
            _sids[i] = true;
            //fprintf( stderr, "assigned stencil %d\n", i ); 
            return i;
        }
    }
    fprintf( stderr, "error - ID array is full!\n");
    return 0;
}
void
IDManager::freeStencilID( int i) {
    if ( i > 0 && i < _sidnum ) { 
        if ( !_sids[i] ) { fprintf( stderr, "IDManager::_freeID - error: freeing unbound ID %d\n", i ); }
        _sids[i] = false;
    }
    else fprintf( stderr,"IDManager::_freeID - ID %d out of range!\n", i);    
}

uint
IDManager::getPickID( ) {
    //fprintf( stderr, "assigned pick %d\n", _pickID + 1 );
    return ++_pickID;
}

void 
UIRectangle::handleMouse ( const InputEvent & e) { 
    _selected = false;
    if ( e.state == GLUT_UP || e.size == 0 ) return ; 
    _selected = e.checkID(_id );

}

void 
UIRectangle::_defColors( ) { 

    _bg[UI_BASE]      = Color4D( 0.2, 0.2, 0.2, 0.3 );
    _border[UI_BASE]  = Color4D( 0.2, 0.8, 0.2, 0.8 );
    
    _bg[UI_HOVER]     = Color4D( 0.2, 0.2, 0.2, 0.5 );
    _border[UI_HOVER] = Color4D( 0.5, 0.8, 0.5, 0.8 );
    
    _bg[UI_SEL]       = Color4D( 0.5, 0.8, 0.8, 0.9 );
    _border[UI_SEL]   = Color4D( 0.5, 0.9, 0.3, 0.9 );

}

void
UIRectangle::setCol( Color4D& c) {
    glColor4f( c[0],c[1],c[2],c[3]);
}

void
UIRectangle::setCols( ui_state which , Color4D bg, Color4D border) {
    if ( which >= 0 && which < 3 ) { 
        if ( bg[3] >= 0 ) _bg[which] = bg;
        if ( border[3] >= 0 ) _border[which] = border;
    }
}

void
UIRectangle::quadVerts( ) { 
    glVertex2d( tl[0], tl[1]);
    glVertex2d( br[0], tl[1]);
    glVertex2d( br[0], br[1]);
    glVertex2d( tl[0], br[1]);
}

void
UIRectangle::roundVerts( float rad) {

	int div = 3;
	double span = TWO_PI * 0.25 / ( double)div;
	int i;

	if ( rad <= 0 ) rad = 0.5 * min ( w( ), h( ) );

	bool xspan = ( w( ) != 2.0 * rad ) ;
	bool yspan = ( h( ) != 2.0 * rad ) ;

	Point2D p;
	Point2D first = tl - Point2D( 0 , -rad ) ;

	p = tl + Point2D( rad, -rad );
	for ( i = 0 ; i <= div ; i++ ) { 
		glVertex2d ( p[0] - rad * cos( ( double)i * span ) , p[1] + rad * sin( ( double)i * span ) );
	}
	p = Point2D( br[0] - rad, tl[1] -rad );
	for ( i = 0 ; i <= div ; i++ ) { 
		if ( xspan || i ) glVertex2d ( p[0] + rad * sin( ( double)i * span ) , p[1] + rad * cos( ( double)i * span ));
	}
	p = br + Point2D( - rad, rad );
	for ( i = 0 ; i <= div ; i++ ) { 
		if ( yspan || i ) glVertex2d ( p[0] + rad * cos( ( double)i * span ) , p[1] - rad * sin( ( double)i * span ));
	}
	p = Point2D( tl[0] + rad, br[1] + rad );
	for ( i = 0 ; i <= div ; i++ ) { 
		if ( xspan || i ) glVertex2d ( p[0] - rad * sin( ( double)i * span ) , p[1] - rad * cos( ( double)i * span ));
	}


}

void
UIRectangle::dropShadowRounded( float dens, float rad) { 
    glPushMatrix( );
    glTranslated( 0.08, -0.08, 0 );

	int div = 6;
	double span = TWO_PI * 0.25 / ( double)div;
	int i;

	if ( rad <= 0 ) rad = 0.5 * min ( w( ), h( ) );

	Point2D p;
        
        double il = tl[0] + rad;
        double ir = br[0] - rad;
        double it = tl[1] - rad;
        double ib = br[1] + rad;

        //center block 
        glColor4d ( 0,0,0,dens);
        glBegin( GL_QUADS);
        glVertex2d ( il ,  it );
        glVertex2d ( ir ,  it );
        glVertex2d ( ir ,  ib );
        glVertex2d ( il ,  ib );


        //upper
        glColor4d ( 0,0,0,0);
        glVertex2d ( il   , tl[1] );
        glVertex2d ( ir   , tl[1] );
        glColor4d ( 0,0,0,dens);
        glVertex2d ( ir   , it    );
        glVertex2d ( il   , it    );

        //lower
        glColor4d ( 0,0,0,0);
        glVertex2d ( il   , br[1] );
        glVertex2d ( ir   , br[1] );
        glColor4d ( 0,0,0,dens);
        glVertex2d ( ir   , ib    );
        glVertex2d ( il   , ib    );

        //left
        glColor4d ( 0,0,0,0);
        glVertex2d ( tl[0] , it  );
        glVertex2d ( tl[0] , ib  );
        glColor4d ( 0,0,0,dens);
        glVertex2d ( il    , ib  );
        glVertex2d ( il    , it  );


        //right
        glColor4d ( 0,0,0,0);
        glVertex2d ( br[0] , it  );
        glVertex2d ( br[0] , ib  );
        glColor4d ( 0,0,0,dens);
        glVertex2d ( ir    , ib  );
        glVertex2d ( ir    , it  );



        glEnd( );        
	p = tl + Point2D( rad, -rad );
        glBegin( GL_TRIANGLE_FAN);
        glColor4d ( 0,0,0,dens);
        glVertex2d ( p[0], p[1]);
        glColor4d ( 0,0,0,0.0);
	for ( i = 0 ; i <= div ; i++ ) { 
		glVertex2d ( p[0] - rad * cos( ( double)i * span ) , p[1] + rad * sin( ( double)i * span ) );
	}
        glEnd( );

	p = Point2D( br[0] - rad, tl[1] -rad );
        glBegin( GL_TRIANGLE_FAN);
        glColor4d ( 0,0,0,dens);
        glVertex2d ( p[0], p[1]);
        glColor4d ( 0,0,0,0.0);
	for ( i = 0 ; i <= div ; i++ ) { 
		glVertex2d ( p[0] + rad * sin( ( double)i * span ) , p[1] + rad * cos( ( double)i * span ));
	}
        glEnd( );

	p = br + Point2D( - rad, rad );
        glBegin( GL_TRIANGLE_FAN);
        glColor4d ( 0,0,0,dens);
        glVertex2d ( p[0], p[1]);
        glColor4d ( 0,0,0,0.0);
	for ( i = 0 ; i <= div ; i++ ) { 
		glVertex2d ( p[0] + rad * cos( ( double)i * span ) , p[1] - rad * sin( ( double)i * span ));
	}
        glEnd( );

	p = Point2D( tl[0] + rad, br[1] + rad );
        glBegin( GL_TRIANGLE_FAN);
        glColor4d ( 0,0,0,dens);
        glVertex2d ( p[0], p[1]);
        glColor4d ( 0,0,0,0.0);
	for ( i = 0 ; i <= div ; i++ ) { 
		glVertex2d ( p[0] - rad * sin( ( double)i * span ) , p[1] - rad * cos( ( double)i * span ));
	}
        glEnd( );

        glPopMatrix( );
    
}

void
UIRectangle::outlineQuad( ) { 
    setCol( _border[UI_BASE]); 
    if ( _selected ) setCol( _border[UI_SEL]);
    else if ( _hover ) setCol( _border[UI_HOVER]);
    
    glBegin( GL_LINE_LOOP);
    quadVerts( );
    glEnd( );
}

void
UIRectangle::drawID( ) { 
    glPushName( _id );
    glBegin( GL_QUADS);
    quadVerts( );
    glEnd( );
    glPopName( );
}

void
UIRectangle::filledQuad( ) { 
    
    setCol( _bg[UI_BASE]); 
    if ( _selected ) setCol( _bg[UI_SEL]);
    else if ( _hover ) setCol( _bg[UI_HOVER]);
    
    glBegin( GL_QUADS);
    quadVerts( );
    glEnd( );
}

void
UIRectangle::outlineRounded( float rad) { 
    setCol( _border[UI_BASE]); 
    if ( _selected ) setCol( _border[UI_SEL]);
    else if ( _hover ) setCol( _border[UI_HOVER]);
    
    glBegin( GL_LINE_LOOP);
	roundVerts( rad);
	glEnd( );
}

void
UIRectangle::filledRounded( float rad) { 
    
    setCol( _bg[UI_BASE]); 
    if ( _selected ) setCol( _bg[UI_SEL]);
    else if ( _hover ) setCol( _bg[UI_HOVER]);
    
    glBegin( GL_POLYGON);
	roundVerts( rad);
	glEnd( );
}

void
UIRectangle::bubbleRounded( float rad ) { 

    Point2D hilite = ( tl * 0.67 ) + ( br * 0.33 );

    glBegin( GL_TRIANGLE_FAN);

    glColor3d ( 1.0, 1.0 , 1.0 );
    glVertex2d( hilite[0], hilite[1]);

    setCol( _bg[UI_BASE]); 
    if ( _selected ) setCol( _bg[UI_SEL]);
    else if ( _hover ) setCol( _bg[UI_HOVER]);
    roundVerts( rad);
    double r = ( rad > 0 ) ? rad : 0.5 * min ( h( ), w( ));
    Point2D first = tl - Point2D( 0 , r ) ;
    glVertex2d( first[0], first[1]);

    glEnd( );
}

void
UIRectangle::bubbleQuad( ) { 

    Point2D hilite = ( tl * 0.67 ) + ( br * 0.33 );

    glBegin( GL_TRIANGLE_FAN);

    glColor3d ( 1.0, 1.0 , 1.0 );
    glVertex2d( hilite[0], hilite[1]);

    setCol( _bg[UI_BASE]); 
    if ( _selected ) setCol( _bg[UI_SEL]);
    else if ( _hover ) setCol( _bg[UI_HOVER]);

    quadVerts( );

    glEnd( );
}

void
UIRectangle::vLine() { 
    setCol( _bg[UI_BASE]); 
    if ( _selected ) setCol( _bg[UI_SEL] );
    else if ( _hover ) setCol( _bg[UI_HOVER] );
    
    glBegin( GL_LINES);
	glVertex2d( 0.5 * ( tl[0]+br[0]) , tl[1] );
	glVertex2d( 0.5 * ( tl[0]+br[0]) , br[1] );
	glEnd( );
}

void
UIRectangle::hLine( ) { 
    setCol(_bg[UI_BASE]); 
    if ( _selected ) setCol(_bg[UI_SEL]);
    else if ( _hover ) setCol(_bg[UI_HOVER]);
    
    glBegin(GL_LINES);
	glVertex2d( tl[0], 0.5 * ( tl[1]+br[1]) );
	glVertex2d( br[0], 0.5 * ( tl[1]+br[1]) );
	glEnd();
}

void
UIRectangle::translate( ) { 
    glTranslated( tl[0], tl[1], 0 );
}

void
UIRectangle::offset() { 
    glTranslated( _origin[0], -_origin[1], 0 );
}

void
UIRectangle::scale() { 
    glScaled( _scale[0], _scale[1], 1 );
}

void 
UIRectangle::transform() { 
    translate();
    offset();
    scale();
}

void
UIRectangle::viewTranslate() { 
    glTranslated( - tl[0], -tl[1], 0 );
}

//cartesian ( for gl rendering )
void
UIRectangle::vpTranslate() { 
    glTranslated( tl[0], br[1], 0 );
}

void
UIRectangle::vpOffset() { 
    glTranslated( -_origin[0], _origin[1] , 0);
}

Point2D
UIRectangle::toGlobal(Point2D pt) { return tl + (pt - _origin).mult(_scale); } 

Point2D
UIRectangle::toLocal(Point2D pt) { return _origin + (pt - tl).div(_scale); } 


UIRectangle
UIRectangle::toGlobal(UIRectangle r) { 
	UIRectangle ret  = r;
	ret.br = toGlobal(ret.br);
	ret.tl = toGlobal(ret.tl);
	return ret;
} 

UIRectangle
UIRectangle::toLocal(UIRectangle r) { 
	UIRectangle ret = r;
	ret.br = toLocal(ret.br);
	ret.tl = toLocal(ret.tl);
	return ret;
} 

Point2D
UIRectangle::vpToGlobal(Point2D pt) { return (pt - tl).mult(_scale) ; }

Point2D
UIRectangle::vpToLocal(Point2D pt) { return tl + pt.div(_scale); } 

void 
UIRectangle::vpTransform() { 
    vpTranslate();
    vpOffset();
    vpScale();
}

void
UIRectangle::vpScale() { 
    glScaled( 1.0 / _scale[0], 1.0 / _scale[1], 1 );
}

UIButton::UIButton() : _menu(NULL) { 
	_label = "button";
}

UIButton::UIButton(char *s) : _menu(NULL) { 
	if ( s ) _label = s;
}

void
UIButton::draw( float scale ) { 
	bubbleRounded();
	glPushMatrix();
        vpTranslate();
        double fsiz = scale * 0.75; 
        double margin = 0.5 * ( 1.0 - fsiz );
        glTranslated( w() * 0.5, margin * h() , 0 );
        scaleFont( fsiz * h() , 1.0 );
        glColor4f(0.0,0.0,0.0, 0.8);
        drawString_centered(_label.c_str() );
	glPopMatrix();
	outlineRounded();
	if ( _menu ) { 
		_menu->draw();
	}
}

void
UIButton::handleMouse ( const InputEvent &e  ) { 
    _selected = e.checkID(_id);
}


void
UIButton::fitLabel() { 
    double dlen = 0.01 * drawString_length_kerned( _label.c_str() );
    setw( h() * max ( 1.0 , dlen )) ;
}

void
UIButton::setMenu(UIMenu * menu ) { 
	_menu = menu;
	if ( _menu ) { 
		_menu->setx( left() + 0.25 * w() );
		_menu->sety( bottom() );
	}	
}

void
UIRectangle::drawNamed() { 
	glPushName( _id );
	draw();
	glPopName();
}

UIScrollBar::UIScrollBar(bool vertical) { 
	_vertical = vertical;
	_vpos = 0;
	_vrange = 1;
	_vspan = 1;
}

void
UIScrollBar::setRange( double pos, double span, double range){ 
		
	_vpos = pos;
	_vspan = span;	
	_vrange = range;

	if ( _vertical ) { 
		_box.sett( _vpos / _vrange );
		_box.seth( _vspan / _vrange );
	}
	else { 
		_box.setl( _vpos / _vrange );
		_box.setw( _vspan / _vrange );
	}

}

void
UIScrollBar::draw() { 

    glLineWidth ( 1 );

    glPushName( _id ) ;
	if ( _vertical ) vLine();
	else hLine();

	if ( _vspan < _vrange ) { 
		glPushName( _box.id() );
		UIRect b = toGlobal(_box);
		b.filledRounded();
		b.outlineRounded();
		glPopName();
	}
    glPopName();
}

bool
UIScrollBar::checkHover(Point2D pt)
{ 
	//box is always inside scrollbar
	_hover = inside(pt) ? true : false ; 
	if ( _hover ) 
		_box.checkHover( toLocal(pt) );
	return _hover;
}

double
UIScrollBar::handleMotion( Point2D dpos ) { 

    double mod = (_vertical)  ? dpos[1] / _scale[1] : dpos[0] / _scale[0] ;
    return mod * _vrange; 
}

void
UIScrollBar::handleMouse(const InputEvent &e) {  
    _selected = false;
    if ( e.checkID(_id) ) { 
        if ( e.state == input_DOWN ) _selected = true;
        InputEvent sub = e;
        sub.popStack();
	    _box.handleMouse ( sub );
    }
}


UIMenu::UIMenu() { 
	_options.clear();
	_selected = -1;
}

void
UIMenu::draw() { 
	filledRounded(0.05);
	glPushMatrix();
	for ( int i = 0 ; i < _options.size() ; i++ ) { 
		glTranslated(0, -1.0, 0 );
		glPushMatrix();
		scaleFont(1.0);
		drawString( _options[i].name.c_str() );
		glPopMatrix();
	}
	glPopMatrix();
	outlineRounded(0.05);
}

bool WindowHandler::handleKey(const InputEvent &e) { return false; }
bool WindowHandler::handleSpec(const InputEvent &e) { return false; }




WindowManager::WindowManager() :
    _wcount(0),
    _topWindow(NULL),   
    _handler(NULL),
    _mexpose(false)
{ 
    _rootWindow.moveto(-1.0, 1.0);
    _rootWindow.resize(2.0, 2.0);
    _inp.setTime(GetTime());
}

void
WindowManager::setHandler(WindowHandler *h) { 
    _handler = h ;
    h->setWM(this);
}

int
WindowManager::findWindow(DisplayWindow * w ) { 
    for ( int i=0; i < _windows.size(); i++)
        if ( _windows[i] == w ) return i;
    return -1;
}

void
WindowManager::addWindow(DisplayWindow *w) 
{ 
    if ( findWindow(w) < 0 ) { 
        w->setID( IDManager::instance()->getPickID());
        _windows.push_back(w);
    }
}

void
WindowManager::removeWindow( DisplayWindow * w ) { 
    int wi;
    if ( ( wi = findWindow(w) ) < 0 ) return;
    _windows.erase(_windows.begin() + wi );
    w->exit();
    
}

void
WindowManager::bound( DisplayWindow * w ) { 
    Point2D accel ( 0, 0  ); 
    UIRect * wb = w->base();
    //bound
    if ( wb->bottom() > _rootWindow.top() )
        w->moveto( wb->x(), _rootWindow.top() + wb->h() );
    if ( wb->top() < _rootWindow.bottom() ) 
        w->moveto( wb->x(), _rootWindow.bottom() );
    if ( wb->right() < _rootWindow.left() ) 
        w->moveto( _rootWindow.left() - wb->w() , wb->y() );
    if ( wb->left() > _rootWindow.right() ) 
        w->moveto( _rootWindow.right() , wb->y() );
    //bounce
    if ( wb->top() > _rootWindow.top() )         accel += Point2D( 0.0, -0.1 );
    if ( wb->bottom() < _rootWindow.bottom() )   accel += Point2D( 0.0,  0.1 );
    if ( wb->left() < _rootWindow.left() )       accel += Point2D( 0.1,  0.0 ); 
    if ( wb->right() > _rootWindow.right() )     accel += Point2D(-0.1,  0.0 ); 
    
    w->setVel( w->getVel() + accel * 20.0 * _inp.dtime ) ;
    


}

void
WindowManager::draw() { 
    
    
    int i;
    int nw = _windows.size();
    DisplayWindow * w;

    for ( i= nw-1; i >= 0 ; i-- ) { 
        if ( _windows[i]->closing() ) removeWindow ( _windows[i] );
    }

    nw = _windows.size();

    _inp.setTime(GetTime());


    for ( i=0 ; i < nw  ; i++) { 

        w = _windows[i];
        if ( w != _topWindow || !_inp.isDepressed ) { 
            bound ( w );
            _windows[i]->drift( _inp.dtime );
        }
        
        if ( _mexpose ) { 
            Point2D center = w->base()->center();
            Vec2D dp = center - _inp.lastPos;
            double clen = dp.normalize();
            double len = w->base()->douter(_inp.lastPos);
            double rot = max ( 0, min ( 1.0, 1.2 * ( len - 0.05 ) ) );
            Point2D ncenter = center;
            if ( w != _topWindow && rot > 0 ) { 
                glPushMatrix();
                glTranslated( ncenter[0], ncenter[1], 0 ); //where center should be
                glRotated( rot * 90.0 , dp[1], -dp[0], 0 ); //rotate on our axis
                glTranslated(-center[0], -center[1], 0 ); // map center back to origin of rotation
                //drawwindow draws in base's coords..  soooooo....
                w->drawWindow();
                glPopMatrix();
            } 
            else w->drawWindow();
        }
		else { 
			if ( w == _topWindow ) w->drawWindow();
			else w->drawWindowInactive();
		}
    }

    if ( RenderMode == GL_RENDER ) drawCursor();
}

void
WindowManager::drawCursor() { 

	Point2D cp = _inp.lastPos + Point2D ( 0, -0.025);
	Point2D tp = cp + Point2D( 0, 0.05 );
	glPointSize(4);
	glColor4f( 0,0,0,0.7 );
	glBegin(GL_POINTS);
	glVertex2d( cp[0], cp[1] );
	glVertex2d( tp[0], tp[1] );
	glEnd();
	glLineWidth(2);
	glBegin(GL_LINES);
	glVertex2d( tp[0], tp[1] );
	glVertex2d( cp[0], cp[1] );
	glEnd();

}


void
WindowManager::selectWindow(uint * idstack, int stacksize) { 
//    _topWindow = NULL;
//    for ( uint i=0; !_topWindow && i < _windows.size(); i++ )  
//		if ( _windows[i]->checkIDList(idstack, stacksize) )  
//			setTopWindow ( _windows[i] );

}


void
WindowManager::setTopWindow( DisplayWindow * w ) { 
    if ( w ) { 
        for ( int i = 0; i < _windows.size() ; i++ ) { 
            if ( _windows[i] == w ) { 
                _windows.erase( _windows.begin() + i );
                _windows.push_back ( w );
                _topWindow = w;
                _topWindow->setVel(Point2D(0,0));
                return;
            }
        }
    }
}

void
WindowManager::handleEvent( const InputEvent & e ) { 

    if ( e.type == input_MOUSE ) 
        handleMouse(e);
    else if ( e.type == input_MOTION) 
        handleMotion(e);
    else if ( e.type == input_KEY ) 
        handleKey (e );
    else if ( e.type == input_SPEC ) 
        handleSpec(e);
}

void
WindowManager::handleMouse ( const InputEvent & e ) { 
    _inp.setEvent(e, e.pos);

    InputEvent sub = _inp.event;
    sub.popStack();

    if ( e.state == input_DOWN ) _topWindow = NULL;

    for ( uint i = 0 ; i < _windows.size(); i++ ) { 
        _windows[i]->handleMouse( sub );
        if ( e.state == input_DOWN && _windows[i]->selected() ) 
            setTopWindow ( _windows[i] );
    }
}


  

void
WindowManager::handleMotion(const InputEvent & e ) { 
    _inp.setEvent(e, e.pos );

    static double ltime = 0; 
    double dtime = GetTime() - ltime;
    ltime += dtime;
    dtime = max ( 0.01 , dtime );
    if ( _topWindow ) { 
        if ( _topWindow->draggable() && _inp.isDepressed ) { 
            UIRect * tBase = _topWindow->base();
            tBase->setPos( tBase->pos() + _inp.dPos );
            _topWindow->setVel( _inp.dPos / dtime );
        }
        else if ( _topWindow->resizeable() && _inp.isDepressed ) { 
            UIRect * tBase = _topWindow->base();
            if ( _inp.altDown ) { 
                Point2D np = _inp.lastPos - _topWindow->base()->pos();
                Point2D op = np - _inp.dPos; 

                _topWindow->rescale(    min ( 20, max ( 0.05, np[0] / op[0] )) , \
                                        min(  20, max ( 0.05, np[1] / op[1] )) );
            }
            else { 
                _topWindow->resize(tBase->w()+ _inp.dPos[0], tBase->h() - _inp.dPos[1] );
            }
        }
        else _topWindow->handleMotion(e);   
    }

}

void
WindowManager::handleKey(const InputEvent &e) { 
    _inp.setEvent(e, e.pos );
    if ( _handler && _handler->handleKey ( e ) ) {} 
    else if ( _topWindow ) _topWindow->handleKey( e );

}

void
WindowManager::handleSpec(const InputEvent &e) { 
    _inp.setEvent(e, e.pos );
    if ( _handler && _handler->handleSpec ( e ) ) {} 
    else if ( _topWindow ) _topWindow->handleSpec( e );

}


DisplayWindow::DisplayWindow() : 
_content    ( NULL ),
_orientation( Quaternion(1,0,0,0) ),
_scrollWidth ( 0.06 ),
_titleHeight ( 0.05 ),
_mesgHeight  ( 0.06 ),
_marginSize  ( 0.04 ),
_selected    ( false ),
_iconified	 ( false ),
_closing     (  0  ),
_velocity    ( Point2D ( 0, 0) ),
_id(0)
{  
    initShapes();
    _baseWindow.moveto( -0.7, 0.7 );
    resize(1.5 , 1.5 );
}


DisplayWindow::~DisplayWindow() { 
    IDManager::instance()->freeStencilID(_id); //release ID;
    if ( _content ) 
        delete _content;
}


void
DisplayWindow::close() { 
    _closing = 1;
}

bool
DisplayWindow::closing() { 
    return ( _closing != 0 );
}

void
DisplayWindow::exit() { 
    
    fprintf(stderr, "cleaning up window...\n");

}

void
DisplayWindow::clean() { 
    if ( _content ) { 
        _titleBox.setLabel( (char*)_content->title() );
        if ( _content->containerDirty() ) { 
		    reshape();
		    _content->containerDirty() = false;
        }
	}
}

void
DisplayWindow::drift( double time ) { 
    while ( time > 0 ) { 
        double step = min ( 0.010 , time );
        _velocity -= _velocity * ( 4.0 * step ) ;
        _baseWindow.setPos(_baseWindow.pos() + _velocity * step );
        time -= step;
    }
}

void
DisplayWindow::drawWindowInactive() {
	clean();

	if ( _iconified ) { drawIconified(); return; }

    if ( RenderMode == GL_RENDER ) 
        _baseWindow.dropShadowRounded( 0.15, 0.1);
	glPushName(_id);
		
	glLineWidth( 1.0 );
	drawBase();
	
	stencilContent();
	drawStenciledContent();
    
	glLineWidth(1);
        
	glPushMatrix();
	
		_baseWindow.transform(); //base window transform
		glLineWidth( 1.0 );
        drawUILayerInactive(); //button UI interface
     
    glPopMatrix();
    
	glPopName();
}

void
DisplayWindow::drawWindow() { 
    
    //XXX
    //contentwindow shouldn't scale,
    
    //   _baseWindow.print();
    clean();  //take care of any updates to buffer state
    
    if ( _iconified ) {  drawIconified(); return; } 

    if ( RenderMode == GL_RENDER ) 
        _baseWindow.dropShadowRounded( 0.35, 0.1);

    glPushName(_id);
    
    //window widgets
    //border
   
	glLineWidth( 3.0 );
    drawBase();	//draw window background
        
    stencilContent (); //set up content area stencil        
    drawStenciledContent(); //draw ( in stencil ) the window content
       
	glPushMatrix();
        
        _baseWindow.transform(); //base window transform
        //  _contentWindow.outlineQuad();//	content boundary  
		glLineWidth(2);
        drawStatus();  //status information
        drawScrollBars(); //scrollbar interaction
        

        drawUILayer(); //button UI interface
        
        glPopMatrix();
        
	glPopName();

}

void 
DisplayWindow::drawBase() { 
	//draw and outline the base rectangle. 
    _baseWindow.filledRounded(0.05);
    _baseWindow.outlineRounded(0.05);
}

void
DisplayWindow::stencilContent() { 

	//set the stencil area for the window's content;
    glColorMask(0,0,0,0);
    glDepthMask(0);
    
    glEnable(GL_STENCIL_TEST);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
    glStencilFunc(GL_ALWAYS, _content->id(), ~0);

    glPushMatrix();
     //stencil for content subwindow
    _baseWindow.transform(); //base window transform
    _contentWindow.filledQuad();

    _contentWindow.outlineQuad();
    glPopMatrix();
    
    //back to normal drawing mode
    glDisable(GL_STENCIL_TEST);
    glColorMask(1,1,1,1);
    glDepthMask(1);

}

void
DisplayWindow::drawUILayer() { 
    
    glLineWidth(2.0);
    //title 

    _titleBox.setHover( _selected );
    _titleBox.drawNamed(0.8);
    //buttons!
    _closeButton.drawNamed(0.7);
}

void
DisplayWindow::drawUILayerInactive() { 
	glLineWidth(1.0);
    _closeButton.drawNamed(0.7);
    _titleBox.setHover( false );
	_titleBox.drawNamed(0.8);
}

void
DisplayWindow::drawStatus() { 

    glLineWidth(1);
    
    //status message
    glPushMatrix();
    glTranslated( _baseWindow.w() * 0.5, -_baseWindow.h() + _marginSize * 0.5 , 0 );
    scaleFont( _mesgHeight * 0.75, 1.0 );
    drawString(_content->mesg());
    glPopMatrix();
    
    //position message
    glPushMatrix();
    glTranslated( _titleHeight , -_baseWindow.h() + _marginSize * 0.5, 0 );
    scaleFont( _mesgHeight * 0.75, 1.0 );
    drawString_mono(_content->posMesg());
    glPopMatrix();
    
}


void
DisplayWindow::drawScrollBars() { 

    //and resize button!
    glLineWidth ( 1 );
    //hscroll
    
    
    
    
    _hScrollBar.setRange (	_content->getX(), _content->getXSpan(), _content->getXRange() );
    _hScrollBar.draw();
    
    
    _vScrollBar.setRange (	_content->getY(), _content->getYSpan(), _content->getYRange() );
    _vScrollBar.draw();
    
    
    /*
      _hScrollBar.hLine();
      
      double w_range = max( _content->canvas().w(), _content->viewport().x() + _content->viewport().vpW() );
      if ( _content->viewport().vpW() <  w_range ) { 
        _hScrollBox.setl( _content->viewport().x() / w_range );
        _hScrollBox.setw( _content->viewport().vpW() / w_range );
        glPushMatrix();
        glPushName( _hScrollBox.id() );
        _hScrollBar.transform();
        _hScrollBox.filledRounded();
        _hScrollBox.outlineRounded();
        glPopName();
        glPopMatrix();
        }
    else { _hScrollBox.setl(0); _hScrollBox.setw(1); }
	*/    
    //vscroll

	/*
    _vScrollBar.vLine();
    
    double h_range = max( _content->canvas().h(), _content->viewport().y() + _content->viewport().vpH() );
    if ( _content->viewport().vpH() < h_range ) { 
        _vScrollBox.sett( _content->viewport().y() / h_range );
        _vScrollBox.seth( _content->viewport().vpH() / h_range );
        glPushMatrix();
            glPushName( _hScrollBox.id() );
            _vScrollBar.transform();
            _vScrollBox.filledRounded();
            _vScrollBox.outlineRounded();
            glPopName();
        glPopMatrix();
    }
    else { _vScrollBox.sett(0) ; _vScrollBox.seth(1);} 
    */

    glPushName( _sizeBox.id() );
    _sizeBox.filledRounded();
    _sizeBox.outlineRounded();
    glPopName();

}

void
DisplayWindow::drawIconified() { 
    
    //XXX

    //window widgets
    //border
    
    glPushName ( _id );
    glLineWidth(2);
    glPushMatrix();
    
    _baseWindow.transform(); //base window transform
	    
    //title box
	drawUILayer();
//	_titleBox.drawNamed();

    glPopMatrix();

    glPopName();
}

void 
DisplayWindow::drawStenciledContent() { 
    
    glPushMatrix();
    glPushName(_content->id());

    windowTransform();
    contentTransform();
    
    glEnable(GL_STENCIL_TEST);
    glStencilMask(0);
    glStencilFunc(GL_EQUAL, _content->id(), ~0);
    
    drawContent();
    
    glStencilMask(0xff);
    glDisable(GL_STENCIL_TEST);
    
    glPopName();
    glPopMatrix();

}

void
DisplayWindow::drawContent() { 
    _content->draw();
}
void
DisplayWindow::initShapes() { 
    
    _contentWindow.setScale(1.0, 1.0);
    
    Color4D border (0.45,0.45,0.45,1.0);    
    _baseWindow.setCols     (UI_BASE, Color4D(1.0,1.0,1.0,1.0),    border);
    _baseWindow.setCols     (UI_HOVER, Color4D(1.0,1.0,1.0,1.0),   border);
    _baseWindow.setCols     (UI_SEL, Color4D(1.0,1.0,1.0,1.0),     border);
    
    _contentWindow.setCols  (UI_BASE, Color4D(0,0.0,0.0, -1),      Color4D(0,0.0,0.0, 0.2));
    _contentWindow.setCols  (UI_HOVER, Color4D(0,0.0,0.0, -1),     Color4D(0,0.0,0.0, 0.2));
    _contentWindow.setCols  (UI_SEL, Color4D(0,0.0,0.0, -1),	   Color4D(0,0.0,0.0, 0.3));
    
    _titleBox.setLabel( "buffer:" );
    _titleBox.setCols       (UI_BASE, Color4D(1.0, 1.0,1.0,1.0 ),  border);
    _titleBox.setCols       (UI_HOVER,Color4D(0.9, 0.9, 1.0, 1.0 ), border);
    _titleBox.setCols       (UI_SEL,  Color4D(1.0, 1.0 ,1.0,1.0 ), Color4D(0,0.2,0.0, 1.0));
    
    _closeButton.setLabel("X");
    _closeButton.setCode (KEY_CTRL_W);
    _closeButton.setCols	(UI_BASE, Color4D(0.8,0.8,0.8,1.0), border ); 
    _closeButton.setCols	(UI_HOVER, Color4D(0.8,0.8,0.8,1.0), border ); 
    _closeButton.setCols	(UI_SEL, Color4D(0.7,0.7,0.7,1.0), border ); 
    
    _hScrollBar.setVertical( false );
    _hScrollBar.setCols     (UI_BASE, Color4D(0.8,0.8,0.8, 1.0),   Color4D(0.2,0.6,0.2, 0.8));
    _hScrollBar.setCols     (UI_HOVER, Color4D(0.9,0.9,0.8, 1.0),  Color4D(0.4,0.6,0.2, 0.8));
    
    _hScrollBar.setBoxCols     (UI_BASE, Color4D(0.8,0.8,1.0, 1.0) ,  Color4D(0.2,0.2,0.2, 0.9));
    _hScrollBar.setBoxCols     (UI_HOVER, Color4D(0.8,0.8,0.9, 1.0) , Color4D(0.0,0.0,0.0, 0.9));
    _hScrollBar.setBoxCols     (UI_SEL,  Color4D(0.8,0.8,1.0, 1.0) , Color4D(0.0,0.0,0.0, 1.0));
    
    _vScrollBar.setVertical( true );
    _vScrollBar.setCols     (UI_BASE, Color4D(0.8,0.8,0.8, 1.0),   Color4D(0.2,0.6,0.2, 0.8));
    _vScrollBar.setCols     (UI_HOVER, Color4D(0.8,0.8,0.8, 1.0),  Color4D(0.2,0.6,0.2, 0.8));
    
    _vScrollBar.setBoxCols     (UI_BASE, Color4D(0.8,0.8,1.0, 1.0),   Color4D(0.2,0.2,0.2, 0.9));
    _vScrollBar.setBoxCols     (UI_HOVER, Color4D(0.8,0.8,0.9, 1.0) ,  Color4D(0.0,0.0,0.0, 0.9));
    _vScrollBar.setBoxCols     (UI_SEL,  Color4D(0.8,0.8,1.0, 1.0) , Color4D(0.0,0.0,0.0, 1.0));
    
    _sizeBox.setCols        (_titleBox );
    
}

bool
DisplayWindow::draggable() { return ( _titleBox.selected() || _titleBar.selected() ) ; }

void
DisplayWindow::moveto(double x, double y ) { _baseWindow.moveto(x,y);}

bool
DisplayWindow::resizeable() { return _sizeBox.selected(); }

void 
DisplayWindow::resize(double width, double height) { 
    
	width  = max ( width, 2.0 * _titleHeight + 0.1 );
	height = max ( height, 2.0 * _titleHeight + 0.1 );
    _baseWindow.resize( width, height);
	reshape();
}

void
DisplayWindow::rescale( double scalex, double scaley )  { 
    _contentWindow.sx() *= scalex;
    _contentWindow.sy() *= scaley;
    setContentArea();
}

void
DisplayWindow::reshape() { 
    UIRectangle bW = _baseWindow;
    
    //title
//    _titleBox.moveto( 0, 0 );
//    _titleBox.resize( bW.w(), _titleHeight);
	_titleBar.moveto(0,0);
	_titleBar.resize(bW.w(), _titleHeight );

	double wide = bW.w() * 0.5;
	if ( _content ) wide = max ( wide , _titleHeight * 0.01 * drawString_length( _content->title() ) );
    _titleBox.moveto(bW.w() * 0.125, 1.2 *  _titleHeight );
	_titleBox.resize( wide , 2.0 * _titleHeight );

	//close button
	_closeButton.moveto ( _titleBox.left() - ( _marginSize + _titleBox.h() ) , _titleBox.top() );
	_closeButton.resize ( _titleBox.h(), _titleBox.h() );

    //content window
    _contentWindow.moveto( _marginSize , -_titleHeight );
    _contentWindow.resize( bW.w() - ( _scrollWidth + 2 * _marginSize ), bW.h() - ( _scrollWidth + _titleHeight + _mesgHeight ) );
    
    //hscroll
    _hScrollBar.moveto(_contentWindow.left(), _contentWindow.bottom());
    _hScrollBar.resize(_contentWindow.w(), _scrollWidth );
    _hScrollBar.selfScale();
    
    //vscroll
    _vScrollBar.moveto(_contentWindow.right(), _contentWindow.top());
    _vScrollBar.resize(_scrollWidth, _contentWindow.h() );
    _vScrollBar.selfScale();
    
    //sizebox
    _sizeBox.moveto(_contentWindow.right(), _contentWindow.bottom() );
    _sizeBox.resize(_scrollWidth, _scrollWidth );
    _sizeBox.selfScale();
    
    /*
    printf("##########resize##########");
    _baseWindow.print();
    _contentWindow.print();
    _titleBox.print();
    _hScrollBar.print();
    _vScrollBar.print();
    _sizeBox.print();
    */
    
    if ( _content ) setContentArea();
}


void 
DisplayWindow::handleKey( const InputEvent &e) {
//DisplayWindow::handleKey( char c, Point2D pt) {
    _inp.setEvent(e,_baseWindow.toLocal(e.pos));
//    _inp.setKey(c);
    if ( _inp.ctrlDown ) { 
        if ( _inp.lastKey == KEY_CTRL_W ) { 
            close();
        }
    }
    if ( _content ) _content->handleKey(e);
}

void 
DisplayWindow::handleSpec( const InputEvent &e) { 
//DisplayWindow::handleSpec( int c, Point2D pt) { 
//    _inp.setSpec(c);
    _inp.setEvent(e,_baseWindow.toLocal(e.pos));
    if ( _content ) _content->handleSpec(e);
}


void
DisplayWindow::handleButton( int button ) { 
      bool tmp_ctrl = _inp.ctrlDown;

      InputEvent ev = InputEvent( input_KEY, _inp.lastPos , button );
      ev.setMods( input_CTRL );
      handleKey( ev );
      
      _inp.ctrlDown = tmp_ctrl;
}


void
DisplayWindow::handleMouseUI( const InputEvent &e) { 
//DisplayWindow::handleMouseUI( int button, int state, Point2D pt, uint * pick_stack, uint pick_size ) { 
    
    _titleBox.handleMouse   ( e );
    _titleBar.handleMouse   ( e );
	_closeButton.handleMouse( e );
    _hScrollBar.handleMouse ( e );
	_vScrollBar.handleMouse ( e );	    
	_sizeBox.handleMouse    ( e );

}

void
DisplayWindow::handleMouse( const InputEvent & e ) { 
//DisplayWindow::handleMouse( int button, int state, Point2D pt, uint * pick_stack, uint pick_size ) { 
    _inp.setEvent( e, _baseWindow.toLocal(e.pos));

    if ( e.state == input_DOWN ) { 
        _selected = e.checkID(_id);
    }

    InputEvent sub = _inp.event;
    sub.popStack();

    //we'll still call this on an empty stack in order to clear out earlier selections
    handleMouseUI ( sub ); 
    
    if ( _content ) { 
        sub.pos -= _contentWindow.pos();
        _content->handleMouse( sub );
    }
    //see that dragaction wasn't handled further upstream
    if ( e.checkID(_id) ) dragAction(); //handle drag n' drop

    if ( _selected ) { 


        if ( _closeButton.selected() ) 
            handleButton( _closeButton.getCode() );
        
        if (  _titleBox.selected() ) { 
		    if ( e.state == input_UP ) {
			    if ( e.button == input_RIGHT_BUTTON ) _iconified = !_iconified;
                _titleBox.getCode();
		    }

	    }
    }
    

}

void 
DisplayWindow::dragAction() { 
    DragManager  * dgm = DragManager::instance();
    if ( dgm->mode() == drag_is_Picking ) { 
        //dgm->setobject ( this , drag_DisplayWindow );
    } else if ( dgm->mode() == drag_is_Dropping ) { 
        //drag Actions             
    }

}
void
DisplayWindow::handleMotionUI() { 
//DisplayWindow::handleMotionUI() { 

    _titleBox.checkHover    (_inp.lastPos );
    _closeButton.checkHover (_inp.lastPos );
    
    _hScrollBar.checkHover  (_inp.lastPos );   
    _vScrollBar.checkHover  (_inp.lastPos );

    _sizeBox.checkHover     (_inp.lastPos );
}

void
DisplayWindow::handleMotion( const InputEvent &e ) { 
//DisplayWindow::handleMotion( Point2D pt) { 
    //   printf("incoming motion - "); pt.printnl();
    _baseWindow.checkHover(e.pos);
    _inp.setEvent ( e, _baseWindow.toLocal(e.pos));
    
    handleMotionUI();

    if ( _content->selected() ) {
        InputEvent sub = _inp.event;
        sub.pos -= _contentWindow.pos();
        _content->handleMotion( sub );
        return; //window does not continue with callback
    }
    else if ( _inp.isDepressed ) { 
        if ( _hScrollBar.selected() ) { 
            _content->setX(_content->viewport().x() + _hScrollBar.handleMotion(_inp.dPos) );
        }
        else if ( _vScrollBar.selected() ) { 
            _content->setY(_content->viewport().y() + _vScrollBar.handleMotion(_inp.dPos ) );
        }
   }
    
}

void
DisplayWindow::setContentArea() { 
    //sets the dimensions and scaling of the viewport
    //to match that of the displaywindow
    _content->setViewPort(_contentWindow); 

}

void
DisplayWindow::setContent(WindowContent * c) 
{ 
    if ( !c ) return;
    if ( c->id() == 0 ) 
        c->id() = IDManager::instance()->getStencilID(); 
    _content = c; 
    setContentArea();
}

void
DisplayWindow::setID(int id ) 
{
    _id = id;
    if ( _content && _content->id() == 0 ) 
        _content->id() = IDManager::instance()->getStencilID();
}


void
DisplayWindow::windowTransform() { 
    _baseWindow.transform();
    //static double m[16];
    //_orientation.genMatrix((double*)m);
    //glMultMatrixd((double*)m);
    
}

void
DisplayWindow::contentTransform() { 
    _contentWindow.translate(); //put the window in the right place.
}


double
WindowContent::getXRange() { 
    return max( _canvas.w() , _viewport.x() + getXSpan() ) ; 
}

double
WindowContent::getYRange() { 
    return max( _canvas.h() , _viewport.y() + getYSpan() ); 
}

void
WindowContent::setCanvas(UIRectangle & cv) { 
    _canvas = cv;
}

void
WindowContent::setViewPort(UIRectangle &vp) { 
    _viewport.setw(vp.w());
    _viewport.seth(vp.h());
    _viewport.setScale( vp.sx(), vp.sy() );
}

bool
WindowContent::checkIDList(uint * idlist, int n) 
{ 
  //different dynamics
  _selected = false;
  for ( int i = 0 ; i < n ; i++ ) { 
    if ( idlist[i] == _id ) { 
      //match lower on stack, but it's not our match...
      //are we selected or not?
      _selected = true;
      if ( i !=  n-1) { 
	//check subelements
      }
      else _selected = true;
    }
  }
  return _selected;
}
