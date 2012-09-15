# Microsoft Developer Studio Project File - Name="texteditor" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=texteditor - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "texteditor.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "texteditor.mak" CFG="texteditor - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "texteditor - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "texteditor - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "texteditor - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I "../../../chuck_dev/src" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /D "__WINDOWS_DS__" /D "__PLATFORM_WIN32__" /D "MSDOS" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 ws2_32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386

!ELSEIF  "$(CFG)" == "texteditor - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "../../../chuck_dev/src" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /D "MSDOS" /D "__PLATFORM_WIN32__" /D "__WINDOWS_DS__" /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 Ws2_32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "texteditor - Win32 Release"
# Name "texteditor - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\audicle_base.cpp
# End Source File
# Begin Source File

SOURCE=.\audicle_coding.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\chuck_dev\src\chuck_absyn.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\chuck_dev\src\chuck_errmsg.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\chuck_dev\src\chuck_otf.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\chuck_dev\src\chuck_symbol.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\chuck_dev\src\chuck_table.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\chuck_dev\src\chuck_utils.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\chuck_dev\src\chuck_win32.c
# End Source File
# Begin Source File

SOURCE=.\dirscan.cpp
# End Source File
# Begin Source File

SOURCE=.\geometry.cpp
# End Source File
# Begin Source File

SOURCE=.\parse_gui.cpp
# End Source File
# Begin Source File

SOURCE=.\platform.cpp
# End Source File
# Begin Source File

SOURCE=.\text_main.cpp
# End Source File
# Begin Source File

SOURCE=.\textbuffer.cpp
# End Source File
# Begin Source File

SOURCE=.\texteditor.cpp
# End Source File
# Begin Source File

SOURCE=.\thread.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\chuck_dev\src\util_network.c
# End Source File
# Begin Source File

SOURCE=.\windowing.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\audicle_base.h
# End Source File
# Begin Source File

SOURCE=.\audicle_coding.h
# End Source File
# Begin Source File

SOURCE=..\..\..\chuck_dev\src\chuck_otf.h
# End Source File
# Begin Source File

SOURCE=.\dirscan.h
# End Source File
# Begin Source File

SOURCE=.\geometry.h
# End Source File
# Begin Source File

SOURCE=.\parse_gui.h
# End Source File
# Begin Source File

SOURCE=.\platform.h
# End Source File
# Begin Source File

SOURCE=.\sys.h
# End Source File
# Begin Source File

SOURCE=.\textbuffer.h
# End Source File
# Begin Source File

SOURCE=.\texteditor.h
# End Source File
# Begin Source File

SOURCE=.\thread.h
# End Source File
# Begin Source File

SOURCE=..\..\..\chuck_dev\src\util_network.h
# End Source File
# Begin Source File

SOURCE=.\windowing.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
