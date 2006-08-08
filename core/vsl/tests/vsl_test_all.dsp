# Microsoft Developer Studio Project File - Name="vsl_tests" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=vsl_tests - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "vsl_tests.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "vsl_tests.mak" CFG="vsl_tests - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "vsl_tests - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "vsl_tests - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "vsl_tests - Win32 Release"

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
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "$(VXLROOT)\vcl\config.win32" /I "$(VXLROOT)\vcl" /I "$(VXLROOT)\core" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /FD /c
# ADD BASE RSC /l 0x809 /d "NDEBUG"
# ADD RSC /l 0x809 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 vsl.lib testlib.lib vcl.lib /libpath:"$(VXLROOT)\lib\Release"

!ELSEIF  "$(CFG)" == "vsl_tests - Win32 Debug"

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
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /I "$(VXLROOT)\vcl\config.win32" /I "$(VXLROOT)\vcl" /I "$(VXLROOT)\core" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x809 /d "_DEBUG"
# ADD RSC /l 0x809 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 vsl.lib testlib.lib vcl.lib /libpath:"$(VXLROOT)\lib\Debug"
# SUBTRACT LINK32 /verbose

!ENDIF 

# Begin Target

# Name "vsl_tests - Win32 Release"
# Name "vsl_tests - Win32 Debug"

# Begin Group "Source Files"
# PROP Default_Filter "cxx;c"
# Begin Source File
SOURCE=.\test_arbitrary_length_int_conversion.cxx
# End Source File
# Begin Source File
SOURCE=.\test_binary_io.cxx
# End Source File
# Begin Source File
SOURCE=.\test_clipon_polymorphic_io.cxx
# End Source File
# Begin Source File
SOURCE=.\test_complex_io.cxx
# End Source File
# Begin Source File
SOURCE=.\test_deque_io.cxx
# End Source File
# Begin Source File
SOURCE=.\test_driver.cxx
# End Source File
# Begin Source File
SOURCE=.\test_indent.cxx
# End Source File
# Begin Source File
SOURCE=.\test_list_io.cxx
# End Source File
# Begin Source File
SOURCE=.\test_map_io.cxx
# End Source File
# Begin Source File
SOURCE=.\test_polymorphic_io.cxx
# End Source File
# Begin Source File
SOURCE=.\test_set_io.cxx
# End Source File
# Begin Source File
SOURCE=.\test_stack_io.cxx
# End Source File
# Begin Source File
SOURCE=.\test_string_io.cxx
# End Source File
# Begin Source File
SOURCE=.\test_vector_io.cxx
# End Source File
# Begin Source File
SOURCE=.\test_vlarge_block_io.cxx
# End Source File
# Begin Source File
SOURCE=.\test_include.cxx
# End Source File
# End Group
# End Target
# End Project
