#ifndef __AUDICLE_UI_EDITOR_H__
#define __AUDICLE_UI_EDITOR_H__

#include "audicle_ui_windowing.h"
#include "audicle_text_buffer.h"
#include "audicle_ui_parser.h"

#include "audicle_font.h"

//chuck includes
#include "util_network.h"
#include "chuck_otf.h"

// font used in text buffers;

extern AudicleFont* bufferFont;
void init_Buffer_Font();

class TextLine; 

// this is an interface for rendering the text once we add all sorts of 
// nubbins and widgies to it.
 
class LineRenderer { 
public: 
    AudicleFont * font;
    void draw ( TextLine & line );
    void charAtX( TextLine & line, double x );
};

class TextBuffer;

class TextContent: public WindowContent { 

protected:
    
    static std::vector < TextSpan > * _kring;
    static bool	_kring_inited;
    

    static ck_socket _tcp;
    static char      _hostname[512];
    static int       _port;
    static int       _connected;

    TextBuffer   *_buf;
    

    //should be in CodeRevision
    a_Program    _parsed;
    tree_a_File  _parse_tree; 
    int		     _parse_edit_point;





    //editing state
    TextLoc     _loc;
    
    int         _magicCharPos;
    
    TextLoc     _markLoc;
    TextSpan	_markSpan;
    
    int         _windowLinePos;   //first line in window
    int         _windowLineSpan;   //num of lines for current viewing height
    string      _posMesg;   //position info
    
    std::vector < TextSpan > * _killring();
    string      _title;     //title
    
    
    //has a canvas and draw methods
    //canvas display 
    double      _leading;
    double      _fontScale;
    double      _fontAspect;
    double	_fontWeight;
    
    double      _wSpace;
    double      _wTab;
    
    bool        _viewportDirty;
    void			_fixView();
    bool        _canvasDirty;
    void        _fixCanvas();
   
public:

   TextContent();

	// display
   void draw();
   void parse();

   void open(char *filename);
   void write(char *filename = NULL);

   virtual void setViewPort(UIRect & r) { 
      WindowContent::setViewPort(r); 
      _windowLineSpan = (int)ceil( _viewport.vpH() / _leading ); 
   }
   
   void setX(double x);
   void setY(double y);

   void setBuf ( TextBuffer * buf );
   TextBuffer * getBuf () { return _buf; } 

   void refreshView();

   void moveToLine(int nLine);
   void moveToChar(int nChar);
   TextLoc loc() { return _loc ; }
   void setLoc(TextLoc n, bool mark);
   TextLoc mouseToLoc(Point2D pt); //window coords to line,char
   bool checkIDList( t_CKUINT * stack, int n );

   const char * mesg() { if ( _buf ) return _buf->mesg().c_str(); else return title(); }
   const char * title() { return _title.c_str();}
   const char * posMesg(); 

   void handleKey   ( const InputEvent &e) ;
   void handleSpec  ( const InputEvent &e) ;
   void handleMouse ( const InputEvent &e);
   void handleMotion( const InputEvent &e);
   void handleButton( int buttonID );

   void paste ( string s, TextSpan &span); 
   void cut   ( TextSpan &span );
   void copy  ( TextSpan &span );

   void parse_adjust_edits(TextEdit e);

   void connect_tcp ( char * hostname = NULL , int port = 8888 );
   void close_tcp(); 
   void sendtcp();
};


//our own subclass of windowman, that handles keys our way n' thangs.

class TextWindowHandler: public WindowHandler { 
protected:
public:
	virtual bool handleKey (const InputEvent &e);
	virtual bool handleSpec(const InputEvent &e);
};


#endif
