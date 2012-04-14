# Microsoft Developer Studio Project File - Name="audicle_ckx_win32" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=audicle_ckx_win32 - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "audicle_ckx_win32.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "audicle_ckx_win32.mak" CFG="audicle_ckx_win32 - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "audicle_ckx_win32 - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "audicle_ckx_win32 - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "audicle_ckx_win32 - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release_ckx"
# PROP Intermediate_Dir "Release_ckx"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "AUDICLE_CKX_WIN32_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I "../../../chuck_dev/src" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "AUDICLE_CKX_WIN32_EXPORTS" /D "__PLATFORM_WIN32__" /D "__WINDOWS_DS__" /D "MSDOS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 ws2_32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386 /out:"Release_ckx/audicle.ckx"

!ELSEIF  "$(CFG)" == "audicle_ckx_win32 - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug_ckx"
# PROP Intermediate_Dir "Debug_ckx"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "AUDICLE_CKX_WIN32_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\..\..\chuck_dev\src" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "AUDICLE_CKX_WIN32_EXPORTS" /D "__PLATFORM_WIN32__" /D "__WINDOWS_DS__" /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib ws2_32.lib /nologo /dll /debug /machine:I386 /out:"Debug_ckx/audicle.ckx" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "audicle_ckx_win32 - Win32 Release"
# Name "audicle_ckx_win32 - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\audicle_base.cpp
# End Source File
# Begin Source File

SOURCE=.\audicle_ckx.cpp
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

SOURCE=.\audicle_ckx.h
# End Source File
# Begin Source File

SOURCE=.\audicle_coding.h
# End Source File
# Begin Source File

SOURCE=..\..\..\chuck_dev\src\chuck_absyn.h
# End Source File
# Begin Source File

SOURCE=..\..\..\chuck_dev\src\chuck_errmsg.h
# End Source File
# Begin Source File

SOURCE=..\..\..\chuck_dev\src\chuck_otf.h
# End Source File
# Begin Source File

SOURCE=..\..\..\chuck_dev\src\chuck_symbol.h
# End Source File
# Begin Source File

SOURCE=..\..\..\chuck_dev\src\chuck_table.h
# End Source File
# Begin Source File

SOURCE=..\..\..\chuck_dev\src\chuck_utils.h
# End Source File
# Begin Source File

SOURCE=..\..\..\chuck_dev\src\chuck_win32.h
# End Source File
# Begin Source File

SOURCE=.\dirscan.h
# End Source File
# Begin Source File

SOURCE=.\geometry.h
# End Source File
# Begin Source File

SOURCE=.\glext.h
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
