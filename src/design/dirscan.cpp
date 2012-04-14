
#include "dirscan.h"
#include "platform.h"
//abstract def: 

fileData * openfD = NULL;
fileData * savefD = NULL;


fileData * 
fileData::clone() { 
    fileData * clon = new fileData();
    *clon = *this;
    if ( next ) { 
        clon->next = next->clone();
    }
    return clon;
}
void
listworkingdir(void) { 
		
   fprintf(stderr, "current directory ( . ) \n" ) ;

#ifdef __PLATFORM_WIN32__
   WIN32_FIND_DATA _file;
   HANDLE          _hFile;
   if ( (_hFile = FindFirstFile( ".", &_file)) != INVALID_HANDLE_VALUE ) { 
       do { 
           fprintf(stderr, "\t- %s \n", _file.cFileName);
       } while ( FindNextFile(_hFile, &_file) ); 
       FindClose(_hFile);
   }
#else
   DIR * dr = opendir(".");
   if ( dr ) { 
       struct dirent* d;
       while ( d = readdir(dr) ) { 
           fprintf(stderr, "\t- %s \n", d->d_name );
       }
       closedir(dr);
   }
#endif
   fprintf(stderr, "end directory\n");
}

DirScanner::DirScanner() {
   _imp = PlatformFactory::Instance()->DirScan();
}

void  
DirScanner::defineSearch(char * directory, char * pattern)  
{ 
   _imp->defineSearch(directory, pattern);  
} 

bool  
DirScanner::hasFile()                                       
{ 
   return _imp->hasFile();                  
}

void  
DirScanner::getFile(fileData * fD)
{ 
   _imp->getFile(fD);
} 

fileData * 
DirScanner::saveFileDialog() { 
    fprintf(stderr, "DirScanner - dispatch\n");
    return _imp->saveFileDialog();
}

fileData * 
DirScanner::openFileDialog() { 
    fprintf(stderr, "DirScanner - dispatch\n");
    return _imp->openFileDialog();
}

#ifdef __PLATFORM_WIN32__

void
winDirScanner::defineSearch( char * path, char * pattern ) { 
   strcpy ( _patbuf, path );
   strcat ( _patbuf, "\\");
   strcat ( _patbuf, pattern );
   _hasFile = false;
   if ( (_hFile = FindFirstFile( _patbuf, &_file)) != INVALID_HANDLE_VALUE )
      _hasFile =true;
}

bool
winDirScanner::hasFile() { 
   return ((bool)_hasFile);
}

void
winDirScanner::getFile(fileData * fD) { 
	if ( !_hasFile ) 
		return;

	fD->fileName = _file.cFileName;
	fD->isDir = (_file.dwFileAttributes && FILE_ATTRIBUTE_DIRECTORY ) ? true : false ;

	if ( !(_hasFile = FindNextFile(_hFile, &_file)) ) 
		FindClose(_hFile);
}



fileData* 
winDirScanner::openFileDialog() { 

    char newname[512];
    newname[0] = '\0';
    
    if ( openfD ) { 
        delete openfD;
        openfD = NULL;
    }
    //XXX make this non-modal 
    fprintf(stderr, "fetching WIN32 file dialog\n");
    
    ShowCursor(true);
    OPENFILENAME ofn;
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lpstrFile = newname;
    ofn.lStructSize = sizeof(ofn); // SEE NOTE BELOW
    ofn.hwndOwner = NULL;
    ofn.lpstrFilter = "Chuck Files (*.ck)\0*.ck\0All Files (*.*)\0*.*\0";
    ofn.lpstrFile = newname;
    ofn.nMaxFile = MAX_PATH;
    ofn.Flags = OFN_EXPLORER | OFN_ALLOWMULTISELECT | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
    ofn.lpstrDefExt = "txt";
    
    if ( GetOpenFileName( &ofn ) && ofn.lpstrFile ) {
        fprintf(stderr, "file - %s %s\n", ofn.lpstrFile, ofn.lpstrFileTitle );
		openfD = new fileData();
		fileData * newFile = openfD;
		char * next = newname; 
		next += strlen(next) + 1;
		if ( *next ) { 
			char basepath[512];
			char fullpath[512];
			strncpy ( basepath, newname, 512 );
			strcat ( basepath, "\\" );
			do  { 
				strcpy ( fullpath, basepath );
				strcat ( fullpath, next );
				fprintf ( stderr, "returned multi file %s\n", fullpath );
				newFile->fileName = fullpath;
				next += strlen(next) + 1;
				if ( *next ) { 
					newFile->next = new fileData();
					newFile = newFile->next;
				}
			} while ( *next ) ;
		} 
		else { //no extra files, we copied the full path in already
			fprintf ( stderr, "returned single file %s\n", newname );
			openfD->fileName = newname;
		}

	}
    else { 
        fprintf(stderr, "openfilename error %d: \n", CommDlgExtendedError());
        return NULL;
    }
    ShowCursor(false);


    return openfD;
}

