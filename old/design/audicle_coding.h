
#include "parse_gui.h"
#include "texteditor.h"

class CodeRevision;

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


    virtual int  findRevision(CodeRevision *r);
    virtual void setRevision(int i);
    virtual void setRevision(CodeRevision* r);
    virtual void addRevision( CodeRevision *r, bool makeCurrent = false );
    virtual void removeRevision( CodeRevision * r );
    virtual void drawContent();
    virtual void drawUILayer();
	virtual void drawUILayerInactive();
    virtual void drawRevisionList();
    void setTextContent( TextContent * t ) { _textcontent = t; setContent(t); }
    TextContent * getTextContent() { return _textcontent; } 
    void makeRevision();

    virtual void handleMotionUI();
    virtual void handleMouseUI( const InputEvent &e );
    bool checkIDList(uint* idlist, uint n );

    virtual void dragAction();
    
    void initShapes();
    void reshape();
    //ui callbacks

    void handleKey      (const InputEvent &e);
    void handleSpec     (const InputEvent &e);
    void handleMouse    (const InputEvent &e);
    void handleMotion   (const InputEvent &e);

    void parse();
    void compile();

};

enum shredState { shred_NEW, shred_RUNNING, shred_NEEDREMOVE, shred_KILLED, shred_NSTATE };


struct ShredInstance { 

    shredState status;

    CodeRevision * code;
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
        pickID = IDManager::instance()->getPickID();
    }

    void handleMouse ( const InputEvent & e);
    void dragAction( DragManager * d );
    void draw();
};


enum revState { rev_NEW = 0, rev_OPENED, rev_EDITED, rev_PARSED, 
                rev_TYPED, rev_COMPILED, rev_READY, rev_RUNNING, 
                rev_NSTATES };

class CodeRevision { 

protected:

    void *           _chuckEnv;
    static ck_socket _tcp;
    static char      _hostname[512];
    static int       _port;
    static int       _connected;

    TextBuffer *    _buffer; //may experience changes
    TextBuffer *    _original;  //initial version of file that 
                                //revision is set to
                                //if compilation successful, 
                                //the running version of the code. 

    a_Program       _parsed_program;
    tree_a_File     _parse_tree;
    int             _parse_edit_point;
    void *          _bytecode;

    revState        _status; //by the state of its shreds
    std::vector < ShredInstance > _shreds;
    uint            _id;
    bool            _selected;

public:

    CodeRevision();
    CodeRevision( TextBuffer * b);
    CodeRevision( CodeRevision * c );

    CodeRevision * splitModified();
    CodeRevision * dup();

    TextBuffer * getBuffer(void) { return _buffer; }
    void setBuffer(TextBuffer * b) { _buffer = b; _status = rev_NEW; }

    void            checkModified() { if ( _buffer->modified() ) _status = rev_EDITED; }
    revState        status() { return _status; }

    void handleMouse(const InputEvent &e);
    void dragAction( DragManager * d );
    uint id()               { return _id; }
    bool selected()         { return _selected; }
    void open( char * c );
    
    void            setVersion();
    int             parse();        //
    int             typecheck();    //needs parse
    int             compile();        //needs compile
    int             spork( int replace = 0 );
    int             size()  { return _shreds.size() + 1 ;}
    int             nshreds() { return _shreds.size(); }
    ShredInstance&  shred(int i) { return _shreds[i]; }

    int             findShred( int id );
    void            removeShred ( int id );
    int             sporkTCP();
    void            connect_tcp ( char * hostname = NULL , int port = 8888 );
    void            close_tcp();
    int             sendtcp( int replace = 0 );
    int             removetcp ( int id );
    
    void				draw();
};

class CodeWindowHandler : public WindowHandler { 
public:
    virtual bool handleKey  ( const InputEvent &e );
    virtual bool handleSpec ( const InputEvent &e );
    virtual bool winFromRev ( CodeRevision * r );
};


