# Microsoft Developer Studio Project File - Name="audicle" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=audicle - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "audicle.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "audicle.mak" CFG="audicle - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "audicle - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "audicle - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "audicle - Win32 Release"

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
# ADD CPP /nologo /MT /W3 /GR /GX /O2 /I "..\ftgl_lib\FTGL\include\\" /I "..\ftgl_lib\freetype-2.1.10\include" /I "..\..\..\chuck_dev\v2" /D "NDEBUG" /D "WIN32" /D "_CONSOLE" /D "_MBCS" /D "__PLATFORM_WIN32__" /D "__WINDOWS_DS__" /D "__CHUCK_STAT_TRACK__" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib dsound.lib dinput.lib dxguid.lib winmm.lib wsock32.lib opengl32.lib glu32.lib glut32.lib /nologo /subsystem:console /machine:I386 /libpath:"..\ftgl_lib\FTGL\win32_vcpp\Build" /libpath:"..\ftgl_lib\freetype-2.1.10\objs\\"
# SUBTRACT LINK32 /nodefaultlib

!ELSEIF  "$(CFG)" == "audicle - Win32 Debug"

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
# ADD CPP /nologo /MDd /W3 /Gm /GR /GX /ZI /Od /I "..\..\..\chuck_dev\v2" /D "_DEBUG" /D "WIN32" /D "_CONSOLE" /D "_MBCS" /D "__PLATFORM_WIN32__" /D "__WINDOWS_DS__" /D "__CHUCK_STAT_TRACK__" /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 dinput.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib dsound.lib dxguid.lib winmm.lib wsock32.lib opengl32.lib glu32.lib glut32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# SUBTRACT LINK32 /incremental:no

!ENDIF 

# Begin Target

# Name "audicle - Win32 Release"
# Name "audicle - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\audicle.cpp
# End Source File
# Begin Source File

SOURCE=.\audicle_elcidua.cpp
# End Source File
# Begin Source File

SOURCE=.\audicle_event.cpp
# End Source File
# Begin Source File

SOURCE=.\audicle_face.cpp
# End Source File
# Begin Source File

SOURCE=.\audicle_face_compiler.cpp
# End Source File
# Begin Source File

SOURCE=.\audicle_face_editor.cpp
# End Source File
# Begin Source File

SOURCE=.\audicle_face_groove.cpp
# End Source File
# Begin Source File

SOURCE=.\audicle_face_mouse.cpp
# End Source File
# Begin Source File

SOURCE=.\audicle_face_shredder.cpp
# End Source File
# Begin Source File

SOURCE=.\audicle_face_skot.cpp
# End Source File
# Begin Source File

SOURCE=.\audicle_face_tnt.cpp
# End Source File
# Begin Source File

SOURCE=.\audicle_face_vmspace.cpp
# End Source File
# Begin Source File

SOURCE=.\audicle_font.cpp
# End Source File
# Begin Source File

SOURCE=.\audicle_geometry.cpp
# End Source File
# Begin Source File

SOURCE=.\audicle_gfx.cpp
# End Source File
# Begin Source File

SOURCE=.\audicle_import.cpp
# End Source File
# Begin Source File

SOURCE=.\audicle_main.cpp
# End Source File
# Begin Source File

SOURCE=.\audicle_net.cpp
# End Source File
# Begin Source File

SOURCE=.\audicle_nexus.cpp
# End Source File
# Begin Source File

SOURCE=.\audicle_session_manager.cpp
# End Source File
# Begin Source File

SOURCE=.\audicle_text_buffer.cpp
# End Source File
# Begin Source File

SOURCE=.\audicle_ui_base.cpp
# End Source File
# Begin Source File

SOURCE=.\audicle_ui_coding.cpp
# End Source File
# Begin Source File

SOURCE=.\audicle_ui_console.cpp
# End Source File
# Begin Source File

SOURCE=.\audicle_ui_editor.cpp
# End Source File
# Begin Source File

SOURCE=.\audicle_ui_parser.cpp
# End Source File
# Begin Source File

SOURCE=.\audicle_ui_windowing.cpp
# End Source File
# Begin Source File

