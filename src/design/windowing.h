#ifndef __WINDOWING_H_INCLUDED__
#define __WINDOWING_H_INCLUDED__

#include "sys.h"
#include "platform.h"
#include "geometry.h"


class DisplayWindow;
class WindowContent;
struct InputState;
struct InputEvent;
class UIMenu;

class IDManager { 

protected:
    static IDManager * _inst;
    bool *      _sids;
    int         _sidnum;
    uint         _pickID;
    IDManager();
public:
    int getStencilID();          
    void freeStencilID(int i);   
    uint getPickID();
    static IDManager * instance();
};

enum draggable_type {  drag_None =0, drag_Unknown, drag_DisplayWindow, drag_ShredInstance, drag_CodeRevision, \
                       drag_CodeWindow } ;
enum draggable_action { drag_is_Empty =0, drag_is_Picking, drag_is_Holding, drag_is_Dropping,} ; 

class DragManager {

protected:

    static DragManager * _instance; 
//    static GLuint               _pickbuffer[512]; //maybe?
//    static GLuint               _stack_size;
//    static GLuint*              _stack_top;
    draggable_type       _type;
    void*                _object;
    draggable_action     _mode;

public:

    DragManager(); 
    static DragManager * instance();
    draggable_type type()               { return _type;     }
    void * object()                     { return _object;   }
    void setobject( void * t,  draggable_type r) { _object = t; _type = r; _mode = ( t ) ? drag_is_Holding : drag_is_Empty; } 
    draggable_action mode()             { return _mode;     }
    void setmode( draggable_action d)   { _mode = d;        }   
};  


enum ui_state { UI_BASE, UI_HOVER, UI_SEL};


// UIRectangle extends the gRectangle class
// and provides a base class for windows, buttons, etc.
// knows how to draw itself(GL), has colors, is active, etc

typedef class UIRectangle UIRect;
class UIRectangle : public gRectangle 
{ 
protected: 

    //coordinates within this rectangle
    Point2D _scale;
    Point2D _origin;

    //UI selection
    bool    _hover;
    bool    _selected;

    //inner/outer colors
    Color4D _bg[3];
    Color4D _border[3];
    void    _defColors();

    //picking ID
    uint    _id;
    
public:
    uint  id() { return _id; } 

    virtual void draw() {}
    void  drawID();
    virtual void drawNamed();

    //generate gl vertex list for drawing
    void  quadVerts();
    void  roundVerts(float rad = 0);

    // draw calls
 
    //filled shape ( POLYGONS )
    void  filledQuad();
    void  bubbleQuad();
    void  filledRounded(float rad = 0);
    void  bubbleRounded(float rad = 0);

    //outline of shape  ( GL_LINE )
    void  outlineQuad();
    void  outlineRounded(float rad = 0);

    //drop shadow for shape
    void  dropShadowRounded(float density, float rad = 0 );

    // horizontal / vertical line through  center
    void  hLine();
    void  vLine();


    void  transform();
    void  scale();
    void  offset();
    void  translate();

    void  vpScale();
    void  vpTransform();
    void  vpTranslate();
    void  vpOffset();

    void  viewTranslate();
    
    double  vpW() { return w()/_scale[0];} 
    double  vpH() { return h()/_scale[1];} 
    
    double&  sx() { return _scale[0]; } 
    double&  sy() { return _scale[1]; } 
    double& ox() { return _origin[0]; } 
    double& oy() { return _origin[1]; } 
    
    bool hover() { return _hover;}
    bool selected() { return _selected;}
    void setHover( bool hov ) { _hover  = hov ; }

    virtual bool checkHover(Point2D pt) { _hover = inside(pt) ? true : false ; return _hover; }
    virtual void handleMouse ( const InputEvent & e);
    virtual void clearHover()	{ _hover	= false; }
        
    bool checkID();
    
    void clearImp() { _selected = false; }


