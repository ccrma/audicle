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
// name: audicle_text_buffer.cpp
// desc: interface for audicle face vmspace
//
// authors: Philip Davidson (philipd@cs.princeton.edu)
//          Ge Wang (gewang@cs.princeton.edu)
//          Perry R. Cook (prc@cs.princeton.edu)
//          Ananya Misra (amisra@cs.princeton.edu)
// date: Autumn 2004
//-----------------------------------------------------------------------------

#include "audicle_text_buffer.h"


int
TextLine::addTab ( int pos ) { 
    int tostop = 4 - pos%4;
    int i=0;
    while ( i < tostop ) { i++; addChar ( ' ', pos ); }
	_dirty = true;
    return ( pos >= 0 && pos <= (int)_s.size() ) ? pos+tostop : _s.size() ;
}

int
TextLine::addChar( char p, int pos ) { 
    static char gimme[] = " ";
    if ( p == '\t' ) return addTab( pos ) ;
    gimme[0] = p;
    if ( _s.size() == 0 || pos < 0 || pos > (int)_s.size() ) _s += gimme ;
    else _s.insert( pos, gimme ); 
	_dirty = true;
    return pos+1;
}

void
TextLine::remChar( int pos ) { 
    if ( pos < 1 ) _s.erase( _s.size()-1, 1);
    else _s.erase(pos-1,1);
	_dirty = true;
}

bool 
TextLoc::operator < ( TextLoc &t) { 
    if ( line < t.line ) return true;
    if ( line == t.line && chr < t.chr ) return true;
    return false;
}
bool 
TextLoc::operator == ( TextLoc &t) { 
    return ( line == t.line && chr == t.chr ) ? true : false;
}

void
TextSpan::add( TextSpan & t ) {
    if ( end == t.begin ) { 
        content += t.content;
        end = t.end;
    }
    else if ( t.end == begin ) { 
        begin = t.begin;
        content = t.content + content;
    }
}

void 
TextEdit::print() { 
    fprintf(stderr, "edit (%d,%d) -(%s)- (%d,%d) => (%d,%d) +(%s)+ (%d,%d);\n",     \
                     from.begin.line, from.begin.chr, from.content.c_str(), from.end.line, from.end.chr,  \
                     to.begin.line, to.begin.chr, to.content.c_str(), to.end.line, to.end.chr );
}

TextSpan::TextSpan (TextLoc b, TextLoc e ) 
{ 
    if ( b < e ) 
    { 
        begin = b; 
        end = e; 
    }
    else { 
        begin = e; 
        end = b;
    }
}


TextBuffer::TextBuffer() { 

    _lines.clear();
    _filename = "*buf*";
    _filepath = "";
    _source  = new fmBuffer(FMODE_FREAD);
    _undoPos = -1;
    _inUndo = false;
    _edits.clear();
    _editDepth = 0;
    _modDepth = 0;

    addLine("");
}

void
TextBuffer::write( const char * filepath ) { 
    fprintf(stderr, "writing to file\n");
    char outfile[256];
    outfile[0] = '\0';
    if ( filepath ) strncpy ( (char*)outfile, filepath, 256 );
    else if ( _filepath.size() > 0 ) strncpy ( (char*)outfile, _filepath.c_str(), 256 );

    if ( strlen(outfile) == 0 ) { 
        char newname[512];
        newname[0]= '\0';

        DirScanner * dScan = new DirScanner();
        fileData * fD = dScan->saveFileDialog();
        if ( fD ) {
            fprintf(stderr, "textbuf - opening %s\n", fD->fileName.c_str() );
            write ( (char*) fD->fileName.c_str() );
        }
        else {  
            fprintf (stderr, "no filename specified\n");
        }
        return;
          
//      _mesg = "waiting for file name..."; 
//        fprintf(stderr, "enter a filename!...\n>");
//        fscanf(stdin, "%s", newname );

    }
    FILE * f = fopen ( outfile ,"w" );
    if ( f ) { 
        print(f);
        fclose(f);
        _mesg = (std::string)"wrote to file " + outfile;
        _filepath = outfile;
        char * name = strrchr ( outfile, DM ); 
        _filename = ( name ) ? name+1 : _filepath ; 
        setVersion();
    }
    else { 
        _mesg = (std::string)"! error writing to file " + outfile;
        fprintf( stderr, "texteditor] TextBuffer::write() error- could not open %s\n", outfile );
    }
}

void
TextBuffer::open( const char * filepath ) { 
    if ( !filepath ) { 
        
        DirScanner dScan;
        fileData * fD = dScan.openFileDialog();
        if ( fD ) {
            fprintf(stderr, "textbuf - opening %s\n", fD->fileName.c_str() );
            open ( (char*) fD->fileName.c_str() );
            if ( fD->next ) 
                fprintf(stderr, "textbuffer: opening first of multiple files...\n");
        }
        else {  
            fprintf (stderr, "no filename specified\n");
        }
        return;
    }
    else if ( !_source->readfile(filepath) ) { 
        _lines.clear();        
        std::string ln;
        while ( _source->getline(ln) != EOF ) { 
            addLine(ln);
        }
        setFilePath ( filepath );
        _mesg = (std::string)"read file: " + _filename ;

        _edits.clear();
        _undoPos = -1;
        _inUndo = false;
        _editDepth  =0;
        _modDepth = 0;
        setVersion();
    }
    
    else { 
        _filepath = "";
        _filename = "";
        _mesg = (std::string)"file: " + filepath + " not found";
    }
}