SOURCE=.\audicle_utils.cpp
# End Source File
# Begin Source File

SOURCE=.\co.cpp
# End Source File
# Begin Source File

SOURCE=.\coaudicle_avatar.cpp
# End Source File
# Begin Source File

SOURCE=.\coaudicle_chat.cpp
# End Source File
# Begin Source File

SOURCE=.\coaudicle_net_session.cpp
# End Source File
# Begin Source File

SOURCE=.\coaudicle_protocol.cpp
# End Source File
# Begin Source File

SOURCE=.\coaudicle_ui_coding.cpp
# End Source File
# Begin Source File

SOURCE=.\coaudicle_view.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\chuck_dev\v2\hidio_sdl.cpp
# End Source File
# Begin Source File

SOURCE=.\rgbhsl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\chuck_dev\v2\uana_extract.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\chuck_dev\v2\uana_xform.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\chuck_dev\v2\util_hid.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\audicle.h
# End Source File
# Begin Source File

SOURCE=.\audicle_def.h
# End Source File
# Begin Source File

SOURCE=.\audicle_elcidua.h
# End Source File
# Begin Source File

SOURCE=.\audicle_event.h
# End Source File
# Begin Source File

SOURCE=.\audicle_face.h
# End Source File
# Begin Source File

SOURCE=.\audicle_face_compiler.h
# End Source File
# Begin Source File

SOURCE=.\audicle_face_editor.h
# End Source File
# Begin Source File

SOURCE=.\audicle_face_groove.h
# End Source File
# Begin Source File

SOURCE=.\audicle_face_mouse.h
# End Source File
# Begin Source File

SOURCE=.\audicle_face_shredder.h
# End Source File
# Begin Source File

SOURCE=.\audicle_face_skot.h
# End Source File
# Begin Source File

SOURCE=.\audicle_face_tnt.h
# End Source File
# Begin Source File

SOURCE=.\audicle_face_vmspace.h
# End Source File
# Begin Source File

SOURCE=.\audicle_font.h
# End Source File
# Begin Source File

SOURCE=.\audicle_geometry.h
# End Source File
# Begin Source File

SOURCE=.\audicle_gfx.h
# End Source File
# Begin Source File

SOURCE=.\audicle_import.h
# End Source File
# Begin Source File

SOURCE=.\audicle_net.h
# End Source File
# Begin Source File

SOURCE=.\audicle_nexus.h
# End Source File
# Begin Source File

SOURCE=.\audicle_session_manager.h
# End Source File
# Begin Source File

SOURCE=.\audicle_text_buffer.h
# End Source File
# Begin Source File

SOURCE=.\audicle_ui_base.h
# End Source File
# Begin Source File

SOURCE=.\audicle_ui_coding.h
# End Source File
# Begin Source File

SOURCE=.\audicle_ui_console.h
# End Source File
# Begin Source File

SOURCE=.\audicle_ui_editor.h
# End Source File
# Begin Source File

SOURCE=.\audicle_ui_parser.h
# End Source File
# Begin Source File

SOURCE=.\audicle_ui_windowing.h
# End Source File
# Begin Source File

SOURCE=.\audicle_utils.h
# End Source File
# Begin Source File

SOURCE=.\co.h
# End Source File
# Begin Source File

SOURCE=.\coaudicle.h
# End Source File
# Begin Source File

SOURCE=.\coaudicle_avatar.h
# End Source File
# Begin Source File

SOURCE=.\coaudicle_chat.h
# End Source File
# Begin Source File

SOURCE=.\coaudicle_local_session.h
# End Source File
# Begin Source File

SOURCE=.\coaudicle_net_session.h
# End Source File
# Begin Source File

SOURCE=.\coaudicle_protocol.h
# End Source File
# Begin Source File

SOURCE=.\coaudicle_remote_session.h
# End Source File
# Begin Source File

SOURCE=.\coaudicle_ui_coding.h
# End Source File
# Begin Source File

SOURCE=.\coaudicle_view.h
# End Source File
# Begin Source File

SOURCE=..\..\..\chuck_dev\v2\hidio_sdl.h
# End Source File
# Begin Source File

SOURCE=.\rgbhsl.h
# End Source File
# Begin Source File

