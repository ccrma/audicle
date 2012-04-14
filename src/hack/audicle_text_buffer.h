//-----------------------------------------------------------------------------
// name: audicle_text_buffer.h
// desc: interface for audicle face vmspace
//
// authors: Philip Davidson (philipd@cs.princeton.edu)
//          Ge Wang (gewang@cs.princeton.edu)
//          Perry R. Cook (prc@cs.princeton.edu)
//          Ananya Misra (amisra@cs.princeton.edu)
// date: Autumn 2004
//-----------------------------------------------------------------------------

#ifndef __AUDICLE_TEXT_BUFFER_H__
#define __AUDICLE_TEXT_BUFFER_H__

#include "audicle_def.h"
#include "audicle_utils.h"

class TextLine { 

public:

   TextLine() { 
      _s = "";
      _indent = 0;
      _dirty = true;
      _indentDirty = true;
   }
   
   TextLine( char * p ) { 
      _s = p;
      _indent = 0;
      _dirty = true;
      _indentDirty = true;
   }

   TextLine( std::string s ) { 
      _s = s;
      _indent = 0;
      _dirty = true;
      _indentDirty = true;
   }

   int  addTab ( int pos = -1 );
   int  addChar(char p, int pos = -1);
   void insertString(std::string p, int pos = -1);
   void remChar(int pos = -1);
   void printnl(FILE * fp = stderr) { fprintf( fp, "%s\n",  _s.c_str() ); }
   void print(FILE * fp = stderr )  { fprintf( fp, "%s",    _s.c_str() ); }
   int  getRange(char ** ret, int pos, int range);
   int  size()  { return _s.size(); }

   void draw(); //GL! bad!
   double len( int pos = -1) ;  // GL related - get out of here!
   int  charbypos (double x) {} // also GL?

   std::string substr(int pos, int len) { return _s.substr(pos,len); }
   const char * cstr() { return _s.c_str(); }
   std::string& str()   { return _s;} 

protected:
   std::string   _s;
   int      _len;
   int      _indent;
   bool     _dirty;
   bool     _indentDirty;

};

struct TextLoc { 

   int line;
   int chr;
   bool operator < (TextLoc & t);
   bool operator == (TextLoc & t);
   TextLoc() { line = 0; chr = 0; }
   TextLoc(int l, int c )  { line = l; chr = c;}
   void print() { printf("(%d %d)\n", line,chr); } 
   TextLoc shift ( int l, int c ) { return TextLoc( line + l , chr + c ); }

};

struct TextSpan { 

   TextLoc begin;
   TextLoc end;
   std::string content;
   TextSpan(TextLoc b, TextLoc e, std::string s) 
   { begin = b; end = e; content = s;}
   TextSpan(TextLoc b, TextLoc e );
   TextSpan() {}
   void print() { 
      printf("b"); begin.print();
      printf("e"); end.print();
   }
   int lines() { return end.line - begin.line; }
   void close() { begin = end; }
   bool empty() { return ( begin == end ); }
   void add(TextSpan & t ); 

};

enum { EDIT_NEW, EDIT_OPEN, EDIT_DONE };
struct TextEdit { 
   int state;
   TextSpan from;
   TextSpan to;
   void undo();
   void redo();
   void print(); 
   TextEdit() { state = EDIT_NEW; } 
};


//Text buffer object 
//to represent the current Document.
//has no knowledge of drawing, only
//text ops.

class TextBuffer  { 

   
protected:

    //the goods
    fmBuffer *				 _source;
    std::string					 _filename;   
    std::string					 _filepath;
    std::string					 _mesg;      //helpful message!

   TextEdit					 _curEdit;
   int						 _editDepth;

   std::vector< TextLine >  _lines; 
   std::vector< TextEdit >  _edits;

   int						_undoPos;
   bool						_inUndo;

   int                     _modDepth;  //edit depth

public:

   TextBuffer();
   
   //part of textbuffer
   void open(char * filepath = NULL);
   void write(char * filepath = NULL);

   void startEdit(TextLoc fromb, TextLoc frome);
   void endEdit(TextLoc tob, TextLoc toe);
   void mergeEdits();
   void closeEdit();
   bool modified() { return ( _modDepth != _edits.size() ) ; } 
   void setVersion() { closeEdit(); _modDepth = _edits.size(); }
   int nEdits()  { return _edits.size(); }
   TextEdit& edit(int i)  { return _edits[i]; }
   
   TextLoc begin();
   TextLoc end();
   TextSpan all();
   
   // messages
   std::string& mesg() { return _mesg; } 
   // text output
   void print(FILE * f = stderr) { 
       for ( t_CKUINT i = 0; i < _lines.size(); i ++ ) {
           if ( i != _lines.size() - 1 ) _lines[i].printnl(f);
           else _lines[i].print(f);
       }
   }
   
   void printEdits() { 
       for ( t_CKUINT i = 0 ; i < _edits.size(); i++ ) 
           _edits[i].print();
       fprintf(stderr, "edits : %d , undoPos : %d  inUndo %d\n", _edits.size(), _undoPos, _inUndo ); 
   }
   
   bool         empty () { return ( nlines() == 1 && line(0).size() == 0 ) ; }
   TextLine& line(int n) { return _lines[n]; } 
   int		 nlines()		{ return _lines.size(); }
   
   std::string& filename()		{ return _filename; }
   void setFileName(std::string name );
   std::string& filepath()      { return _filepath; } 
   void setFilePath(std::string path );
   
   // editing operations
   // maybe these all return textLoc?  textSpan? 
   void insertChar( char c, TextLoc &loc ); //w TextLoc
   void removeChar( TextLoc &loc );
   void insertLine( TextLoc &loc );
   void removeLineBreak( TextLoc &loc );
   void addLine( std::string s = "", int pos = -1);
   void addTextLine( TextLine &t, int pos = -1 );
   void checkSpan(TextSpan &span);
   void insertSpan( std::string s, TextSpan &span );
   void fillSpan  ( TextSpan &span ); //fills the span with text from buffer. 
   void removeSpan( TextSpan &span ); //call fillSpan, then removes range, 
   
   void undo( TextLoc& loc );
   void redo( TextLoc& loc );
   
};

#endif
