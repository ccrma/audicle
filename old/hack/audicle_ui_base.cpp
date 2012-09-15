//-----------------------------------------------------------------------------
// name: audicle_ui_base.cpp
// desc: user interface basic classes
//
// authors: Philip Davidson (philipd@cs.princeton.edu)
//          Ge Wang (gewang@cs.princeton.edu)
//          Perry R. Cook (prc@cs.princeton.edu)
//          Ananya Misra (amisra@cs.princeton.edu)
// date: Autumn 2004
//-----------------------------------------------------------------------------

#include "audicle_ui_base.h"
#include "audicle_gfx.h"
#include "audicle_font.h"

AudicleFont * labelFont;
AudicleFont * labelFontMono;

bool _ui_fonts_inited = false;

void init_UI_Fonts() { 
    if ( _ui_fonts_inited ) return;
#ifdef _USE_FTGL_FONTS_
    labelFont = AudicleFont::loadFont ( "FTGL:verdana.TTF" );
    labelFontMono = AudicleFont::loadFont ( "FTGL:LUCON.TTF" );
#else
    labelFont = AudicleFont::loadFont ( "OpenGL:variable" ); 
    labelFontMono = AudicleFont::loadFont ( "OpenGL:variable" ); 
#endif
    _ui_fonts_inited = true;

}

UIRectangle::UIRectangle() : 
	gRectangle(), 
        _scale   ( Point2D( 1,1 ) ), 
        _origin  ( Point2D( 0,0 ) ), 
        _selected(false), 
        _hover   (false) 
	{   
            _defColors();
            _id = IDManager::instance()->getPickID(); 
    }

UIRectangle::UIRectangle(double x,double y,double w,double h) : 
        gRectangle(x,y,w,h), 
        _scale   (Point2D(1,1)), 
        _origin  (Point2D( 0,0 )), 
        _selected(false), 
        _hover   (false)  
	{   
            _defColors(); 
            _id = IDManager::instance()->getPickID();
        }