fileData* 
winDirScanner::saveFileDialog() { 

    if ( savefD ) { 
        delete savefD;
        savefD = NULL;
    }

	fprintf(stderr, "win32 save file dialog\n");
    char newname[512];
    newname[0] = '\0';
    ShowCursor(true);
    OPENFILENAME ofn;
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lpstrFile = newname;
    ofn.lStructSize = sizeof(ofn); // SEE NOTE BELOW
    ofn.hwndOwner = NULL;
    ofn.lpstrFilter = "Chuck Files (*.ck)\0*.ck\0All Files (*.*)\0*.*\0";
    ofn.lpstrFile = newname;
    ofn.nMaxFile = MAX_PATH;
    ofn.Flags = OFN_EXPLORER | OFN_HIDEREADONLY;
    ofn.lpstrDefExt = "txt";
    
    if ( GetSaveFileName( &ofn ) ) { 
        fprintf(stderr, "file - %s\n", ofn.lpstrFile );
        strncpy ( newname, ofn.lpstrFile, 512 );
        savefD = new fileData();
        savefD->fileName = newname;
    }
    else { 
        fprintf(stderr, "openfilename error %d: \n", CommDlgExtendedError());
        return NULL;
    }

    ShowCursor(false);

    return savefD;

}

#else

void
UnixDirScanner::defineSearch ( char * path, char * pattern ) {
    strcpy ( _pathbuf, path ) ;
    if ( pattern ) strcpy ( _patternbuf, pattern ) ;
    fprintf (stderr, "scanner : searching '%s' for '%s'\n", path, pattern );
    fprintf (stderr, " current directory : %s\n", getenv ("PWD") );
    if ( pattern && strlen (pattern) > 0 ) {
        char * s = strrchr (_patternbuf, '*');
        if ( s )  	strcpy ( _suffix, s+1 );
        else 	strcpy ( _suffix , _patternbuf );
        _hasPat = true;
    }
    else _hasPat = false;
    
    if ( (_dir = opendir(_pathbuf)) == NULL ) {
        fprintf(stderr, "directory open failed (%s)\n", _pathbuf);
        _hasFile = false;
        return;
    }
    fprintf(stderr, "open succeeded\n");
    while ( (_ent = readdir ( _dir )) != NULL && ( _hasPat && !strstr (_ent->d_name, _suffix) ) ) { printf("skipped %s\n", _ent->d_name); }
    
    if ( _ent ) _hasFile = true;
    else closedir(_dir);
    
}

fileData* 
UnixDirScanner::openFileDialog()
{ 
  char newfilebuf[512];
  fprintf(stderr, "\nopen file : \n" );

  scanf ("%s", newfilebuf );
  if ( openfD ) { 
    delete openfD;
    openfD = NULL;
  }
  if ( strlen( newfilebuf  ) ) {  
    openfD = new fileData();
    openfD->fileName = newfilebuf; 
    openfD->isDir = false;
  }
  return openfD;

} 
 
fileData* 
UnixDirScanner::saveFileDialog() 
{ 
  char newfilebuf[512];
  fprintf(stderr, "\nsave file : \n" );
  scanf ("%s", newfilebuf );
  
  if ( savefD ) { 
        delete savefD;
        savefD = NULL;
  }
  if ( strlen ( newfilebuf ) ) { 
    savefD = new fileData();
    savefD->fileName = newfilebuf; 
    savefD->isDir = false;
  }
  return savefD;

}

bool
UnixDirScanner::hasFile() {
    return _hasFile;
}

void
UnixDirScanner::getFile( fileData * fD ) {

    if ( !_hasFile ) return;
    
    fD->fileName = _ent->d_name;
	stat ( _ent->d_name, &filestat );
	//fD->isDir = ( IS_DIR(filestat.st_mode) ) ? true : false ;
	fD->isDir = false;

    while ( (_ent = readdir ( _dir )) != NULL && ( _hasPat && !strstr (_ent->d_name, _suffix) ) ) { printf("skipped %s\n", _ent->d_name); }

    if ( !_ent ) { 
        _hasFile = false;
        closedir ( _dir ) ;
    }
    
}

#ifdef __PLATFORM_MACOSX__

NavEventUPP gNavEventHandlerPtr;



