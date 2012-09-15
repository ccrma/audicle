#ifndef __AUDICLE_UI_WINDOWING_H__
#define __AUDICLE_UI_WINDOWING_H__

#include "audicle_def.h"
#include "audicle_gfx.h"
#include "audicle_event.h"
#include "audicle_ui_base.h"

class DisplayWindow;
class AlertBox; 
class WindowContent;

class WindowHandler;

enum window_render_mode { WINDOW_RENDER_BLEND, WINDOW_RENDER_DEPTH }; 
class WindowManager { 
protected:
    static    std::vector < AlertBox * >  _alerts;
    
    WindowHandler *             _handler;

    static window_render_mode   _rendermode; 

    UIRect                      _rootWindow;
    std::vector < DisplayWindow * > _windows;

    DisplayWindow *             _topWindow;

    InputState                  _inp;
    int                         _wcount;
    bool                        _mexpose;

    Point2D                     _cur_pt;
    Point2D                     _last_pt;
    Point2D                     _pick_pt;

    double                      _last_time;

    t_CKUINT                        _id;

public:

    static void addAlert ( std::string mesg, Color4D col = Color4D(0.8,1.0, 0.8, 0.9), double b = 0.0, double e = 5.0 );
    static void addAlert(AlertBox * a);
    static void setRenderMode(window_render_mode m ) { _rendermode = m; }
    static window_render_mode getRenderMode() { return _rendermode; }
    void addWindow(DisplayWindow* w) ; 
    void removeWindow( DisplayWindow *w);
    int  findWindow( DisplayWindow * w );


    void cleanAlerts();
    void bound( DisplayWindow * w );
    void arrangeWindows();

    void drawCursor();
    void setHandler( WindowHandler * wH ); 
    void selectWindow(t_CKUINT * stack, t_CKUINT  stackTop);
    void setTopWindow ( DisplayWindow * w );

    DisplayWindow * top() { return _topWindow; }

    InputState * wimp()       { return &_inp; }

    virtual void handleEvent ( const InputEvent &e );
    
    virtual void handleKey ( const InputEvent &e );
    virtual void handleSpec( const InputEvent &e );
    virtual void handleMouse( const InputEvent &e );
    virtual void handleMotion( const InputEvent &e );

    void setPick( Point2D pt ) { _pick_pt = pt; } 
    void setCur ( Point2D pt ) { 
        if ( !( pt == _cur_pt) ) 
        {
            _last_pt = _cur_pt; 
            _cur_pt = pt; 
        } 
    }
    UIRectangle * root( void ) { return &_rootWindow; } 
    
    WindowManager();
    virtual void draw();
};



class WindowHandler { //handles interaction types so that WindowManager can be a 
                        //more general manager classs
protected:
    WindowManager * _wm;

public:

    void setWM ( WindowManager* w) { _wm = w; }  
    virtual bool handleKey (const InputEvent &e);
	virtual bool handleSpec(const InputEvent &e);

};

//should extend a basic window?
//3d window with 2d content area...

//if we're feeling lucky, cache a copy of the window into
//texture when it's in the background, so that we save redraws..
//as long as we prohibit the same buffer in multiple windows, or have an _edited flag.

enum { WINDOW_OPEN, WINDOW_CLOSING, WINDOW_FINISHED };

class DisplayWindow { 

protected:

    Point2D             _velocity;
    WindowContent *		_content;
    GLuint				_texbuf;
    
    int					_id;  //stencil id ( contents id+1 )
	UIRect				_baseWindow;
	Quaternion			_orientation;   

	InputState			_inp;
	int					_closing;
	bool				_selected;
	bool				_iconified;
	//ui elements
	double              _scrollWidth;
	double              _titleHeight;
	double              _mesgHeight;
	double				_marginSize;

	UIRect              _contentWindow;

	UIButton			_titleBox;
	UIRect				_titleBar;

        //UI scrollbar class	
	UIScrollBar			_vScrollBar;
	UIScrollBar			_hScrollBar;