void 
UIRectangle::handleMouse ( const InputEvent & e) { 
    _selected = false;
    if ( e.state == ae_input_UP || e.size == 0 ) return ; 
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
    glColor4dv( c.data() );
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
UIRectangle::center_at( Point2D pos ) { 
    glTranslated ( pos[0] - center()[0], pos[1]  - center()[1] , 0);
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
UIButton::drawLabel( float scale , int mono) { 
	glPushMatrix();
        vpTranslate();

        double fsiz = scale * 0.75; 
        double margin = 0.5 * ( 1.0 - fsiz );
        glTranslated( w() * 0.5, margin * h() , 0 );
        glColor4f(0.0,0.0,0.0, 0.9);

        AudicleFont * lf = ( mono ) ? labelFontMono : labelFont; 
        lf->scale ( fsiz * h() );
        lf->draw_centered ( _label );

/*
        if ( mono ) { 
            scaleFont_mono( fsiz * h(), 1.0 );
            drawString_centered_mono ( _label.c_str() );
        }
        else { 
            scaleFont( fsiz * h() , 1.0 );
            drawString_centered( _label.c_str() );
        }
*/
    
	glPopMatrix();

}

void
UIButton::drawLeadedLabel( float scale , float lift, int mono) { 
	glPushMatrix();
        vpTranslate();
        double fsiz = scale * 0.75; 
        double margin = 0.5 * ( 1.0 - fsiz );
        glTranslated( w() * 0.5, ( margin + lift*fsiz ) * h(), 0 );
        glColor4f(0.0,0.0,0.0, 0.8);
        
        AudicleFont * lf = ( mono ) ? labelFontMono : labelFont; 
        lf->scale ( fsiz * h() );
        lf->draw_centered ( _label );
/*
        if ( mono ) { 
            scaleFont_mono ( fsiz * h() , 1.0 );
            drawString_centered_mono ( _label.c_str() );
        }
        else { 
            scaleFont( fsiz * h() , 1.0 );
            drawString_centered(_label.c_str() );
        }
*/
	glPopMatrix();
}


void
UIButton::drawQuad ( float scale ) { 
    filledQuad();
    drawLabel( scale );
    outlineQuad();
    
}

void
UIButton::drawRounded ( float scale ) { 
    filledRounded();
    drawLabel( scale );
    outlineRounded();
}

void
UIButton::drawBubbled ( float scale ) { 
    bubbleRounded();
    drawLabel( scale );
    outlineRounded();
}

void
UIButton::draw( float scale ) { 
    drawBubbled ( scale );
    if ( _menu && _selected ) { 
        glPushMatrix();
        glTranslated( x(), y()-h(), 0 );
        glScaled ( h(), h() , 1.0   );
        _menu->draw();
        glPopMatrix();
    }
}

void 
UIButton::drawNamed( float scale)  
{ 
    glPushName( _id ); 
    draw(scale); 
    glPopName(); 
} 

t_CKUINT
UIButton::getCode() { 
    _selected = false ; 
    return ( _menu && _menu->selected() ) ? _menu->getCode() : _code;  
}

void
UIButton::handleMouse ( const InputEvent &e  ) { 
    _selected = e.checkID(_id);
    if ( _selected && _menu ) { 
        InputEvent sub = e;
        sub.popStack();
        _menu->handleMouse( sub );            
    }
}


void
UIButton::fitLabel( t_CKBOOL mono ) { 
    
    AudicleFont * lf = ( mono ) ? labelFontMono : labelFont; 
    double dlen = lf->length( _label ) / lf->height() ;
        
    setw( h() * max ( 1.0 , dlen )) ;
}

void
UIButton::fitXLabel( double tscale, t_CKBOOL mono ) { 
    fitLabel( mono );
    setw( w() * tscale * 0.75 + h() * 0.75);
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
        if ( e.state == ae_input_DOWN ) _selected = true;
        InputEvent sub = e;
        sub.popStack();
	    _box.handleMouse ( sub );
    }
}

menuItem::menuItem() { 
    name = "option";
    code = 0;
    reshape();        
}
menuItem::menuItem ( char * n, t_CKUINT c ) { 
    name = n;
    code = c;
    reshape();
}


void
menuItem::reshape()  
{ 
    button.resize(1.0,1.0);
    button.moveto(0.0,0.0);
    button.setCode( code );
    button.setLabel( name );
    button.fitXLabel( 0.75 );
    button.fillCol(Color4D ( 1.0, 1.0, 1.0, 0.8 ) );
    button.lineCol(Color4D ( 0,0,0,1.0) ) ;
}

UIMenu::UIMenu() { 
	_options.clear();
	_selected = -1;
    _id = IDManager::instance()->getPickID();
    fillCol(Color4D ( 0.8, 0.8, 1.0, 0.5 ) );
    lineCol(Color4D ( 0,0,0,1.0) ) ;
}

void
UIMenu::addOption(char *c, unsigned long code) 
{ 
    _options.push_back( menuItem( c, code ) );
    reshape();
}



void
UIMenu::draw() 
{ 
    glPushName( _id );
    filledRounded(0.5);
	outlineRounded(0.5);
    glPushMatrix();
	for ( t_CKUINT i = 0 ; i < _options.size() ; i++ ) { 
        glPushName( _options[i].button.id() );
        _options[i].button.drawRounded( 0.75 );
        glPopName();
		glTranslated(0, -1.0, 0 );
	}
	glPopMatrix();
    glPopName();
}

void
UIMenu::reshape() { 
    float w = 0;
    for ( int i=0 ; i < _options.size(); i++ ) { 
        w = max ( w, _options[i].button.w() );
    }
    moveto( 0, 0 );
    resize( w, _options.size() );
}

void
UIMenu::handleMouse( const InputEvent &e ) { 
    _selected = e.checkID ( _id );
    if ( _selected ) { 
        InputEvent sub = e;
        sub.popStack();
        for ( int i = 0; i < _options.size(); i++ ) 
            _options[i].button.handleMouse(sub);
    }
}

t_CKUINT
UIMenu::getCode() { 
    _selected = false; 
    for ( int i = 0; i < _options.size(); i++ )
        if ( _options[i].button.selected() ) {
            _default_code = _options[i].button.getCode();
            return _default_code;
        }
    return _default_code;
}