void
TextBuffer::setFileName(std::string name) { 
    _filename = name;
    int fpos = _filepath.rfind ( DM );   
    _filepath = ( fpos != std::string::npos ) ? _filepath.substr( fpos+1 ) + _filename : _filename;  
}

void
TextBuffer::setFilePath(std::string path) { 
    _filepath = path;
    int fpos = _filepath.rfind( DM );
    _filename =  ( fpos != std::string::npos ) ? _filepath.substr( fpos+1 ) : _filepath ;  

}

void
TextBuffer::addLine( std::string s, int pos ) { 
    if ( pos > (int)_lines.size() || pos == -1 ) {   _lines.push_back(TextLine(s)); }
    else { _lines.insert(_lines.begin()+pos, TextLine(s) ); }
}



void
TextBuffer::addTextLine( TextLine &t, int pos ) { 
    if ( empty() ) 
        _lines[0] = t; 
    else if ( pos > (int)_lines.size() || pos == -1 )
        _lines.push_back(t);
    else 
        _lines.insert(_lines.begin()+pos, t ); 
	t.dirty() = true;
}




void
TextBuffer::startEdit( TextLoc fromB , TextLoc fromE ) { 
    if ( _curEdit.state == EDIT_NEW )  
    {
        _curEdit.from = TextSpan(fromB, fromE);
        fillSpan(_curEdit.from);
        _curEdit.state = EDIT_OPEN;
        _editDepth = 0;
    }
    else if ( _curEdit.state == EDIT_OPEN ) { 
        //as long as successive edits don't modify the original range ( remove, then add...) 
        //this is okay...but we need to check for those cases!
        _editDepth++;
    }
    else { 
        fprintf(stderr,"error, edit not finished\n");  return; 
    }
}

//can be from outside the buffer's state ( network! ) 
void
TextBuffer::makeEdit ( TextEdit & e )
{ 
    EM_log( CK_LOG_FINER, "(audicle) TextBuffer::makeEdit..." );
    insertSpan( e.to.content, e.from );
}

//might broadcast outside the buffer's state ( network! )
void
TextBuffer::pushEdit ( TextEdit & e ) { 
    EM_log( CK_LOG_FINER, "(audicle) TextBuffer::pushEdit..." );
    _edits.push_back( _curEdit );
}

void
TextBuffer::endEdit(TextLoc toB, TextLoc toE) { 
    if ( _curEdit.state == EDIT_OPEN ) { 
        _curEdit.to = TextSpan(toB, toE); //always save last span, in case of closeEdit?
        if ( _editDepth > 0 ) --_editDepth;
        else { 
            fillSpan(_curEdit.to);
            
            pushEdit(_curEdit);
            //_edits.push_back(_curEdit);

            if ( !_inUndo ) mergeEdits(); //possible disaster here. warning! 

            _curEdit.state = EDIT_NEW;
            if ( !_inUndo ) _undoPos = _edits.size() - 1;
        }
    }
    else { fprintf(stderr,"error, edit not opened\n");  return; }
}

void
TextBuffer::closeEdit() { 

    if ( _editDepth ) { 
        _editDepth = 0;
        fillSpan(_curEdit.to);
        pushEdit(_curEdit);
        //_edits.push_back(_curEdit);
        if ( !_inUndo ) _undoPos = _edits.size() - 1;
    }
    if (_edits.size() > 0 ) { 
        _edits.back().state = EDIT_DONE; 
    }
    _curEdit.state = EDIT_NEW;
}

//XXX we need to control how merging occurs during undo.

void
TextBuffer::mergeEdits() { 
    if ( _edits.size() < 2 ) return;
    if ( _edits[_edits.size()-2].state != EDIT_OPEN ) return; 
    // try to merge these into a single edit...??
    TextEdit &fir = _edits[_edits.size()-2];
    TextEdit &sec = _edits.back();
    if ( sec.from.empty() && fir.to.end == sec.to.begin ) {     
        fir.to.add(sec.to); //append addition
        _edits.pop_back();
    }
    else if ( sec.to.empty() && fir.to.empty() && fir.to.end == sec.from.end ) { 
        fir.from.add(sec.from); //prepend subtraction
        fir.to = sec.to;
        _edits.pop_back();
    }
    // gonna be vooodooo
}



void 
TextBuffer::undo(TextLoc & loc) {
    if ( _edits.size() > 0 && _undoPos >= 0 ) { 
        TextEdit e = _edits[_undoPos];
        _inUndo = true;
        insertSpan( e.from.content, e.to );
        _inUndo = false;
        loc = e.from.end;
        _undoPos--;
    }

}

