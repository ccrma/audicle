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
// name: audicle_ui_windowing.cpp
// desc: ...
//
// authors: Philip Davidson (philipd@cs.princeton.edu)
//          Ge Wang (gewang@cs.princeton.edu)
//          Perry R. Cook (prc@cs.princeton.edu)
//          Ananya Misra (amisra@cs.princeton.edu)
// date: Autumn 2004
//-----------------------------------------------------------------------------
#include "audicle_ui_windowing.h"
#include "audicle_ui_console.h"
#include "audicle_font.h"

//#Includeo "image.h"
//write all that into a file

//DragManager and IDManager went to audicle_gfx.cpp

bool WindowHandler::handleKey(const InputEvent &e) { return false; }
bool WindowHandler::handleSpec(const InputEvent &e) { return false; }
DisplayWindow * WindowHandler::new_window() { return new DisplayWindow(); }


std::vector < AlertBox * > WindowManager::_alerts;
window_render_mode WindowManager::_rendermode = WINDOW_RENDER_BLEND ; 

WindowManager::WindowManager() :
    _wcount(0),
    _topWindow(NULL),   
    _handler(NULL),
    _mexpose(false),
    _last_time( 0 )
{ 
    _rootWindow.moveto(-1.0, 1.0);
    _rootWindow.resize(2.0, 2.0);
    _inp.setTime(AudicleWindow::main()->get_current_time() );
}

void
WindowManager::setHandler(WindowHandler *h) { 
    _handler = h ;
    h->setWM(this);
}

int
WindowManager::findWindow(DisplayWindow * w ) { 
    for ( t_CKUINT i=0; i < _windows.size(); i++)
        if ( _windows[i] == w ) return i;
    return -1;
}

void
WindowManager::addAlert ( std::string mesg, Color4D col , double b , double e ) { 

    ConsoleWindow::addAlert ( mesg, col, b , e );

//    AlertBox * a = new AlertBox(mesg);
//    a->setColor ( col );
//    a->setTime ( b , e );
//    _alerts.push_back( a ) ;
}

void
WindowManager::addAlert( AlertBox * a ) { 
    ConsoleWindow::addAlert ( a ) ;
//    _alerts.push_back( a ) ;
}

void
WindowManager::cleanAlerts() { 

    for ( int i = _alerts.size()-1; i >= 0 ; i-- ) { 
        if ( _alerts[i]->dismissed() ) { 
            delete _alerts[i];
            _alerts.erase( _alerts.begin() + i );
        }
    }
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
    bool isTop = false;
    if ( ( wi = findWindow(w) ) < 0 ) return;
    if ( w == _topWindow ) isTop = true;
    _windows.erase(_windows.begin() + wi );
    w->exit();

    if ( isTop && _windows.size() > 0 ) _topWindow = _windows.back( );
    
}

