//-----------------------------------------------------------------------------
// name: audicle_ui_console.cpp
// desc: interface for audicle face vmspace
//
// authors: Philip Davidson (philipd@cs.princeton.edu)
//          Ge Wang (gewang@cs.princeton.edu)
//          Perry R. Cook (prc@cs.princeton.edu)
//          Ananya Misra (amisra@cs.princeton.edu)
// date: Autumn 2004
//-----------------------------------------------------------------------------

#include "audicle_ui_console.h"
#include "audicle_ui_editor.h"
#include "audicle_nexus.h"
#include "audicle_font.h"

#include "digiio_rtaudio.h"
#include "audicle.h"
#include "stdlib.h"

std::vector < AlertBox * > ConsoleWindow::_alerts;

ConsoleWindow::ConsoleWindow() : 
    DisplayWindow(),
    _time_start( 0 ),
    _time_span ( 0 ),
    _active ( 0 ),
    m_time ( 0 )
{
    setContent( new ShellContent() );
    initShapes();
    _id = IDManager::instance()->getPickID();
}


void
ConsoleWindow::addAlert ( std::string mesg, Color4D col , double b , double e ) { 

    AlertBox * a = new AlertBox(mesg);
    a->setColor( col );
    a->setTime( b , e );
    _alerts.push_back( a ) ;

}

void
ConsoleWindow::addAlert( AlertBox * a ) { 
    _alerts.push_back( a ) ;
}


void
ConsoleWindow::cleanAlerts() { 

    for ( int i = _alerts.size()-1; i >= 0 ; i-- ) { 
        if ( _alerts[i]->dismissed() ) { 
            delete _alerts[i];
            _alerts.erase( _alerts.begin() + i );
        }
    }
}

void
ConsoleWindow::drawAlerts() { 
    cleanAlerts();
    glPushMatrix();
    glTranslated ( _alertBase[0], _alertBase[1] , 0.0 );
    for ( t_CKUINT a = 0 ; a < _alerts.size(); a++ ) { 
        _alerts[a]->draw();
        glTranslated (0, -1.2 * _alerts[a]->animh(), 0);
    }
    glPopMatrix();

}

void
ConsoleWindow::handleMouseAlerts ( const InputEvent & e ) { 

    for ( t_CKUINT a = 0 ; a < _alerts.size(); a++ ) { 
        _alerts[a]->handleMouse( e );
    }

    cleanAlerts();
}

void
ConsoleWindow::initShapes() 
{
    DisplayWindow::initShapes();

    Color4D border (0.4, 0.4, 0.4, 1.0  );

    _baseWindow.setCols( UI_BASE, Color4D (1.0,1.0,1.0,0.95), border ); 
    _cubeWindow.setCols( UI_BASE, Color4D (1.0,1.0,1.0,0.9), border ); 
    _contentWindow.setCols( UI_BASE, Color4D (1.0,1.0,1.0,0.9), border ); 

    _sizeBox.setCols ( UI_BASE, Color4D ( 1.0, 0.9,0.9, 1.0 ), border );

    _timeLabel.setLabel("now");
    _timeLabel.fitLabel();
    _timeLabel.setCols( UI_BASE, Color4D (0.8,1.0,0.8,1.0), border ); 
    _timeDisplay.setLabel("1:00:00.10313");
    _timeDisplay.fitLabel();
    _timeDisplay.setCols( UI_BASE, Color4D (1.0,1.0,1.0,1.0), border ); 

    _curLabel.setLabel ("cur");
    _curLabel.fitLabel ();
    _curLabel.setCols( UI_BASE, Color4D (1.0,1.0,0.8,0.9), border ); 
    _curDisplay.setLabel("1");
    _curDisplay.setCols( UI_BASE, Color4D (1.0,1.0,0.8,0.9), border ); 
    
    _prevLabel.setLabel( "prev" );
    _prevLabel.fitLabel();
    _prevLabel.setCols( UI_BASE, Color4D (1.0,1.0,1.0,1.0), border ); 
    _prevDisplay.setLabel("3");
    _prevDisplay.setCols( UI_BASE, Color4D (0.8,0.8,1.0,0.9), border ); 

    char foo[8];
    for ( int i = 0 ; i < 6 ; i++ ) { 
        sprintf(foo, "%d", i+1 );
        _cubeSides[i].setLabel( foo );
    }

    _cubeSides[0].setCols( UI_BASE, Color4D (1.0,1.0,0.8,0.9), border ); 
    _cubeSides[1].setCols( UI_BASE, Color4D (0.8,1.0,0.8,0.9), border ); 
    _cubeSides[2].setCols( UI_BASE, Color4D (0.8,0.8,1.0,0.9), border ); 
    _cubeSides[3].setCols( UI_BASE, Color4D (1.0,0.9,0.8,0.9), border ); 
    _cubeSides[4].setCols( UI_BASE, Color4D (1.0,0.8,0.8,0.9), border ); 
    _cubeSides[5].setCols( UI_BASE, Color4D (1.0,0.8,1.0,0.9), border ); 

}