    void setCol(Color4D& c);
    void setCols( ui_state which, Color4D bg, Color4D border); 
    void setCols( UIRect & r ) { 
        for ( int i = 0 ; i < 3; i++ ) { 
            _bg[i] = r._bg[i]; 
            _border[i] = r._border[i]; 
        } 
    }

    void print() { printf("--UIRect--\n"); gRectangle::print(); _scale.printnl(); _origin.printnl(); }
    
    void  setScale (double x, double y) { _scale  = Point2D(x,y); }
    void  selfScale () { _scale = Point2D(w(), h()); }
    void  setOrigin(double x, double y) { _origin = Point2D(x,y); }
    
    //Transform Points
    Point2D toLocal( Point2D pt );
    Point2D toGlobal( Point2D pt );
    
    //Transform Rects
    UIRectangle toLocal( UIRectangle );
    UIRectangle toGlobal( UIRectangle );

    //transform as viewport
    Point2D vpToLocal( Point2D pt );
    Point2D vpToGlobal( Point2D pt );

    Point2D getScale( ) { return _scale; }
    Point2D getOrigin( ) { return _origin; }

    UIRectangle() : 
	gRectangle(), 
        _scale   ( Point2D( 1,1 ) ), 
        _origin  ( Point2D( 0,0 ) ), 
        _selected(false), 
        _hover   (false) 
	{   
            _defColors();
            _id = IDManager::instance()->getPickID(); 
        }

    UIRectangle(double x,double y,double w,double h) : 
        gRectangle(x,y,w,h), 
        _scale   (Point2D(1,1)), 
        _origin  (Point2D( 0,0 )), 
        _selected(false), 
        _hover   (false)  
	{   
            _defColors(); 
            _id = IDManager::instance()->getPickID();
        }

};

//scrollbar class

class UIScrollBar : public UIRect { 
protected : 
	//range
	UIRect	_box;
	bool	_vertical;
	float	_vmin;
	float	_vpos;
	float	_vspan;
	float   _vrange;

public:

	UIScrollBar( bool vertical = false );
	UIRect& box() { return _box; }
	void    draw();
	void	setRange( double pos, double span, double  range);
	double	handleMotion( Point2D dpos );
    void    handleMouse( const InputEvent &e ) ;
	double	getPos();
	void	setPos(double d);

	void setBoxCols( ui_state which, Color4D bg, Color4D border) { _box.setCols(which, bg, border); }
	void setVertical( bool vert ) { _vertical = vert; }
	bool checkHover(Point2D pt);
	void clearHover()	{ _hover	= false; _box.clearHover(); }

};


class UIButton : public UIRect { 
protected :
	string _label;
	string _height;
	uint   _code;
	UIMenu * _menu; //attach a menu;
public:
	UIButton();
	UIButton(char *s);
	virtual void draw( float scale = 1.0);
	virtual void drawNamed( float scale = 1.0)  { glPushName( _id ); draw(scale); glPopName(); } 
    virtual void handleMouse(const InputEvent &e);
	void setLabel ( char *s ) { _label = s;}
	void setCode (uint id ) { _code = id; } 
    uint getCode () { _selected = false ; return _code; } 
	void setMenu ( UIMenu * menu);
	void fitLabel ();
	uint buttonID() { return _code ; }
	//fnptr _callback();
};


struct menuItem { 
	string name;
	uint   code; 
	UIMenu * subMenu;
};

class UIMenu : public UIRect { 
protected :
	std::vector < menuItem >	_options;
	int							_selected;
	double						_w;
	double						_h;
	double						_margin;
public:
	UIMenu();
	void draw();
	void addOption(char *c, uint code = 0);
	void addOption(string s, uint code = 0) { addOption( s.c_str(), code ); }
};

#define input_ALT       GLUT_ACTIVE_ALT
#define input_CTRL      GLUT_ACTIVE_CTRL
#define input_SHIFT     GLUT_ACTIVE_SHIFT

#define input_DOWN      GLUT_DOWN
#define input_UP        GLUT_UP