SOURCE=..\..\..\chuck_dev\v2\uana_extract.h
# End Source File
# Begin Source File

SOURCE=..\..\..\chuck_dev\v2\uana_xform.h
# End Source File
# Begin Source File

SOURCE=..\..\..\chuck_dev\v2\util_hid.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# Begin Group "chuck"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\chuck_dev\v2\chuck_absyn.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\chuck_dev\v2\chuck_absyn.h
# End Source File
# Begin Source File

SOURCE=..\..\..\chuck_dev\v2\chuck_bbq.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\chuck_dev\v2\chuck_bbq.h
# End Source File
# Begin Source File

SOURCE=..\..\..\chuck_dev\v2\chuck_compile.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\chuck_dev\v2\chuck_compile.h
# End Source File
# Begin Source File

SOURCE=..\..\..\chuck_dev\v2\chuck_console.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\chuck_dev\v2\chuck_console.h
# End Source File
# Begin Source File

SOURCE=..\..\..\chuck_dev\v2\chuck_def.h
# End Source File
# Begin Source File

SOURCE=..\..\..\chuck_dev\v2\chuck_dl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\chuck_dev\v2\chuck_dl.h
# End Source File
# Begin Source File

SOURCE=..\..\..\chuck_dev\v2\chuck_emit.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\chuck_dev\v2\chuck_emit.h
# End Source File
# Begin Source File

SOURCE=..\..\..\chuck_dev\v2\chuck_errmsg.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\chuck_dev\v2\chuck_errmsg.h
# End Source File
# Begin Source File

SOURCE=..\..\..\chuck_dev\v2\chuck_frame.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\chuck_dev\v2\chuck_frame.h
# End Source File
# Begin Source File

SOURCE=..\..\..\chuck_dev\v2\chuck_globals.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\chuck_dev\v2\chuck_globals.h
# End Source File
# Begin Source File

SOURCE=..\..\..\chuck_dev\v2\chuck_instr.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\chuck_dev\v2\chuck_instr.h
# End Source File
# Begin Source File

SOURCE=..\..\..\chuck_dev\v2\chuck_lang.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\chuck_dev\v2\chuck_lang.h
# End Source File
# Begin Source File

SOURCE=..\..\..\chuck_dev\v2\chuck_oo.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\chuck_dev\v2\chuck_oo.h
# End Source File
# Begin Source File

SOURCE=..\..\..\chuck_dev\v2\chuck_otf.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\chuck_dev\v2\chuck_otf.h
# End Source File
# Begin Source File

SOURCE=..\..\..\chuck_dev\v2\chuck_parse.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\chuck_dev\v2\chuck_parse.h
# End Source File
# Begin Source File

SOURCE=..\..\..\chuck_dev\v2\chuck_scan.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\chuck_dev\v2\chuck_scan.h
# End Source File
# Begin Source File

SOURCE=..\..\..\chuck_dev\v2\chuck_shell.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\chuck_dev\v2\chuck_shell.h
# End Source File
# Begin Source File

SOURCE=..\..\..\chuck_dev\v2\chuck_stats.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\chuck_dev\v2\chuck_stats.h
# End Source File
# Begin Source File

SOURCE=..\..\..\chuck_dev\v2\chuck_symbol.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\chuck_dev\v2\chuck_symbol.h
# End Source File
# Begin Source File

SOURCE=..\..\..\chuck_dev\v2\chuck_table.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\chuck_dev\v2\chuck_table.h
# End Source File
# Begin Source File

SOURCE=..\..\..\chuck_dev\v2\chuck_type.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\chuck_dev\v2\chuck_type.h
# End Source File
# Begin Source File

SOURCE=..\..\..\chuck_dev\v2\chuck_ugen.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\chuck_dev\v2\chuck_ugen.h
# End Source File
# Begin Source File

SOURCE=..\..\..\chuck_dev\v2\chuck_utils.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\chuck_dev\v2\chuck_utils.h
# End Source File
# Begin Source File

SOURCE=..\..\..\chuck_dev\v2\chuck_vm.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\chuck_dev\v2\chuck_vm.h
# End Source File
# Begin Source File