void
ConsoleWindow::fit_to_window ( double xmin, double xmax, double ymin, double ymax ) { 

    double hsize = ymax - ymin;
    double hcon = hsize * 0.25; 

    _alertBase = Point2D ( xmin + 2.0 * _marginSize, ymax - _marginSize );

    _activePos  =  Point2D( xmin + _marginSize * 2.0,  ymin + hcon );
    _inactivePos = Point2D( xmin + _marginSize * 2.0,  ymin + _marginSize * 2.0 );

    resize ( (xmax -  xmin) - _marginSize * 4.0 , hcon - _marginSize * 2.0 );
    base()->setPos ( _activePos );
    
}

void
ConsoleWindow::reshape( ) { 

    DisplayWindow::reshape();

    UIRect &bw = _baseWindow;
    
    //these are all relative to basewindow coordinates! 
    double pad = _marginSize;
    double pad2 = 2.0 * _marginSize;

    double cspan =  max ( 1.5 * bw.h() - pad2, min ( 1.2, 0.4 * bw.w() ) );

    _cubeWindow.moveto ( pad, -pad );
    _cubeWindow.resize ( cspan - pad2 , bw.h() - pad2 );

    _contentWindow.moveto ( _cubeWindow.right() + pad2 , _cubeWindow.top() );
    _contentWindow.resize (  bw.w() - ( cspan + pad2 ) , _cubeWindow.h() );

    if ( _content ) setContentArea();


    _vScrollBar.moveto ( _contentWindow.right(), _contentWindow.top() );
    _vScrollBar.resize ( pad2, _contentWindow.h() );
    
    _hScrollBar.moveto ( _contentWindow.left(), _contentWindow.bottom() );
    _hScrollBar.resize ( pad , _contentWindow.w()  );

    _sizeBox.moveto ( -pad , - bw.h() + pad  );
    _sizeBox.resize ( pad2, pad2 );

    _curLabel.moveto ( cspan + pad, pad * 1.5 );
    _curLabel.resize ( pad2 * 3.0, pad * 3.0 );
    _curLabel.setLabel("current face");
    _curLabel.fitLabel();
    _curDisplay.moveto ( cspan - pad * 1.5, pad * 1.5 );
    _curDisplay.resize ( pad * 3.0 , pad * 3.0 );


    _prevLabel.resize ( pad2 * 3.0, pad2 );
    _prevLabel.setLabel( "previous face" );
    _prevLabel.fitLabel();
    _prevLabel.moveto ( cspan - ( _prevLabel.w()  + pad ) , - bw.h() + pad );

    _prevDisplay.moveto ( cspan - pad * 1.5, -bw.h() + pad * 1.25 );
    _prevDisplay.resize ( pad * 3.0 , pad * 2.5 );
    _prevDisplay.setLabel("0");

    double timex = cspan + 0.5 * ( bw.w() - cspan );

    _timeDisplay.moveto ( timex + pad, pad );
    _timeDisplay.resize ( pad2 * 3.0, pad2 );
    _timeLabel.resize ( pad * 3.0 , pad * 3.0 );
    _timeLabel.fitLabel ( );
    _timeLabel.moveto ( timex - ( pad + _timeLabel.w() * 0.5 ) , pad * 1.5 );
    _timeDisplay.setx( _timeLabel.right() - pad * 0.5 );

    double kdim = min ( _cubeWindow.h() * 0.25 , _cubeWindow.w() * 0.125 ); 
    for ( int i = 0 ; i < 6; i++ ) { //resize 
        _cubeSides[i].resize( kdim, kdim  );
    }

    //this is always the same. 
    //depending on the current face, each
    //cubeside will move to one of these positions...


    _cubePos[ Audicle::UP ]    = Point2D (  kdim,           0 );
    _cubePos[ Audicle::DOWN ]  = Point2D (  kdim, -2.0 * kdim );
    _cubePos[ Audicle::LEFT ]  = Point2D (  0,          -kdim );
    _cubePos[ Audicle::RIGHT ] = Point2D (  2.0 * kdim, -kdim );
    _cubePos[ Audicle::BACK ]  = Point2D (  3.0 * kdim, -kdim );
    _cubePos[ Audicle::CUR ]  = Point2D (  kdim,       -kdim );


    setCubes();

  
}

