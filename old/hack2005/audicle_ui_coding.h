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
// name: audicle_ui_coding.h
// desc: ...
//
// authors: Philip Davidson (philipd@cs.princeton.edu)
//          Ge Wang (gewang@cs.princeton.edu)
//          Perry R. Cook (prc@cs.princeton.edu)
//          Ananya Misra (amisra@cs.princeton.edu)
// date: Autumn 2004
//-----------------------------------------------------------------------------
#ifndef __AUDICLE_UI_CODING_H__
#define __AUDICLE_UI_CODING_H__

#include "audicle_ui_editor.h"
#include "audicle_ui_parser.h"

#include "chuck_stats.h"


class CodeRevision;

//needs co-audicle imp
//window 
//   - revisions
//          - process/shreds
class CodeWindow : public DisplayWindow { 

protected:
    //already has content from dwindow;
    CodeRevision*                   _current;
    std::vector < CodeRevision* >   _revisions;
    TextContent *                   _textcontent;

    //coding    
    UIButton    _testButton;
    UIButton    _compileButton;
    UIButton    _sporkButton;
    UIRect      _revisionBox;



public:

    CodeWindow();
    CodeWindow(CodeRevision * r);

    CodeRevision * current() { return _current; }


    //file management
    
    void makeRevision();
    virtual int  findRevision(CodeRevision *r);
    virtual void setRevision(int i);
    virtual void setRevision(CodeRevision* r);
    virtual void addRevision( CodeRevision *r, bool makeCurrent = false );
    virtual void removeRevision( CodeRevision * r );

    virtual void checkErrorLocations();

    virtual void drawContent();
    virtual void drawUILayer();
    virtual void drawUILayerInactive();
    virtual void drawRevisionList();

    //UI Layer
    void setTextContent( TextContent * t )  { _textcontent = t; setContent(t); }
    TextContent * getTextContent()          { return _textcontent; } 

    virtual void dragAction();
    
    virtual void initShapes();
    virtual void reshape();
    //ui callbacks

    void handleKey      (const InputEvent &e);
    void handleSpec     (const InputEvent &e);
    void handleMouse    (const InputEvent &e);
    void handleMotion   (const InputEvent &e);
    virtual void handleMotionUI();
    virtual void handleMouseUI( const InputEvent &e );
    bool checkIDList(t_CKUINT* idlist, t_CKUINT n );


    void parse();
    void compile();

};

enum shredState { shred_NEW, shred_RUNNING, shred_NEEDREMOVE, shred_KILLED, shred_NSTATE };


struct ShredInstance { 

    static t_CKBOOL _buttonsready;
    static void _setupButtons();
    static UIButton * _killButton;

    shredState status;

    CodeRevision * code; //revision from whence it came
    Shred_Stat * stat; //stat for shred
    int procID;
    int parent;
    int source;

    int pickID;
    bool selected;

    ShredInstance() { 
        procID = 0;
        parent = 0; //procID of immediate shred parent
        source = 0; //procID shred...
        status = shred_NEW;
        code = NULL;
        stat = NULL;
        selected = false;
        _setupButtons();
        pickID = IDManager::instance()->getPickID();
    }

    void handleMouse ( const InputEvent & e);
    void dragAction( DragManager * d );
    void draw();
};


enum revState { rev_NEW = 0, rev_OPENED, rev_EDITED, rev_PARSED, \
                rev_TYPED, rev_COMPILED, rev_READY, rev_RUNNING, \
                rev_NEED_SPLIT, rev_NSTATES };

class CodeRevision { 

protected:

    void *           _chuckEnv;
    static ck_socket _tcp;
    static char      _hostname[512];
    static int       _port;
    static int       _connected;
    static t_CKBOOL  _buttonsready;
    static void      _setupButtons();
    static UIButton  *_killButton;
    static UIButton  *_sporkButton;


    TextBuffer *    _buffer; //may experience changes
    TextBuffer *    _original;  //initial version of file that 
                                //revision syncs to..
                                //if compilation successful, 
                                //it's the running version of the code.
    FILE *          _tmp_fd;

    a_Program       _parsed_program;
    tree_a_File     _parse_tree;
    int             _parse_edit_point;
    void *          _bytecode;

    std::queue < TextLoc > _errloc;

public:
    revState        _status; //by the state of its shreds
    std::vector < ShredInstance* > _shreds;
    t_CKUINT            _id;
    bool            _selected;
    t_CKUINT        _branch_count;

public:

    CodeRevision();
    CodeRevision( TextBuffer * b);
    CodeRevision( CodeRevision * c );

    CodeRevision * splitModified();
    CodeRevision * dup();

    std::string  name_branch();

    void parseErrorLocation( std::string errmsg );
    std::queue < TextLoc >& error_loc( void ) { return _errloc; } 

    TextBuffer * getBuffer(void) { return _buffer; }
    void setBuffer(TextBuffer * b) { _buffer = b; _branch_count = 0; _status = rev_NEW; }

    void            checkModified() { if ( _buffer->modified() ) _status = rev_EDITED; }
    revState        status() { return _status; }

    void handleMouse(const InputEvent &e);
    void dragAction( DragManager * d );
    t_CKUINT id()               { return _id; }
    bool selected()         { return _selected; }
    void open( char * c );
    
    void            setVersion();
    int             parse();        //
    int             typecheck();    //needs parse
    int             compile();        //needs compile
    int             spork( int replace = 0 );
    int             size()  { return _shreds.size() + 1 ;}
    int             nshreds() { return _shreds.size(); }
    ShredInstance*  shred(int i) { return _shreds[i]; }

    void            addShred ( ShredInstance * s );
    int             findShred( int id );
    void            removeShred ( int id );
    void            removeShred ( ShredInstance * s);
    int             sporkTCP();
    void            connect_tcp ( char * hostname = NULL , int port = 8888 );
    void            close_tcp();
    int             sendtcp( int replace = 0 );
    int             removetcp ( int id );

    t_CKBOOL        local_spork( int replace = 0 );
    t_CKBOOL        local_remove( int id );

    void            draw();

};

class CodeWindowHandler : public WindowHandler { 
public:
    virtual bool handleKey  ( const InputEvent &e );
    virtual bool handleSpec ( const InputEvent &e );
    virtual bool winFromRev ( CodeRevision * r );
    virtual DisplayWindow * new_window();
};


#endif