void
TextBuffer::redo(TextLoc & loc) { 
    if ( _edits.size() > 0 && _undoPos < (int) _edits.size() ) { 
        ++_undoPos;
        TextEdit e = _edits[_undoPos];
        _inUndo = true;
        insertSpan( e.to.content, e.from );
        _inUndo = false;
        loc = e.to.end;
    }
}
void
TextBuffer::insertChar( char c, TextLoc& loc ) { 
    
    startEdit( loc,loc);
    TextLoc p = loc;
    loc.chr = _lines[loc.line].addChar(c, loc.chr);   
    endEdit( p, loc );
}

void
TextBuffer::removeChar(TextLoc &loc ) { 
    if ( loc.chr == 0 && loc.line == 0 ) { 
        return;
    }
    else if ( loc.chr == 0 && loc.line > 0 ) {
        removeLineBreak(loc);
    }
    else { 
        startEdit( loc.shift(0,-1), loc);
        _lines[loc.line].remChar(loc.chr--);
        endEdit(loc,loc);
    }
}

void
TextBuffer::removeLineBreak( TextLoc &loc ) { 

    loc.line--;
    loc.chr = _lines[loc.line].size();
    
    startEdit ( loc,   TextLoc( loc.line+1 , 0 ) );
    _lines[loc.line].str() += _lines[loc.line+1].str();
    _lines.erase(_lines.begin() + loc.line + 1 );
    endEdit(loc, loc);  
	_lines[loc.line].dirty() = true;
}

void
TextBuffer::insertLine( TextLoc &loc ) { 

    TextLoc p = loc;
//  fprintf(stderr, "inserting line break\n");

    startEdit( loc,loc );

    addLine("", loc.line + 1);
    if ( loc.chr < _lines[loc.line].size() ) { 
        std::string l1 = _lines[loc.line].str();
        std::string l2 = _lines[loc.line+1].str();
        _lines[loc.line+1].str() = l1.substr( loc.chr, l1.size() - loc.chr );
        _lines[loc.line].str() = l1.substr( 0, loc.chr );
		_lines[loc.line+1].dirty() = true;
		_lines[loc.line].dirty() = true;
    }
    loc.chr = 0;
    loc.line++;

    endEdit(p, loc);
    //    _viewportDirty = true;
//    _canvasDirty = true;
}

TextLoc
TextBuffer::begin() { 
    return TextLoc ( 0, 0);
}

TextLoc
TextBuffer::end() { 
    if ( _lines.size() == 0 ) return begin();
    return TextLoc ( _lines.size()-1, _lines[_lines.size()-1].size() );
}

TextSpan
TextBuffer::all() { return TextSpan (begin(), end()); } 

void
TextBuffer::checkSpan( TextSpan &span ) { 
    if ( span.end < span.begin ) { 
        TextLoc t = span.end;
        span.end = span.begin;
        span.begin = t;
    }
    span.begin.line = max ( span.begin.line, 0 );
    span.begin.chr  = max ( 0, min ( span.begin.chr, _lines[span.begin.line].size() ) );
    span.end.line   = max ( 0, min ( span.end.line, (int)_lines.size()-1 ) );
    span.end.chr    = max ( 0, min ( span.end.chr,  _lines[span.end.line].size() ) );
}

void
TextBuffer::insertSpan( std::string s, TextSpan &span ) { 

    startEdit( span.begin, span.end );

    if ( !span.empty() ) removeSpan(span);

    for ( t_CKUINT i=0; i < s.size(); i++) { 
        if ( s[i] == '\n' ) insertLine( span.end );
        else insertChar( s[i], span.end );
    }

    endEdit ( span.begin, span.end );
}


void
TextBuffer::fillSpan( TextSpan &span ) { 
    checkSpan(span);
    span.content = "";
    if ( span.begin.line == span.end.line ) { 
        span.content = _lines[span.begin.line].substr ( span.begin.chr, span.end.chr - span.begin.chr );
    }
    else { 
        span.content += _lines[span.begin.line].substr( span.begin.chr, _lines[span.begin.line].size() ) + "\n";
        for ( int i = span.begin.line + 1; i < span.end.line ; i++ )
            span.content += _lines[i].str() + "\n";
        span.content += _lines[span.end.line].substr ( 0, span.end.chr );
    }
}

void
TextBuffer::removeSpan( TextSpan &span ) {
    checkSpan(span); //also checks
    
    //start new edit
    startEdit( span.begin, span.end );
    if ( span.begin.line == span.end.line ) { 
        std::string orig = _lines[span.begin.line].str();
        _lines[span.begin.line].str() = orig.substr(0, span.begin.chr) + orig.substr( span.end.chr, orig.size() - span.end.chr );
		_lines[span.begin.line].dirty() = true;
    }
    else { 
        //concatenate first to last;
        _lines[span.begin.line].str() =   _lines[span.begin.line].substr(0, span.begin.chr) \
                                        + _lines[span.end.line].substr( span.end.chr, _lines[span.end.line].size() - span.end.chr );
        _lines[span.begin.line].dirty() = true;
        for ( int i = span.end.line; i > span.begin.line; i-- ) { 
            //if pointers - delete _lines[i];
            _lines.erase(_lines.begin()+i);     
		}
    }    
    span.end = span.begin;
    endEdit( span.begin, span.end );
}