#define input_LEFT_BUTTON   GLUT_LEFT_BUTTON
#define input_RIGHT_BUTTON  GLUT_RIGHT_BUTTON

enum input_type { input_NONE=0, input_MOUSE, input_MOTION, input_KEY, input_SPEC, input_NTYPES };

struct InputEvent {

    input_type type;
    Point2D pos;
    int button;
    int state;
    int key;
    double time;
    uint mods;
    uint * stack;
    uint size;

    InputEvent() 
    { 
        clear();
    }

    InputEvent( input_type itype, Point2D ipt, int ibutton, int istate)
    { 
        clear();
        type = itype;
        pos = ipt;
        button = ibutton;
        state = istate; 
    }
    
    InputEvent( input_type itype, Point2D ipt)
    {
        clear();
        type = itype;
        pos = ipt;
    }

    InputEvent( input_type itype, Point2D ipt, int ikey )
    {

        clear();
        type = itype;
        pos = ipt;
        key = ikey;
    }

    void clear() {        
        type    = input_NONE;
        pos     = Point2D(0,0);
        button  = -1;
        state   = -1;
        key     = -1;
        time    = 0.0;
        mods = 0;
        stack   = NULL;
        size    = 0;
    }
    void setMods  ( uint imods )                { mods  = imods; } 
    void setStack ( uint * istack, uint isize ) { stack = istack; size = isize; }

    Point2D p() { return pos; } 
    
    void popStack( ) 
    { 
        size  = ( size > 0  ) ? size-1  : 0; 
        stack = ( size > 0  ) ? stack+1 : NULL ; 
    } 
    
    void fprint ( FILE * fd) const { 
        Point2D p = pos;
        switch ( type ) { 
            
        case input_MOUSE: 
            fprintf( fd, "Mouse Event : (%f,%f) %d %d\n", p[0], p[1], button, state );
            break;
        case input_MOTION: 
            fprintf( fd, "Motion Event : (%f,%f)\n", p[0], p[1] );
            break;
        case input_KEY: 
            fprintf( fd, "Key Event : key %c (%d) - mods %d - (%f,%f)\n", key, key, mods, p[0], p[1]);
            break;
        case input_SPEC: 
            fprintf( fd, "Spec Event : key (%d) - mods %d - (%f,%f)\n", key, mods, p[0], p[1]);
            break;
        }
    }
    uint curID ( void )     const   { return ( size ) ? *stack : 0 ; }
    bool checkID(uint id )  const   { return ( id && id == curID()); }
};


struct InputState {

    InputEvent event;
	bool isDepressed;
	bool leftDown;
	bool midDown;
	bool rightDown;

	Point2D dPos;
	Point2D lastPos;
	Point2D lastClick;

	char lastKey;
	int  lastSpec;
	bool ctrlDown;
	bool altDown;
	bool shiftDown;

    double dtime;
    double lasttime;

    InputState() : 
	isDepressed(false), 
		leftDown(false), 
		midDown(false), 
		rightDown(false), 
		altDown(false), 
		ctrlDown(false), 
		shiftDown(false), 
		lastPos(Point2D(0,0)),
		dPos(Point2D(0,0)),
		lastClick(Point2D(0,0)),
		lastKey('a'),
		lastSpec(0),
        dtime(0),
        lasttime(0)
	{}

    void setEvent ( const InputEvent &e , Point2D local_pt) { 
        event = e;
        event.pos = local_pt;
        setMouse  ( event );
        if ( e.type == input_KEY ) setKey(event);
        if ( e.type == input_SPEC ) setSpec(event);
        //setTime(event.time);
    }

    void setEvent ( const InputEvent &e) { 
        setEvent( e, e.pos );
    }