void
ConsoleWindow::setCubes() { 

    Audicle * audi = Audicle::instance(); 
    t_CKUINT cur = audi->look_here();

    double now = AudicleWindow::main()->get_current_time(); 
    if ( cur != _l_cur ) { 
        _last_cur = _l_cur;
        _cube_swapping = true;
        _cube_swap_span = 0.5;        

        if ( _cube_swapping ) 
        { 
            double w = ( now - _cube_swap_start ) / _cube_swap_span ;
            if ( w > 0 && w < 1.0 ) 
                for ( int i = 0 ; i < 6 ; i++ )  
                    _swapPos[i] = _swapPos[i].interp ( _cubeSides[i].pos() , w );
        }
        else 
            for ( int i = 0 ; i < 6 ; i++ ) 
                _swapPos[i] = _cubeSides[i].pos();
            
        _cube_swap_start = now;
    }
    _l_cur = cur;

    
    

    _cubeSides[ cur ].setPos( _cubePos[ Audicle::CUR ] );
    _cubeSides[ audi->look_from( cur, Audicle::UP ) ].setPos ( _cubePos[ Audicle::UP ] );
    _cubeSides[ audi->look_from( cur, Audicle::DOWN ) ].setPos( _cubePos[ Audicle::DOWN ] );
    _cubeSides[ audi->look_from( cur, Audicle::LEFT ) ].setPos (_cubePos[ Audicle::LEFT ] );
    _cubeSides[ audi->look_from( cur, Audicle::RIGHT ) ].setPos( _cubePos[ Audicle::RIGHT ] );
    _cubeSides[ audi->look_from( cur, Audicle::BACK ) ].setPos( _cubePos[ Audicle::BACK ] );

    if ( audi->face() )  
        _curLabel.setLabel ( (char*) audi->face()->name().c_str() );
    else 
        _curLabel.setLabel( "no face" );
    
    _curLabel.fitLabel( );
    _curLabel.setw( _curLabel.w() * 0.75 );

    static char buffer[256];
    sprintf(buffer, "%d", audi->look_here()+1 );
    _curDisplay.setLabel( buffer );


}

void
ConsoleWindow::clean() { 
    DisplayWindow::clean();
    m_time = AudicleWindow::main()->get_current_time();
//    fprintf(stderr, "clean-cb.. %d %f %f %f\n", _active, m_time, _time_start, _time_span);
    if ( _active ) {
        double tdiff = m_time - _time_start;
        if ( tdiff > 0 &&  tdiff < _time_span  ) { 
            double df = tdiff / _time_span;
            base()->setPos( _inactivePos.interp ( _activePos, df)  );
        }
        else 
            base()->setPos( _activePos );
    }
    else { 

        double tdiff = m_time - _time_start;
        if ( tdiff > 0 &&  tdiff < _time_span  ) { 
            double df = tdiff / _time_span;
            base()->setPos( _activePos.interp ( _inactivePos, df)  );
        }
        else 
            base()->setPos( _inactivePos );

    }
}

