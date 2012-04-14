//-----------------------------------------------------------------------------
// name: audicle_ui_console.h
// desc: interface for audicle face vmspace
//
// authors: Philip Davidson (philipd@cs.princeton.edu)
//          Ge Wang (gewang@cs.princeton.edu)
//          Perry R. Cook (prc@cs.princeton.edu)
//          Ananya Misra (amisra@cs.princeton.edu)
// date: Autumn 2004
//-----------------------------------------------------------------------------
#ifndef __AUDICLE_UI_CONSOLE_H_
#define __AUDICLE_UI_CONSOLE_H__

#include "audicle_ui_windowing.h"
#include "audicle_text_buffer.h"

class AlertBox; 
class ShellContent;

class ConsoleWindow : public DisplayWindow { 

protected:

    static std::vector < AlertBox * >  _alerts;
    Point2D   _alertBase;

public:

    static void addAlert ( std::string mesg, Color4D col = Color4D(0.8,1.0, 0.8, 0.9), \
                           double b = 0.0, double e = 5.0 );
    static void addAlert(AlertBox * a);
    void cleanAlerts();
    void drawAlerts();
    void handleMouseAlerts( const InputEvent & e );

protected:

    t_CKUINT _cur; 

    UIButton _timeLabel;
    UIButton _timeDisplay;

    UIButton _curLabel;
    UIButton _curDisplay;

    UIButton _prevLabel;    
    UIButton _prevDisplay;

    UIRect   _cubeWindow;

    UIButton  _cubeSides[6];
    Point2D   _cubePos[6];

    Point2D   _activePos;
    Point2D   _inactivePos;
    t_CKBOOL  _active;
    double    _time_start;
    double    _time_span;

    double    m_time;

    t_CKUINT  _last_cur;
    t_CKUINT  _l_cur;
    t_CKBOOL  _cube_swapping;
    double    _cube_swap_start;
    double    _cube_swap_span;
    Point2D   _swapPos[6];

    ShellContent * _content;

public:

    ConsoleWindow();

    void    clean();
    void    fit_to_window ( double xmin, double xmax, double ymin, double ymax );
    void    reshape();
    virtual void initShapes();

    bool    active() { return ( !!_active ) ; }
    void    setCubes();

    void    activate();
    void    deactivate();

    void    drawWindow();
    void    drawUILayer();
    void    drawUILayerInactive();
    void    drawStatus();
 
    void    print_to_console  ( std::string out );
    void    handleKey      ( const InputEvent &e );
    void    handleSpec     ( const InputEvent &e );
    void    handleMotionUI ( );
    void    handleMouseUI  ( const InputEvent &e );

};

struct matchLoc { int pos; char element; };

class ShellContent : public WindowContent { 
protected:

    TextLine _entry;  //current line we are entering
    TextBuffer * _output; //buffer of all output
    std::vector < TextLine > _history; //recent entries
    int  _hpos;
    std::vector < matchLoc > _match;
    TextLoc _loc; //
    TextSpan _markSpan;

    double      _leading;
    double      _fontScale;
    double      _fontAspect;
    double	_fontWeight;
    
    double       _wSpace;
    double      _wTab;
    
    bool        _viewportDirty;
    void	_fixView();
    bool        _canvasDirty;
    void        _fixCanvas();
    
    std::string _prompt;

public:

    ShellContent();

    void cursor_left();
    void cursor_right();
    void hist_up();
    void hist_down();

    void output       ( std::string s );

    void handleKey    ( const InputEvent & e );
    void handleSpec   ( const InputEvent & e );
    void handleMouse  ( const InputEvent & e );
    void handleMotion  ( const InputEvent & e );

    void processLine();
    void make_system_call( std::string s);
    void draw();
    void drawPromptLine ( TextLine & s );

};

class AlertBox : public UIRectangle { 

protected:

    bool                        _dismissed;
    UIButton                    _dismissButton;
    std::vector < std::string > _message;
    double                      _time_started;
    double                      _time_ends;
    double                      _mscale; 
    double                      _anim;
public:

    AlertBox ( std::string s );
    bool dismissed() { return _dismissed; } 
    void draw();
    double animh() { return _anim * h() * _mscale ; }
    void handleMouse ( const InputEvent & e ) ;
    void setTime( double b, double e );
    void setColor(Color4D col) { 

        setCols( UI_BASE, col , Color4D( 0,0,0,1.0)  ); 
        setCols( UI_HOVER, col , Color4D( 0,0,0,1.0)  ); 
        setCols( UI_SEL, col , Color4D( 0,0,0,1.0)  ); 
        
        col = Color4D( 1, 0.2, 0.2 ,1 );
        _dismissButton.setCols( UI_BASE, col , Color4D( 0,0,0,1.0)  ); 
        _dismissButton.setCols( UI_HOVER, col , Color4D( 0,0,0,1.0)  ); 
        _dismissButton.setCols( UI_SEL, col , Color4D( 0,0,0,1.0)  ) ;

    }
    void setMessage( std::string s );

};


#endif 