	UIRect				_sizeBox;

        // close or file i/o
	UIButton			_closeButton;

public:

    virtual void handleKey (const InputEvent &e);
    virtual void handleSpec(const InputEvent &e);
    
    virtual void handleMouse(const InputEvent &e);
    virtual void handleMouseUI(const InputEvent &e);

    virtual void handleMotion( const InputEvent &e);
    virtual void handleMotionUI();

    virtual void handleButton( int button );
    
    virtual void dragAction();

    void drift ( double time );
    void windowTransform();
    void contentTransform();

    void drawWindow();
    void drawWindowInactive();
    virtual void drawIconified();    
//subdraw functions

    void drawBase();

    virtual void stencilContent();
    virtual void drawStenciledContent();
    
    virtual void drawContent();
    
    virtual void drawUILayer();
    virtual void drawUILayerInactive();
    
    void drawScrollBars();
    virtual void drawStatus();
    
    virtual void clean();

    bool closing();
    void close();
    void exit();

    //interaction query
    bool draggable();
    bool resizeable();
    bool iconified();
    
    void moveto(double x, double y );
    void resize(double width, double height);
    void rescale (double scalex, double scaley );

    virtual void initShapes();

    virtual void reshape();
    void setContentArea();
    

    void setContent( WindowContent * c );
    void selectContent();
    WindowContent * getContent() { return _content; }
    UIRect * base() { return &_baseWindow; }
    void setVel( Point2D vel ) { _velocity = vel; }
    Point2D getVel()           { return _velocity; }
    
    int&  id() { return _id; } 
    void setID(int id);
    bool& selected() { return _selected; }
    DisplayWindow();
    ~DisplayWindow();
    


};

class WindowContent { 

protected:

	int         _id;  //id for matching
	UIRectangle _viewport;
	UIRectangle _canvas;

	InputState  _inp;
	bool	    _selected;
	bool	    _containerDirty;

public:

	bool& selected() { return _selected; }
	bool& containerDirty() { return _containerDirty; }
        virtual void refreshView() {}
	//current view position (display coords) for scrollbars 
	virtual void setX(double x) { _viewport.moveto(x, _viewport.y());} 
	virtual void setY(double y) { _viewport.moveto(_viewport.x(), y);} 

	virtual void setViewPort(UIRectangle &vp);
	virtual void setCanvas(UIRectangle &cv);

	//content determines its current height / width
	virtual double width()  { return _canvas.w(); }
	virtual double height() { return _canvas.h(); }

	virtual double getX() { return _viewport.x();}
	virtual double getY() { return _viewport.y();}

	virtual double getXSpan() { return _viewport.vpW(); }
	virtual double getYSpan() { return _viewport.vpH(); } 
        
        virtual double getXRange();
        virtual double getYRange();
        
        virtual UIRectangle & viewport() { return _viewport;}
	virtual UIRectangle & canvas()   { return _canvas;}


	virtual const char * title()   { return "title- virtual!"; }
	virtual const char * mesg()    { return "mesg - virtual!"; }
	virtual const char * posMesg() { return "posMesg - virtual!"; }

	virtual void handleKey  (const InputEvent &e)   { _inp.setEvent(e); }
	virtual void handleSpec (const InputEvent &e)   { _inp.setEvent(e); }
	virtual void handleMouse(const InputEvent &e)   { _inp.setEvent(e); }
	virtual void handleMotion(const InputEvent &e)  { _inp.setEvent(e); }

	virtual void draw()         { fprintf(stderr, "no base draw defined!\n"); }

	virtual int& id() { return _id; } 

	virtual int checkID(int id) { _selected = ( _id == id ) ? true : false ; return _selected; }

	virtual bool checkIDList(t_CKUINT * idlist, int n);

	virtual int onClose() { 
		fprintf(stderr, "window closing!");
		return 0;
	}

	WindowContent() :
	_id (0),
		_selected (0)
	{ }
};

#endif