    void setMouse ( const InputEvent &e ) {

        Point2D p = e.pos;
        dPos = p - lastPos;

        switch ( e.button ) { 
        
        case input_LEFT_BUTTON:
            leftDown = ( e.state == input_DOWN ) ? true: false;
            break;
        
        case input_RIGHT_BUTTON:
            rightDown = ( e.state == input_DOWN ) ? true : false;
            break;

        }

        isDepressed = ( leftDown | rightDown );
        if ( e.button >= 0 ) keyState ( e );
        if ( e.state == input_DOWN ) lastClick = e.pos;
        lastPos = e.pos;

    }

	void setMouse ( Point2D pos, int button=-1, int state=-1 ) { 
		//         printf("%d %d", button, state ); pos.printnl();

		dPos = pos - lastPos;
		switch ( button ) {
		case GLUT_LEFT_BUTTON:
			leftDown =  ( state == GLUT_DOWN ) ? true : false;
			break;
		case GLUT_MIDDLE_BUTTON:
			midDown =   ( state == GLUT_DOWN ) ? true : false;
			break;
		case GLUT_RIGHT_BUTTON:
			rightDown = ( state == GLUT_DOWN ) ? true : false;
			break;
		}
		isDepressed = ( leftDown | midDown | rightDown );
		if ( button >= 0 ) keyState();
		if ( state == GLUT_DOWN ) lastClick = pos;
		lastPos = pos;
	}

    void setKey  ( const InputEvent &e ) { keyState(e); lastKey = e.key; }
	void setKey  ( char c ) { keyState(); lastKey = c;  }

    void setSpec ( const InputEvent &e ) { keyState(e); lastSpec = e.key; }
    void setSpec ( int c  ) { keyState(); lastSpec = c; }

    void setTime ( double t ) { dtime = t - lasttime; lasttime = t; }
	
    void keyState( const InputEvent &e ) { 
		altDown     =  !!( e.mods & input_ALT );
		shiftDown   =  !!( e.mods & input_SHIFT );
		ctrlDown    =  !!( e.mods & input_CTRL );
    }

    void keyState() { 
		altDown     =  !!( glutGetModifiers() & GLUT_ACTIVE_ALT    );
		shiftDown   =  !!( glutGetModifiers() & GLUT_ACTIVE_SHIFT  );
		ctrlDown    =  !!( glutGetModifiers() & GLUT_ACTIVE_CTRL   );
	}
};


class WindowHandler;

class WindowManager { 
protected:
    WindowHandler *             _handler;
    
    UIRect                      _rootWindow;
    std::vector < DisplayWindow * > _windows;
    DisplayWindow *             _topWindow;

    InputState                  _inp;
    int                         _wcount;
    bool                        _mexpose;

    uint                        _id;
public:

	void addWindow(DisplayWindow* w) ; 
    void removeWindow( DisplayWindow *w);
    int  findWindow( DisplayWindow * w );

    void bound( DisplayWindow * w );

	void drawCursor();
    void setHandler( WindowHandler * wH ); 
	void selectWindow(uint * stack, int stackTop);
    void setTopWindow ( DisplayWindow * w );

    DisplayWindow * top() { return _topWindow; }

    InputState * wimp()       { return &_inp; }

    virtual void handleEvent ( const InputEvent &e );
    
    virtual void handleKey ( const InputEvent &e );
    virtual void handleSpec( const InputEvent &e );
    virtual void handleMouse( const InputEvent &e );
    virtual void handleMotion( const InputEvent &e );
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

    void stencilContent();
    void drawStenciledContent();
    
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

	virtual bool checkIDList(uint * idlist, int n);

	virtual int onClose() { 
		fprintf(stderr, "window closing!");
		return 0;
	}

	WindowContent() :
	_id (0),
		_selected (0)
	{ }
};

/*
class ScrollableContent : public WindowContent { 
protected:

    WindowContent *             _content;

    UIRectangle                 _baseWindow; //reshaped by the displaywindow
    UIRectangle                 _contentWindow; 
    UIRectangle                 _viewport;

    //UI scrollbar class	
    UIScrollBar			_vScrollBar;
    UIScrollBar			_hScrollBar;

public:

};
*/
#endif