void
WindowManager::arrangeWindows() {

    double margin = _rootWindow.w() * 0.05; 
    if ( _windows.size() == 1 ) { 
        _windows[0]->moveto( _rootWindow.x() + margin , _rootWindow.y() - margin );
        _windows[0]->resize( _rootWindow.w() - 2.0*margin, _rootWindow.h() - 2.0*margin );
    }
    else { 
        if ( _topWindow ) { 
            _topWindow->moveto( _rootWindow.x() + margin , _rootWindow.y() - margin );
            _topWindow->resize( _rootWindow.w() * 0.5 , _rootWindow.h() - 2.0 * margin );
        }
        int num = ( _topWindow ) ? _windows.size() -1 : _windows.size() ;
        int nc = (int) sqrt ( (float)num ) ;
    int nr = num / nc;
        double rdiv = 1.0 / (double)nr;
    double cdiv = 1.0 / (double)nc;

        double homex = ( _topWindow ) ? _rootWindow.center()[0] : _rootWindow.x() ; 
        double homey = _rootWindow.y() -  margin ;
        double hwidth = ( _topWindow ) ? _rootWindow.w() * 0.5   : _rootWindow.w();
        double hheight = _rootWindow.h() - 2.0 * margin ;

        int n = 0;
        for ( t_CKUINT i=0 ; i < _windows.size() ; i++ ) { 
            if ( _windows[i] != _topWindow ) { 
                int r  = n / nc;
                int c  = n % nc;
                _windows[i]->moveto( homex + c * hwidth * cdiv, homey - r * hheight * rdiv );
                _windows[i]->resize( hwidth * cdiv - margin , hheight * rdiv );
                n++;
            }
        }

    }

}
void
WindowManager::bound( DisplayWindow * w ) { 

    UIRect * wb = w->base();
    Point2D accel ( 0, 0  ); 
    
    //bound
    if ( wb->bottom() > _rootWindow.top() ) { 
        w->moveto( wb->x(), _rootWindow.top() + wb->h() );
    w->setVel( w->getVel().mult(Point2D ( 1.0 , 0.0 )) );
    }
    if ( wb->top() < _rootWindow.bottom() ) { 
        w->moveto( wb->x(), _rootWindow.bottom() );
    w->setVel( w->getVel().mult(Point2D ( 1.0 , 0.0 )) );
    }
    if ( wb->right() < _rootWindow.left() ) { 
        w->moveto( _rootWindow.left() - wb->w() , wb->y() );
        w->setVel( w->getVel().mult(Point2D ( 0.0 , 1.0 )) );
    }
    if ( wb->left() > _rootWindow.right() ) { 
        w->moveto( _rootWindow.right() , wb->y() );
    w->setVel( w->getVel().mult(Point2D ( 0.0 , 1.0 )) );
    }


    if ( wb->h() > _rootWindow.h() ) { 
        w->resize( wb->w(), _rootWindow.h() );    
    }

    if ( wb->w() > _rootWindow.w() ) { 
        w->resize( _rootWindow.w() , wb->h() );    
    }



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
        if ( _windows[i]->closing() ) removeWindow( _windows[i] );
    }

    nw = _windows.size();

    _inp.setTime(AudicleWindow::main()->get_current_time());

    glPushMatrix();

    glTranslated ( 0 , 0,  (float)(nw-1) * -0.06 );
    for ( i = 0 ; i < nw ; i++ ) { 

        w = _windows[i];
        if ( w != _topWindow || !_inp.isDepressed ) { 
            bound ( w );
            _windows[i]->drift( _inp.dtime );
        }
        
        
        if ( w == _topWindow ) w->drawWindow();
        else w->drawWindowInactive();
        glTranslated( 0,0, 0.06);   
    }

    glPopMatrix();
    
    cleanAlerts();
    glPushMatrix();
    glTranslated ( _rootWindow.x() + 0.1, _rootWindow.y() - 0.1 , 0.0 );
    for ( t_CKUINT a = 0 ; a < _alerts.size(); a++ ) { 
        _alerts[a]->draw();
        glTranslated (0, -1.2 * _alerts[a]->animh(), 0);
    }
    glPopMatrix();
    
    if ( AudicleWindow::main()->m_render_mode == GL_RENDER ) drawCursor();
}

void
drawArrow( Point3D cur_pt, Point3D prev_pt, Point3D orig_pt, Color4D linecol, Color4D highlight ) { 
    //cur_pt  is the current position 
    //prev_pt is the previous position.
    //orig_pt is origin of the line;

    static Point3D p1 = Point3D(0,0);
    static Point3D p2 = Point3D(0,0);
    static Point3D p3 = Point3D(0,0);
    static Point3D * pv[3] = { &p1, &p2, &p3 };

    if ( ! ( cur_pt == orig_pt ) ) { //only draw the arrow head if we're not on the origin..

        Vec2D dir = ( orig_pt - cur_pt ).unit() * 0.08;
        Vec2D orth = Point2D( - dir[1], dir[0] ) * 0.4 ;
        
        p1 = cur_pt + dir;
        p2 = p1 + ( prev_pt - cur_pt ) * 2.0 ;
        p3 = orig_pt;

        glLineWidth (2.0);    
        // 'line' color
        glColor4dv ( linecol.data() );
        glBegin(GL_LINE_STRIP); //0.025
        for ( int ri = 0 ; ri <= 40 ; ri++) { //draw bezier...
            glVertex2dv( p1.bezier_interp_pt( (Point3D **)&pv, 3, 0.025 * ri ).data() );
        }
        glEnd();
        

        glBegin( GL_TRIANGLE_FAN ); 

        glVertex2dv( cur_pt.data());
        glVertex2dv( (p1+orth).data() );
        
        // 'highlight' color in center
        glColor4dv( highlight.data() );
        glVertex2dv( (p1+ dir*0.25).data() ); 

        glColor4dv( linecol.data() );
        glVertex2dv( (p1-orth).data() );
        glEnd();
    }
}