void userActionCallback ( NavCBRecPtr callBackParms ) { 


    OSStatus err;
    long int n;
    NavReplyRecord * nReply = new NavReplyRecord;
    err = NavDialogGetReply ( callBackParms->context, nReply );
    if ( err != noErr ) {
        NavDisposeReply( nReply );
        return;
    } 

    NavUserAction userAction; 
    userAction = NavDialogGetUserAction( callBackParms->context );

    switch ( userAction ) { 
	case kNavUserActionOpen:

		AECountItems( &(nReply->selection), &n ) ;

		if ( n != 0 ) { 
			AEKeyword aeKey;
			AEDesc record;
			FSRef fref;
			char newfilename[512];
			openfD = new fileData();
			fileData * newfD = openfD;
			for ( int i = 0; i < n ; i++ ) { 
				if ( i != 0 ) { 
                                    newfD->next = new fileData();
                                    newfD = newfD->next;
				}                

				AEGetNthDesc (&(nReply->selection), i+1, typeFSRef, &aeKey, &record );
				err = AEGetDescData( &record, ( void * )( &fref ), sizeof( FSRef ) );      
				FSRefMakePath( &fref, (UInt8*)newfilename, 512);
				fprintf(stderr, "hey %s\n", newfilename );
				newfD->fileName = newfilename;
				newfD->isDir = false;
			}
		}

		break;
    case kNavUserActionSaveAs:

        AECountItems( &(nReply->selection), &n ) ;

        if ( n != 0 ) { 
            AEKeyword aeKey;
            AEDesc record;
            FSRef fref;
            char newfilename[512];
            char fileSaveName[512];
            savefD = new fileData();
            for ( int i = 0; i < n ; i++ ) { 
                AEGetNthDesc (&(nReply->selection), i+1, typeFSRef, &aeKey, &record );
                err = AEGetDescData( &record, ( void * )( &fref ), sizeof( FSRef ) );      
                FSRefMakePath( &fref, (UInt8*)newfilename, 512);
                fprintf(stderr, "hey %s\n", newfilename );
                savefD->fileName = newfilename;
                savefD->isDir = false;

                char * fname  = CFStringGetCStringPtr( nReply->saveFileName, kCFStringEncodingASCII );
                if ( fname ) {
                    fprintf(stderr, "file is %s\n", fname);
                    savefD->fileName += "/";
                    savefD->fileName += fname;
                }
                else { 
                    CFStringGetCString ( nReply->saveFileName, (char*)fileSaveName, 512, kCFStringEncodingASCII );
                    savefD->fileName += "/";
                    savefD->fileName += fileSaveName;
                    fprintf(stderr, "no filename given\n");
                }

            }
        }

        break;

	}
    NavDisposeReply( nReply );
    
}

pascal void dialogFileCallback (
    NavEventCallbackMessage callBackSelector, 
    NavCBRecPtr  callBackParams, 
    void*   callBackUD ) { 


    switch ( callBackSelector ) { 
    case kNavCBUserAction:
        userActionCallback( callBackParams );
        break;
    case kNavCBTerminate:
        NavDialogDispose( callBackParams->context );
        DisposeNavEventUPP( gNavEventHandlerPtr );
        break;
    }
}

fileData*
MacOSXDirScanner::openFileDialog() { 

    OSStatus   err;
    NavDialogRef  openDialog;
    NavDialogCreationOptions dialogAttributes;
    
    int n = 0;

    if ( openfD ) { 
        delete openfD;
        openfD = NULL;
    }
    
    err = NavGetDefaultDialogCreationOptions( &dialogAttributes );
    
    dialogAttributes.modality = kWindowModalityAppModal;   
    
    gNavEventHandlerPtr = NewNavEventUPP( dialogFileCallback );    
    
    err = NavCreateGetFileDialog( &dialogAttributes, NULL, 
                                  gNavEventHandlerPtr, NULL, NULL, 
                                  NULL, &openDialog );
    
    if ( !cursorOn ) { ShowCursor(); cursorOn = true; }
    err = NavDialogRun( openDialog );
    
    if ( err != noErr )
    {
        NavDialogDispose( openDialog );
        DisposeNavEventUPP( gNavEventHandlerPtr );
        return NULL;
    }

    
    return openfD;
}

fileData*
MacOSXDirScanner::saveFileDialog() { 

	//XXX not finished yet. 

	fprintf(stderr, "start OSX File Save Dialog\n");
    OSStatus   err;
    NavDialogRef  saveDialog;

    NavDialogCreationOptions dialogAttributes;
        
    int n = 0;
    if ( savefD ) { 
        delete savefD;
        savefD = NULL;
    }

    err = NavGetDefaultDialogCreationOptions( &dialogAttributes );
    
    dialogAttributes.modality = kWindowModalityAppModal;   
    
    gNavEventHandlerPtr = NewNavEventUPP( dialogFileCallback );    
    
    err = NavCreatePutFileDialog( &dialogAttributes, 'ChuK', kNavGenericSignature,
                                  gNavEventHandlerPtr, NULL, &saveDialog );
    
    if ( !cursorOn ) { ShowCursor(); cursorOn = true; }
    err = NavDialogRun( saveDialog );
    
    if ( err != noErr )
    {
        NavDialogDispose( saveDialog );
        DisposeNavEventUPP( gNavEventHandlerPtr );
        return NULL;
    }
    
    
    return savefD;
}

#endif

#endif
