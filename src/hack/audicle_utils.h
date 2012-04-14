//-----------------------------------------------------------------------------
// name: audicle_utils.h
// desc: ...
//
// authors: Ge Wang (gewang@cs.princeton.edu)
//          Perry R. Cook (prc@cs.princeton.edu)
//          Philip Davidson (philipd@cs.princeton.edu)
//          Ananya Misra (amisra@cs.princeton.edu)
// date: Autumn 2004
//-----------------------------------------------------------------------------
#ifndef __AUDICLE_UTILS_H__
#define __AUDICLE_UTILS_H__

#include "audicle_def.h"



void listworkingdir();

struct fileData { 
    bool isDir;
    std::string fileName;
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

#include <windows.h>
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


class PlatformFactory { 
protected:
   static PlatformFactory *_instance;
public:

   PlatformFactory() {}
   static PlatformFactory *Instance();
   DirScannerImp*  DirScan();

};

double GetTime(void);

int fgetline(char**buf, int *size, FILE *fp);

double interp ( int a,     int b,      double w );
double interp ( double a,  double b,   double w );


enum fmode { FMODE_CLOSED, FMODE_FREAD, FMODE_FWRITE, FMODE_NREAD, FMODE_NWRITE, FMODE_NUM };
#define CBUFSIZ 2048

class fmBuffer { 

protected:
   fmode    _mode;
   std::string   _buffer;
   char     _line[CBUFSIZ];
   int      _pos;
   bool     _ready;
public:

   fmBuffer() : _mode(FMODE_CLOSED) 
   {  
      _buffer.erase(); 
      _buffer.reserve(CBUFSIZ); 
      _line[0] = '\0';
   }

   fmBuffer( fmode m ) : _mode(m) 
   {  
      _buffer.erase(); 
      _buffer.reserve(CBUFSIZ);
      _line[0] = '\0';
   }

   void  flush();

   void  smode( fmode m )  { _mode = m; }  
   void  writefile(char* w);
   int   readfile(char* r);

   void  netrecv(char * address, int port); //TCP IP
   void  netsend(char * address, int port); //TCP IP

   void  close();
   
   int      getline(char ** sc, int * sz);
   int      getline(std::string &s);
   char*    in();
   char*    data()  { flush(); return (char*)_buffer.c_str(); }

};

int tokenize(char *buf, char * tok, char ***args, int * size );

#endif
