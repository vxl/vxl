# Microsoft Developer Studio Project File - Name="vcl_tests" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=vcl_tests - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE
!MESSAGE NMAKE /f "vcl_tests.mak".
!MESSAGE
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE
!MESSAGE NMAKE /f "vcl_tests.mak" CFG="vcl_tests - Win32 Debug"
!MESSAGE
!MESSAGE Possible choices for configuration are:
!MESSAGE
!MESSAGE "vcl_tests - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "vcl_tests - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "vcl_tests - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /Ob2 /I "$(VXLROOT)\vcl\config.win32" /I "$(VXLROOT)\vcl" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /FD /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 /nologo /subsystem:console /machine:I386

!ELSEIF  "$(CFG)" == "vcl_tests - Win32 Debug"

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
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /Ob2 /I "$(VXLROOT)\vcl\config.win32" /I "$(VXLROOT)\vcl" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /FR /FD /GZ /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept

!ENDIF

# Begin Target

# Name "vcl_tests - Win32 Release"
# Name "vcl_tests - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File
SOURCE=.\test_driver.cxx
# End Source File
# Begin Source File
SOURCE=.\test_include.cxx
# End Source File
# Begin Source File
SOURCE=.\test_algorithm.cxx
# End Source File
# Begin Source File
SOURCE=.\test_cctype.cxx
# End Source File
# Begin Source File
SOURCE=.\test_cmath.cxx
# End Source File
# Begin Source File
SOURCE=.\test_compiler.cxx
# End Source File
# Begin Source File
SOURCE=.\test_complex.cxx
# End Source File
# Begin Source File
SOURCE=.\test_deque.cxx
# End Source File
# Begin Source File
SOURCE=.\test_exception.cxx
# End Source File
# Begin Source File
SOURCE=.\test_fstream.cxx
# End Source File
# Begin Source File
SOURCE=.\test_sstream.cxx
# End Source File
# Begin Source File
SOURCE=.\test_iostream.cxx
# End Source File
# Begin Source File
SOURCE=.\test_iterator.cxx
# End Source File
# Begin Source File
SOURCE=.\test_list.cxx
# End Source File
# Begin Source File
SOURCE=.\test_map.cxx
# End Source File
# Begin Source File
SOURCE=.\test_multimap.cxx
# End Source File
# Begin Source File
SOURCE=.\test_new.cxx
# End Source File
# Begin Source File
SOURCE=.\test_set.cxx
# End Source File
# Begin Source File
SOURCE=.\test_stlfwd.cxx
# End Source File
# Begin Source File
SOURCE=.\test_string.cxx
# End Source File
# Begin Source File
SOURCE=.\test_vector.cxx
# End Source File
# Begin Source File
SOURCE=.\test_cstdio.cxx
# End Source File
# Begin Source File
SOURCE=.\test_limits.cxx
# End Source File
# Begin Source File
SOURCE=.\test_memory.cxx
# End Source File
# End Group
# End Target
# End Project
