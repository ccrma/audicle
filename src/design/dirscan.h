#ifndef _DIRSCAN_H_INCLUDED_
#define _DIRSCAN_H_INCLUDED_

#include "sys.h"

void listworkingdir();

struct fileData { 
    bool isDir;
    string fileName;
    int  size;

    struct fileData * next;
    
    fileData() { isDir = false; next = NULL; }
    ~fileData() { if ( next ) delete next; }
    fileData * clone();
};

class DirScannerImp;

class DirScanner { 
protected:
   DirScannerImp *_imp;
public:
   
   DirScanner();
   void  defineSearch(char * directory, char * pattern);
   fileData*  openFileDialog();
   fileData*  saveFileDialog();
   bool  hasFile();                                       
   void  getFile( fileData * fD );                                      
};

class DirScannerImp { 
public:
    DirScannerImp() {}
    virtual void  defineSearch(char * directory, char * pattern)   {}
    virtual bool  hasFile()                                        {return false;}
    virtual void  getFile(fileData * fD)                       {}                           
    virtual fileData* openFileDialog() { return NULL; }
    virtual fileData* saveFileDialog() { return NULL; } 
};

#ifdef __PLATFORM_WIN32__

class winDirScanner : public DirScannerImp { 
protected:
    WIN32_FIND_DATA _file;
    HANDLE          _hFile;
    char            _patbuf[512];
    char            _retbuf[512];
    BOOL            _hasFile;
public:
    winDirScanner() : _hasFile(false) {}
    virtual void  defineSearch(char * directory, char * pattern );
    virtual bool  hasFile();
    virtual void  getFile(fileData * fD);
    fileData* openFileDialog();
    fileData* saveFileDialog();
};

#else

#include <dirent.h>
#include <sys/stat.h>

class UnixDirScanner : public DirScannerImp {

protected:
    
    DIR * _dir;
    struct dirent * _ent;
    struct stat filestat;
    char _pathbuf[512];
    char _patternbuf[512];
    char _suffix[32];
    bool _hasFile;
    bool _hasPat;
    
public:
    UnixDirScanner() : _hasFile(false), _hasPat(false) {}
    virtual void defineSearch(char * path, char * pattern);
    virtual bool hasFile();
    virtual void getFile(fileData * fD);
    virtual fileData* openFileDialog(); 
    virtual fileData* saveFileDialog();  
};

#ifdef __PLATFORM_MACOSX__ 

class MacOSXDirScanner : public UnixDirScanner { 
public:
    fileData* openFileDialog();
    fileData* saveFileDialog();
};

#endif

#endif


#endif // _DIRSCAN_H_INCLUDED_