void
WindowManager::drawCursor() { 

    //do we want the arrow?
    Color4D linecol = Color4D( 0, 0.3, 0.3, 0.9 );
    Color4D highcol = Color4D( 0.5, 0.8, 0.8, 0.9 );
    
    if (    AudicleWindow::main()->m_render_mode == GL_RENDER  && \
            DragManager::instance()->mode() == ae_drag_is_Holding ) 
    { 
        drawArrow( _cur_pt, _last_pt, _pick_pt, linecol, highcol );
    }

    else { 
        Point2D cp = _inp.lastPos + Point2D ( 0, -0.025);
        Point2D tp = cp + Point2D( 0, 0.05 );

        glPointSize(4);
        glColor4f( 0,0,0,0.7f );
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
}

void
WindowManager::selectWindow(t_CKUINT * idstack, t_CKUINT stacksize) { 

}


void
WindowManager::setTopWindow( DisplayWindow * w ) { 

    if ( w == _topWindow ) return;
    int wi = findWindow ( w );
    if ( wi < 0 ) { 
        _topWindow = NULL;
        return;    
    }

    _windows.erase( _windows.begin() + wi );
    _windows.push_back ( w );

    _topWindow = w;
    _topWindow->setVel(Point2D(0,0));

    _topWindow->selected() = true;
    _topWindow->selectContent();
    
    return;
}

void
WindowManager::handleEvent( const InputEvent & e ) { 

    if ( e.type == ae_input_MOUSE ) 
        handleMouse(e);
    else if ( e.type == ae_input_MOTION) 
        handleMotion(e);
    else if ( e.type == ae_input_KEY ) 
        handleKey (e );
    else if ( e.type == ae_input_SPEC ) 
        handleSpec(e);
}

void
WindowManager::handleMouse ( const InputEvent & e ) { 
    _inp.setEvent(e, e.pos);
    setCur ( e.pos ) ;


    InputEvent sub = _inp.event;
    sub.popStack();

    double toss_timeout = 0.1;
    
    if ( _topWindow ) { 
        if ( _topWindow->draggable() && e.state == ae_input_UP ) { 
            if ( e.time > _last_time + toss_timeout )  { 
                _topWindow->setVel(Point2D(0,0));
            }
        }
    }

    DisplayWindow * last_top = _topWindow;

    if ( e.state == ae_input_DOWN ) _topWindow = NULL;
    for ( t_CKUINT i = 0 ; i < _windows.size(); i++ ) { 
        _windows[i]->handleMouse( sub );
        if ( e.state == ae_input_DOWN && _windows[i]->selected() ) { 
            if ( _windows[i] != last_top ) 
                setTopWindow ( _windows[i] );
            else 
                _topWindow = _windows[i];
        }
    }

    for ( t_CKUINT a = 0 ; a < _alerts.size(); a++ ) { 
        _alerts[a]->handleMouse( sub );
    }
    cleanAlerts();

}


  

void
WindowManager::handleMotion(const InputEvent & e ) { 
    _inp.setEvent(e, e.pos );
    setCur ( e.pos );

    //    fprintf(stderr, "motion %d - %f - ", _inp.isDepressed , e.time);
    //e.pos.printnl();

    double dtime = AudicleWindow::main()->get_current_time() - _last_time;
    _last_time += dtime;
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
                double dx = ( np[0] > 0 && op[0] > 0 ) ? min ( 20, max ( 0.05, np[0]/op[0] ) ) : 1.0 ;
                double dy = ( np[1] < 0 && op[1] < 0 ) ? min ( 20, max ( 0.05, np[1]/op[1] ) ) : 1.0 ; 
                _topWindow->rescale( dx, dy );
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
_iconified   ( false ),
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
    

    bool blending = ( WindowManager::getRenderMode() == WINDOW_RENDER_BLEND );

    if ( _iconified ) { drawIconified(); return; }
    
    if ( AudicleWindow::main()->m_render_mode == GL_RENDER 
         && blending ) 
        _baseWindow.dropShadowRounded( 0.15f, 0.1f);
    
    glPushName(_id);
        
    glLineWidth( 1.0 );
    drawBase();
    
    stencilContent();
    drawStenciledContent();
            
    glPushMatrix();
    
        glLineWidth( 1.0 );

        if ( !blending ) glTranslated ( 0, 0, 0.01 );

        _baseWindow.transform(); //base window transform
    
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
    
    bool blending = ( WindowManager::getRenderMode() == WINDOW_RENDER_BLEND );
        
    if ( _iconified ) {  drawIconified(); return; } 
    
    if ( AudicleWindow::main()->m_render_mode == GL_RENDER 
         && blending ) 
        _baseWindow.dropShadowRounded( 0.35f, 0.1f );
    
    glPushName(_id);
    
    //window widgets
    //border
    
    glLineWidth( 3.0 );

    drawBase(); //draw window background
    
    stencilContent (); //set up content area stencil        
    drawStenciledContent(); //draw ( in stencil ) the window content
    
    glPushMatrix();
    
        _baseWindow.transform(); //base window transform
        //  _contentWindow.outlineQuad();// content boundary  

        glLineWidth(2);

        drawStatus();  //status information

        if ( !blending )
            glTranslated ( 0, 0, 0.01 );

        drawScrollBars(); //scrollbar interaction
    
        drawUILayer(); //button UI interface

    glPopMatrix();
    
    glPopName();

}

void 
DisplayWindow::drawBase() { 
    //draw and outline the base rectangle. 
    _baseWindow.filledRounded(0.05f);
    _baseWindow.outlineRounded(0.05f);
}

void
DisplayWindow::stencilContent() { 

    //set the stencil area for the window's content;
    glPushAttrib ( GL_DEPTH_BUFFER_BIT );

    glColorMask(0,0,0,0);
    glDepthMask(0);
    
    glDisable(GL_DEPTH_TEST);

    glEnable(GL_STENCIL_TEST);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
    glStencilFunc(GL_ALWAYS, _content->id(), 0xffffffff);

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
    glPopAttrib();
}

void
DisplayWindow::drawUILayer() { 
    
    glLineWidth(2.0);
    //title 

    _titleBox.setHover( _selected );
    _titleBox.drawNamed(0.8f);
    //buttons!
    _closeButton.drawNamed(0.7f);
}

void
DisplayWindow::drawUILayerInactive() { 
    glLineWidth(1.0);
    _closeButton.drawNamed(0.7f);
    _titleBox.setHover( false );
    _titleBox.drawNamed(0.8f);
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
    scaleFont_mono( _mesgHeight * 0.75, 1.0 );
    drawString_mono(_content->posMesg());
    glPopMatrix();
    
}


void
DisplayWindow::drawScrollBars() { 

    //and resize button!
    glLineWidth ( 1 );
    //hscroll
    
    _hScrollBar.setRange (  _content->getX(), _content->getXSpan(), _content->getXRange() );
    _hScrollBar.draw();
    
    //vscroll
    _vScrollBar.setRange (  _content->getY(), _content->getYSpan(), _content->getYRange() );
    _vScrollBar.draw();
        
    glPushName( _sizeBox.id() );
    _sizeBox.filledRounded();
    _sizeBox.outlineRounded();
    glPopName();

}




void
DisplayWindow::drawIconified() { 
    
    //XXX

    glPushName ( _id );
    glLineWidth(2);
    glPushMatrix();
    
    _baseWindow.transform(); //base window transform
        
    //title box
    drawUILayer();
//  _titleBox.drawNamed();

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
    glStencilFunc(GL_EQUAL, _content->id(), 0xffffffff);
    
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
    _contentWindow.setCols  (UI_SEL, Color4D(0,0.0,0.0, -1),       Color4D(0,0.0,0.0, 0.3));
    
    _titleBox.setLabel( "buffer:" );
    _titleBox.setCols       (UI_BASE, Color4D(1.0, 1.0,1.0,1.0 ),  border);
    _titleBox.setCols       (UI_HOVER,Color4D(0.9, 0.9, 1.0, 1.0 ), border);
    _titleBox.setCols       (UI_SEL,  Color4D(1.0, 1.0 ,1.0,1.0 ), Color4D(0,0.2,0.0, 1.0));
    
    _closeButton.setLabel("X");
    _closeButton.setCode (KEY_CTRL_W);
    _closeButton.setCols    (UI_BASE, Color4D(0.8,0.8,0.8,1.0), border ); 
    _closeButton.setCols    (UI_HOVER, Color4D(0.8,0.8,0.8,1.0), border ); 
    _closeButton.setCols    (UI_SEL, Color4D(0.7,0.7,0.7,1.0), border ); 
    
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
DisplayWindow::draggable() { return ( ( _titleBox.selected() || _titleBar.selected() ) 
                      || ( _selected && _inp.ctrlDown && !_sizeBox.selected() ) ); }

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

    _contentWindow.sx() = max ( 0.25, _contentWindow.sx() * scalex );
    _contentWindow.sy() = max ( 0.25, _contentWindow.sy() * scaley );
//    fprintf ( stderr, "content scaled to: %f %f\n", _contentWindow.sx(), _contentWindow.sy() );
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
    _titleBox.moveto(bW.w() * 0.125, 1.2 *  _titleHeight );
    _titleBox.resize( wide , 2.0 * _titleHeight );
    if ( _content ) _titleBox.setLabel ( _content->title() );
    _titleBox.fitXLabel( 0.9 );
    _titleBox.setw( max ( _titleBox.w(), wide ) );

    //close button
    _closeButton.moveto ( _titleBox.left() - ( _marginSize + _titleBox.h() ) , _titleBox.top() );
    _closeButton.resize ( _titleBox.h(), _titleBox.h() );
    
    //content window
    _contentWindow.moveto( _marginSize , -_titleHeight );
    _contentWindow.resize( bW.w() - ( _scrollWidth + 2 * _marginSize ), \
                           bW.h() - ( _scrollWidth + _titleHeight + _mesgHeight + _marginSize * 0.5 ) );
    
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

    _inp.setEvent(e,_baseWindow.toLocal(e.pos));
    if ( !_selected ) return;
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

      InputEvent ev = InputEvent( ae_input_KEY, _inp.lastPos , button );
      ev.setMods( ae_input_CTRL );
      handleKey( ev );
      
      _inp.ctrlDown = tmp_ctrl;
}


void
DisplayWindow::handleMouseUI( const InputEvent &e) { 

    
    _titleBox.handleMouse   ( e );
    _titleBar.handleMouse   ( e );
    _closeButton.handleMouse( e );
    _hScrollBar.handleMouse ( e );
    _vScrollBar.handleMouse ( e );      
    _sizeBox.handleMouse    ( e );
    
}

void
DisplayWindow::handleMouse( const InputEvent & e ) { 

    _inp.setEvent( e, _baseWindow.toLocal(e.pos));

    if ( e.state == ae_input_DOWN ) { 
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

        if ( _closeButton.selected() && e.state == ae_input_UP ) 
            handleButton( _closeButton.getCode() );
        
        if (  _titleBox.selected() ) { 
            if ( e.state == ae_input_UP ) {
                if ( e.button == ae_input_RIGHT_BUTTON ) _iconified = !_iconified;
                _titleBox.getCode();
            }

        }
    }
    

}

void 
DisplayWindow::dragAction() { 
    DragManager  * dgm = DragManager::instance();
    if ( dgm->mode() == ae_drag_is_Picking ) { 
        //dgm->setobject ( this , drag_DisplayWindow );
    } else if ( dgm->mode() == ae_drag_is_Dropping ) { 
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
//    _content->refreshView();
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
DisplayWindow::selectContent() { 
    if ( _content ) 
    {
        _content->selected() = true;
    }

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
WindowContent::checkIDList(t_CKUINT * idlist, int n) 
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