void
ConsoleWindow::activate() 
{ 
    if ( !_active ) { 
        _active = true;
        _time_start = m_time;
        _time_span = 0.15;
        _selected = true;
        selectContent();
    }
}

void
ConsoleWindow::deactivate() { 
    if ( _active ) { 
        _active = false;
        _time_start = m_time;
        _time_span = 0.35;
    }
}

void
ConsoleWindow::drawWindow() { 
    //XXX
    //contentwindow shouldn't scale,
    
    //   _baseWindow.print();
    clean();  //take care of any updates to buffer state
    
    if ( _iconified ) {  drawIconified(); return; } 
    
//    if ( AudicleWindow::main()->m_render_mode == GL_RENDER ) 
//        _baseWindow.dropShadowRounded( 0.35, 0.1);
    
    glPushName(_id);
    
    //window widgets
    //border
    
    glLineWidth( 5.0 );
    drawBase();	//draw window background

    glLineWidth( 3.0 );

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
ConsoleWindow::drawUILayer() {


    glLineWidth ( 3.0 );

//    _contentWindow.outlineRounded(0.05);
//    _cubeWindow.outlineRounded(0.05);

    Audicle* audi = Audicle::instance();

    double glomod =  fabs( -1.0 + 2.0 * ( m_time * 0.6  - floor(m_time * 0.6 ) ) );

    double xdiv = 0.5 * ( _cubeWindow.right() + _contentWindow.left() ) ;
    glBegin(GL_LINES);
    glColor4d ( 0.2,0.2, 0.2, 1.0 );
    glVertex2d ( xdiv, _cubeWindow.top() - _marginSize );
    glVertex2d ( xdiv, _cubeWindow.bottom() + _marginSize );
    glEnd();


    _sizeBox.outlineRounded();


    glLineWidth(1.5);

    _curLabel.draw( 0.75 );
    _curDisplay.draw( 0.75 );
    
    if ( _active ) { 
        _prevLabel.draw( 0.75 );
        _prevDisplay.draw( 0.75 );
    }

    static char buffer[256];
    double time = the()->shreduler()->now_system;
    int sr = Digitalio::sampling_rate(); //m_sampling_rate;

    int samp = (int) time;
    int sec = samp / sr;
    int min = ( sec / 60 ) ;
    int hr =  ( min / 60 ) ;
    int day = ( hr / 24 );

    samp = samp % sr;
    sec = sec % 60;
    min = min % 60;
    hr = hr % 24;

    if ( day ) sprintf(buffer, "%dd:%02d:%02d:%02d.%05d", day,  hr, min, sec, samp );
    else if ( hr ) sprintf(buffer, "%02d:%02d:%02d.%05d", hr, min, sec, samp );
    else if ( min ) sprintf(buffer, "%02d:%02d.%05d", min, sec, samp );
    else if ( sec ) sprintf(buffer, "%02d.%05d", sec, samp );
    else sprintf(buffer, "%05d", samp );

    _timeDisplay.setLabel( buffer ) ;
    _timeDisplay.fitLabel( 1 );
    _timeDisplay.setw( _timeDisplay.w() * 0.70 );
    _timeDisplay.filledRounded( 0.03 );
    _timeDisplay.drawLabel( 0.75, 1 );
    _timeDisplay.outlineRounded( 0.03 );

    glPushName( _timeLabel.id() );

    Color4D tdark = Color4D( 0.6, 0.9, 0.6, 1.0 );
    Color4D tglow = tdark.interp( _timeLabel.col(), glomod * 1.2  );
    tglow[3] = 1.0;

    _timeLabel.setCol( tglow );

    glBegin( GL_POLYGON );
    _timeLabel.roundVerts( );
    glEnd( );

    _timeLabel.drawLeadedLabel( 0.75, 0.15 );
    _timeLabel.outlineRounded( ); 

    glPopName();

    if ( _active ) { 

        glLineWidth( 3.0 );
        glPushMatrix();
        
        double cubh = _cubeSides[0].h();
        
        glTranslated ( _cubeWindow.center()[0] - cubh * 3.5 , _cubeWindow.center()[1] + cubh * 1.5, 0 );
        
        if ( _cube_swapping )  { 

            double w = ( m_time - _cube_swap_start ) / _cube_swap_span ;
            if ( w > 1.0 ) { 
                _cube_swapping = false ;
                for ( int i = 0 ; i < 6 ; i++ ) 
                    _swapPos[i] = _cubeSides[i].pos();
            }
            else if ( w > 0 ) 
            {
                for ( int i = 0; i < 6 ; i++ ) { 
                    
                    glPushMatrix();
                    glPushName( _cubeSides[i].id() );
                    Point2D animp = _swapPos[i].interp ( _cubeSides[i].pos() , w );
                    
                    glTranslated( animp[0] - _cubeSides[i].pos()[0], \
                                  animp[1] - _cubeSides[i].pos()[1], 0 );
                    
                    _cubeSides[i].drawQuad( 0.75 );
                    
                    glPopName();
                    glPopMatrix();
                }
            }
        }
        
        if ( !_cube_swapping ) {  
            for ( int i = 0 ; i < 6; i++ ) { 
                glPushName( _cubeSides[i].id() );
                _cubeSides[i].drawQuad( 0.75 );
                glPopName();
            }
        }

        t_CKUINT cur = audi->look_here();
        
        Point2D _cube_spot( cubh * 6.0 , -cubh * 1.5 );
        
        glPushMatrix();
        glTranslated ( _cube_spot[0], _cube_spot[1], 0 );
        glScaled  ( 1.5, 1.5, 1.5 );
        glRotatef ( -30, 1.0, 0.0, 0.0 );
        glRotatef ( 30, 0.0, 1.0 ,0.0 );
        
        glPushMatrix();
        glTranslated( 0, 0, -0.5 * cubh );
        glTranslated(-_cubeSides[cur].center()[0], -_cubeSides[cur].center()[1], 0.0  );
        glPushName( _cubeSides[cur].id() );
        
        
        
        Color4D dark = _cubeSides[cur].col().scale(0.75);
        Color4D glow = dark.interp( _cubeSides[cur].col(), glomod * 1.1  );
        glow[3] = 1.0;
        _cubeSides[cur].setCol( glow );
        
        glBegin( GL_QUADS);
        _cubeSides[cur].quadVerts( );
        glEnd( );
        
        _cubeSides[cur].drawLabel( 0.75 );
        _cubeSides[cur].outlineQuad( );
        
//    _cubeSides[cur].drawQuad( 0.75 );
        
        
        glPopName();
        glPopMatrix();
        
        glPushMatrix();
        glRotatef ( 90, 1.0 , 0, 0 );
        t_CKUINT up = audi->look_from( cur, Audicle::UP ) ;
        glTranslated( 0, 0, -0.5 * cubh );
        glTranslated(-_cubeSides[up].center()[0], -_cubeSides[up].center()[1], 0.0  );
        glPushName( _cubeSides[up].id() );
        _cubeSides[up].drawQuad( 0.75 );
        glPopName();
        glPopMatrix();
        
        glPushMatrix();
        glRotatef ( -90, 0.0 , 1.0 , 0 );
        t_CKUINT rt = audi->look_from( cur, Audicle::RIGHT ); 
        glTranslated( 0, 0, -0.5 * cubh );
        glTranslated(-_cubeSides[rt].center()[0], -_cubeSides[rt].center()[1], 0.0  );
        glPushName( _cubeSides[rt].id() );
        _cubeSides[rt].drawQuad( 0.75 );
        glPopName();
        glPopMatrix();
        
        glPopMatrix();
        
        glPopMatrix();

    }


}

void
ConsoleWindow::drawUILayerInactive() 
{
    
}

void
ConsoleWindow::drawStatus () { 

}

void
ConsoleWindow::handleMouseUI( const InputEvent & e ) 
{

    if ( e.state == ae_input_DOWN && _selected ) activate();

    for ( int i = 0 ; i < 6 ; i ++ ) { 
        if ( e.state == ae_input_DOWN && e.checkID(_cubeSides[i].id()) ) { 
            Audicle::instance()->move_to( i );
            setCubes();
        }
    }

    _timeLabel.handleMouse(e);
    _timeDisplay.handleMouse(e);
    _curLabel.handleMouse(e);
    _curDisplay.handleMouse(e);
    _prevLabel.handleMouse(e);
    _prevDisplay.handleMouse(e);
    _cubeWindow.handleMouse(e);
    _sizeBox.handleMouse(e);

    if ( e.state == ae_input_DOWN && _sizeBox.selected() ) { 
        deactivate();
    }


}

void
ConsoleWindow::handleMotionUI() 
{
}


void
ConsoleWindow::print_to_console ( std::string out ) { 
    if ( _content )
        ((ShellContent*)_content)->output ( out );
    
}
void
ConsoleWindow::handleSpec( const InputEvent & e ) 
{
    DisplayWindow::handleSpec ( e );
}


void
ConsoleWindow::handleKey( const InputEvent & e ) 
{
    DisplayWindow::handleKey ( e );
}



//ShellContent class


ShellContent::ShellContent() : 
    WindowContent(),
    _leading( 0.06 ),
    _fontScale( 0.045),
    _fontAspect( 0.9 ),
    _fontWeight( 1.8 ),
    _canvasDirty ( true ),
    _viewportDirty ( true )
{
    _loc.line = 0;
    _loc.chr = 0;
    _wSpace = drawString_length_mono ("m");
    _wTab = _wSpace * 4;
    _viewport.setScale( 1.1, 1.1 );
    _output = new TextBuffer();
    _id = IDManager::instance()->getStencilID();
    _prompt = "audicle";
}


void 
ShellContent::draw() { 
    _fixCanvas();
    _fixView();

    glPushMatrix();
       
    glPushMatrix();
    _viewport.setCols( UI_BASE, Color4D( 1.0,1.0,1.0, 0.8 ), Color4D(0.2, 0.5, 0.2, 0.8 ) );
    _viewport.viewTranslate();
    _viewport.filledQuad();
    glPopMatrix();
    
    _viewport.scale();
    _viewport.viewTranslate();

        
    glColor4d( 0,0,0,0.9);
    glLineWidth (2.0);

    glLineWidth ( _fontWeight );

    //translate to bottom to draw our input line
    glTranslated ( 0, - _viewport.vpH() + _leading * 0.333, 0 );

    glColor4d( 1.0,1.0,0.9,1.0);

    glBegin (GL_QUADS);
    glVertex2d (0.0, 0.0  );
    glVertex2d (0.0, _leading  );
    glVertex2d ( _viewport.vpW(), _leading  );
    glVertex2d ( _viewport.vpW(), 0.0  );
    glEnd();

    //draw current entry
    glColor4d( 0.0,0.0,0,1.0);
    glPushMatrix();
      bufferFont->scale ( _fontScale, _fontAspect );
 //     scaleFont_mono ( _fontScale, _fontAspect );
      glPushMatrix();
      drawPromptLine ( _entry );
      glPopMatrix();
      double ds = bufferFont->length( _prompt + "% " + _entry.substr( 0, _loc.chr ) );
      glTranslated ( ds , 0 , 0 );
      glLineWidth(2.0);
      glColor4d( 1.0,0.0,0,1.0);

      glBegin(GL_LINES);
      glVertex2d ( 0 , bufferFont->height() * -0.2 );
      glVertex2d ( 0 , bufferFont->height() *  1.2 );
      glEnd();

    glPopMatrix();


    if ( !_output->empty() ) { 
        int range = min ( _output->nlines(), 8 );
        glColor4d( 0,0.0,0,0.9);
        glTranslated( 0, _leading * ( range + 1 ) , 0 );
        for ( t_CKUINT i = _output->nlines()-range ; i < _output->nlines() ; i++ ) { 
            
            glTranslated ( 0, -_leading, 0 );
            glPushMatrix();
            bufferFont->scale ( _fontScale, _fontAspect );
            drawPromptLine( _output->line(i) );
            glPopMatrix();
            
        }
    }

    glPopMatrix();

}

void 
ShellContent::drawPromptLine( TextLine &s ) { 


    glLineWidth ( _fontWeight * 1.0 );
    bufferFont->draw_sub( _prompt + "% " ); // we lose kerning from one to the next, but otherwise this is pretty legal. 
    glLineWidth ( _fontWeight * 2.0 );
    bufferFont->draw_sub( s.str() );

}


void
ShellContent::handleKey(const InputEvent & e) { 
    
    switch ( e.key ) { 
    case SYS_BACKSPACE:
        if( _loc.chr > 0 ) { 
            _entry.remChar( _loc.chr );
            _loc.chr--;
        }
        break;
    case SYS_RETURN:
        processLine();
        break;
    default:
        _loc.chr = _entry.addChar( e.key, _loc.chr );
        break;
    }
    _canvasDirty = true;
}

void
ShellContent::processLine() { 

    _history.push_back( _entry );
    _hpos = _history.size();

    _output->addTextLine ( _entry );
//    TextSpan end = TextSpan ( _output->end(), _output->end() );
//    _output->insertSpan ( _entry.str() + "\n", end );

    make_system_call(_entry.str() );

    _entry = TextLine();
    _loc.chr = 0;



}


void
ShellContent::make_system_call (std::string s ) { 

    system ( s.c_str() );

    /* 
//up you go, to await a lady of more discriminatin' taste..
    int mypipe[2];
    //child writes to 1, parent reads from 0
    pid_t childpid;
    if ( pipe(mypipe) >=  0 ) { 
        if ( (childpid = fork()) < 0)
        {
            fprintf(stderr, "system - could not fork\n");
        }
        else if ( childpid == 0 )       
        {
            close( mypipe[0] );
            dup2(mypipe[1], 1);  close(mypipe[1]);
            system( s.c_str() );
            exit(0);
        }
        else
        {
            close(mypipe[1]);
            char buffer[1025];
            int nread = 0;
            while ( ( nread = read ( mypipe[0], (void*) buffer, sizeof(char) * 1024 ) )  >  0  ) { 
                buffer[nread] = '\0';
                output ( buffer );
            }           
            close ( mypipe[0] );
            
        }
    }
    */
}


void
ShellContent::cursor_left() { 
    _loc.chr = max ( 0, _loc.chr - 1 );
}

void
ShellContent::cursor_right() { 
    _loc.chr = min ( _entry.size(), _loc.chr+1 ); 
}

void
ShellContent::hist_up() { 
    if ( _hpos > 0 ) { 
        _entry = _history[--_hpos];
        _loc.chr = _entry.size();
    }    
}

void
ShellContent::hist_down() { 
    if ( _hpos < _history.size() - 1 ) { 
        _entry = _history[++_hpos];
    }
    else { 
        _hpos = _history.size();
        _entry = TextLine();
    }
    _loc.chr = _entry.size();
}

void
ShellContent::output ( std::string out ) { 

    TextSpan b_end (_output->end(), _output->end());
    _output->insertSpan(  out, b_end );

}

void
ShellContent::_fixView() { 
    if ( _viewportDirty ) { 


    }
}

void
ShellContent::_fixCanvas() {
 
    if ( _canvasDirty ) { 
        
        double w = bufferFont->length( _entry.str() ) ;  //max length of lines

        for ( int i = 0 ; i < _output->nlines(); i++ )
            w = max ( bufferFont->length( _output->line(i).str() ) , w ) ;

        w /= bufferFont->height();
        
        _canvas.resize( w * _fontScale * _fontAspect , \
                        1 + _output->nlines()*_leading ); //easy!
        

/*
        double w = drawString_length_mono( _entry.str() );  //max length of lines


        for ( int i = 0 ; i < _output->nlines(); i++ )
            w = max ( drawString_length_mono ( _output->line(i).str() ) , w ) ;
        
        _canvas.resize( w * 0.01 * _fontScale * _fontAspect , \
                        ( 1 + _output->nlines() ) * _leading ); //easy!
*/
        
        _canvasDirty = false;
    }

}

void
ShellContent::handleMouse(const InputEvent & e) { 
    
}

void
ShellContent::handleMotion(const InputEvent & e) { 
    
}

void
ShellContent::handleSpec(const InputEvent & e) { 
    // fprintf(stderr, "bleep\n");
    switch ( e.key ) { 
    case KEY_LEFTARROW : 
        cursor_left();
        break;
    case KEY_RIGHTARROW :
        cursor_right();
        break;
    case KEY_UPARROW : 
        hist_up();
        break;
    case KEY_DOWNARROW :
        hist_down();
        break;
    }
}






AlertBox::AlertBox( std::string s ) :
    _dismissed ( false ),
    _mscale ( 0.045 )
{ 
    setMessage ( s ) ;
    setColor ( Color4D(0.5 , 1.0, 0.5 ) );
}  

void
AlertBox::setTime( double b, double e ) { 
    double time = AudicleWindow::main()->get_current_time();
    _time_started = time + b; 
    _time_ends    = time + e; 
}

double _padding = 0.5;

void
AlertBox::draw() { 

    double time = AudicleWindow::main()->get_current_time();

    _anim = 1.0;
    if ( time < _time_started  ) { 
        _anim = 0.0;
        return;
    }
    else if ( time > _time_ends ) { 
        _anim = 0.0;
        _dismissed = true;
        return;
    }
    else if ( time < _time_started + 0.5 ) { 
        _anim = max ( 0, ( time - _time_started ) / 0.5 ) ;
    }

    else if ( time > _time_ends - 0.5 ) { 
        _anim = max ( 0 , ( _time_ends - time ) / 0.5 );
    }

    glPushMatrix();
    glPushName( _id );

    glScaled ( _anim, _anim, 1.0 );

    glScaled ( _mscale , _mscale , 1.0 );
//    glTranslated ( -center()[0] , -center()[1], 0 );

    if ( AudicleWindow::main()->m_render_mode == GL_RENDER ) { 
        filledRounded( 0.5 );
    
        glPushMatrix();
        glTranslated ( _padding * 2.0 , -_padding * 0.75 , 0.0 ); 
        glColor4d( 0,0,0,1.0 );
        glLineWidth( 1.2 );
        for ( int i = 0 ; i < _message.size() ; i++ ) { 
            glTranslated( 0, -1.2, 0 );
            glPushMatrix();
            labelFontMono->scale( 1.0, 1.0 );
            labelFontMono->draw( _message[i] );
            glPopMatrix();
        }
        glPopMatrix();
        outlineRounded(0.5);
    }   
    
    glPushName(_dismissButton.id() );
    _dismissButton.bubbleRounded();
    _dismissButton.outlineRounded();
    glPopName( );
    
    glPopName();
    glPopMatrix();
}

void 
AlertBox::handleMouse( const InputEvent & e ) { 
    UIRectangle::handleMouse( e );
    if ( _selected ) { 
        InputEvent sub = e;
        sub.popStack();
        _dismissButton.handleMouse ( sub ) ;
        if ( _dismissButton.selected() ) { 
            _time_ends = AudicleWindow::main()->get_current_time() + 0.5;
        }
    }
}

void
AlertBox::setMessage ( std::string s ) { 

    _message.clear();

    int n = s.length();
    int start, stop;
    start = s.find_first_not_of("\n");

    while ((start >= 0) && ( start < n ) ) { 
        stop = s.find_first_of("\n", start );
        if ((stop < 0 ) || ( stop > n ) ) stop = n;
        _message.push_back(s.substr(start, stop-start ) );
        start = s.find_first_not_of("\n", stop+1 );
    }

    double h = 1.2 * (double )  _message.size();
    double w = 0 ;

    for ( int i = 0 ; i < _message.size(); i++ ) { 
        w = max ( w, labelFontMono->length ( _message[i] ) );
    }

    w /= labelFontMono->height(); // scale to display size 

    moveto ( 0, 0);
    resize ( w + _padding * 3.0  , h + _padding * 2.0 );
    _dismissButton.setLabel ("X");
    _dismissButton.moveto( -1.0 , 0 ); 
    _dismissButton.resize( 1.5 , 1.5 ); 

}
