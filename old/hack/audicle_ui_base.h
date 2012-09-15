#include "audicle_def.h"
#include "audicle_geometry.h"
#include "audicle_event.h"
#include <vector>

enum ui_state { UI_BASE, UI_HOVER, UI_SEL};

// UIRectangle extends the gRectangle class
// and provides a base class for windows, buttons, etc.
// knows how to draw itself(GL), has colors, is active, etc

void   init_UI_Fonts(); 
extern class AudicleFont * labelFont;
extern class AudicleFont * labelFontMono;


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
    t_CKUINT    _id;
    
public:
    t_CKUINT  id() { return _id; } 

    void center_at(Point2D pos); //..center Rect at this pos

    Color4D col ( ui_state id = UI_BASE ) { return _bg[id]; } 
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
    void setSelected ( bool sel ) { _selected = sel; }

    virtual bool checkHover(Point2D pt) { _hover = inside(pt) ? true : false ; return _hover; }
    virtual void handleMouse ( const InputEvent & e);
    virtual void clearHover()	{ _hover	= false; }
        
    bool checkID();
    
    void clearImp() { _selected = false; }


    
    void setCol(Color4D& c);
    void setCols( ui_state which, Color4D bg, Color4D border); 
    void fillCol( Color4D col ) { _bg[0] = _bg[1] = _bg[2] = col; }
    void lineCol( Color4D col ) { _border[0] = _border[1] = _border[2] = col; }
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

    UIRectangle();
    UIRectangle(double x,double y,double w,double h) ;

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

class UIMenu;


class UIButton : public UIRect { 
protected :
	std::string _label;
	std::string _height;
	t_CKUINT   _code;
	UIMenu * _menu; //attach a menu;
public:
	UIButton();
	UIButton(char *s);
	virtual void draw( float scale = 1.0);
        virtual void drawLabel( float scale = 1.0, int mono = 0 );
        virtual void drawLeadedLabel( float scale , float lift, int mono = 0 );
        virtual void drawQuad ( float scale = 1.0 );
        virtual void drawRounded ( float scale = 1.0 );
        virtual void drawBubbled ( float scale = 1.0 );
	virtual void drawNamed( float scale = 1.0);

        virtual void handleMouse(const InputEvent &e);

	void setLabel ( char *s ) { _label = s;}
    void setLabel ( std::string s ) { _label = s; } 
	void setCode (t_CKUINT id ) { _code = id; } 
    t_CKUINT getCode (); 
	void setMenu ( UIMenu * menu);
	void fitLabel ( t_CKBOOL mono = false );
    void fitXLabel ( double tscale, t_CKBOOL mono = false );
	t_CKUINT buttonID() { return _code ; }
	//fnptr _callback();
};


struct menuItem { 
	std::string name;
    t_CKUINT    id;
	t_CKUINT    code; 
    UIButton    button;
    menuItem();
    menuItem(char* c, t_CKUINT code );
    void reshape();
};

class UIMenu : public UIRect { 
protected :
	std::vector < menuItem >	_options;
	double						_margin;
    UIButton                    _button;
    t_CKUINT                    _default_code;
public:
	UIMenu();
	void draw();
    void reshape();
    void addOption(char *c, t_CKUINT code = 0);
    virtual void handleMouse( const InputEvent &e);
    t_CKUINT getCode( );
	void addOption(std::string s, t_CKUINT code = 0) { addOption( s.c_str(), code ); }
    UIButton& button(int i ) { return _options[i].button; }
};