SOURCE=..\..\..\chuck_dev\v2\chuck_win32.c
# End Source File
# Begin Source File

SOURCE=..\..\..\chuck_dev\v2\chuck_win32.h
# End Source File
# Begin Source File

SOURCE=..\..\..\chuck_dev\v2\digiio_rtaudio.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\chuck_dev\v2\digiio_rtaudio.h
# End Source File
# Begin Source File

SOURCE=..\..\..\chuck_dev\v2\midiio_rtmidi.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\chuck_dev\v2\midiio_rtmidi.h
# End Source File
# Begin Source File

SOURCE=..\..\..\chuck_dev\v2\rtaudio.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\chuck_dev\v2\rtaudio.h
# End Source File
# Begin Source File

SOURCE=..\..\..\chuck_dev\v2\rterror.h
# End Source File
# Begin Source File

SOURCE=..\..\..\chuck_dev\v2\rtmidi.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\chuck_dev\v2\rtmidi.h
# End Source File
# Begin Source File

SOURCE=..\..\..\chuck_dev\v2\ugen_filter.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\chuck_dev\v2\ugen_filter.h
# End Source File
# Begin Source File

SOURCE=..\..\..\chuck_dev\v2\ugen_osc.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\chuck_dev\v2\ugen_osc.h
# End Source File
# Begin Source File

SOURCE=..\..\..\chuck_dev\v2\ugen_stk.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\chuck_dev\v2\ugen_stk.h
# End Source File
# Begin Source File

SOURCE=..\..\..\chuck_dev\v2\ugen_xxx.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\chuck_dev\v2\ugen_xxx.h
# End Source File
# Begin Source File

SOURCE=..\..\..\chuck_dev\v2\ulib_machine.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\chuck_dev\v2\ulib_machine.h
# End Source File
# Begin Source File

SOURCE=..\..\..\chuck_dev\v2\ulib_math.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\chuck_dev\v2\ulib_math.h
# End Source File
# Begin Source File

SOURCE=..\..\..\chuck_dev\v2\ulib_opsc.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\chuck_dev\v2\ulib_opsc.h
# End Source File
# Begin Source File

SOURCE=..\..\..\chuck_dev\v2\ulib_std.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\chuck_dev\v2\ulib_std.h
# End Source File
# Begin Source File

SOURCE=..\..\..\chuck_dev\v2\util_buffers.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\chuck_dev\v2\util_buffers.h
# End Source File
# Begin Source File

SOURCE=..\..\..\chuck_dev\v2\util_console.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\chuck_dev\v2\util_console.h
# End Source File
# Begin Source File

SOURCE=..\..\..\chuck_dev\v2\util_math.c
# End Source File
# Begin Source File

SOURCE=..\..\..\chuck_dev\v2\util_math.h
# End Source File
# Begin Source File

SOURCE=..\..\..\chuck_dev\v2\util_network.c
# End Source File
# Begin Source File

SOURCE=..\..\..\chuck_dev\v2\util_network.h
# End Source File
# Begin Source File

SOURCE=..\..\..\chuck_dev\v2\util_opsc.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\chuck_dev\v2\util_opsc.h
# End Source File
# Begin Source File

SOURCE=..\..\..\chuck_dev\v2\util_raw.c
# End Source File
# Begin Source File

SOURCE=..\..\..\chuck_dev\v2\util_raw.h
# End Source File
# Begin Source File

SOURCE=..\..\..\chuck_dev\v2\util_sndfile.c
# End Source File
# Begin Source File

SOURCE=..\..\..\chuck_dev\v2\util_sndfile.h
# End Source File
# Begin Source File

SOURCE=..\..\..\chuck_dev\v2\util_string.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\chuck_dev\v2\util_string.h
# End Source File
# Begin Source File

SOURCE=..\..\..\chuck_dev\v2\util_thread.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\chuck_dev\v2\util_thread.h
# End Source File
# Begin Source File

SOURCE=..\..\..\chuck_dev\v2\util_xforms.c
# End Source File
# Begin Source File

SOURCE=..\..\..\chuck_dev\v2\util_xforms.h
# End Source File
# End Group
# End Target
# End Project
