# Microsoft Developer Studio Project File - Name="test_vgl_io" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=test_vgl_io - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "test_vgl_io.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "test_vgl_io.mak" CFG="test_vgl_io - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "test_vgl_io - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "test_vgl_io - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "test_vgl_io - Win32 Release"

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
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "$(VXLROOT)\vcl\config.win32" /I "$(VXLROOT)\vcl" /I "$(VXLROOT)\core" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /FD /c
# ADD BASE RSC /l 0x809 /d "NDEBUG"
# ADD RSC /l 0x809 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 vgl_io.lib vsl.lib vul.lib vgl.lib vcl.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386 /libpath:"$(VXLROOT)\lib\Release"

!ELSEIF  "$(CFG)" == "test_vgl_io - Win32 Debug"

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
# ADD LINK32 vgl_io.lib vsl.lib vul.lib vgl.lib vcl.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept /libpath:"$(VXLROOT)\lib\Debug"
# SUBTRACT LINK32 /verbose

!ENDIF 

# Begin Target

# Name "test_vgl_io - Win32 Release"
# Name "test_vgl_io - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File
SOURCE=.\test_driver.cxx
# End Source File
# Begin Source File
SOURCE=.\golden_test_vgl_io.cxx
# End Source File
# Begin Source File
SOURCE=.\test_box_2d_io.cxx
# End Source File
# Begin Source File
SOURCE=.\test_box_3d_io.cxx
# End Source File
# Begin Source File
SOURCE=.\test_conic_io.cxx
# End Source File
# Begin Source File
SOURCE=.\test_homg_line_2d_io.cxx
# End Source File
# Begin Source File
SOURCE=.\test_line_3d_2_points_io.cxx
# End Source File
# Begin Source File
SOURCE=.\test_homg_line_3d_2_points_io.cxx
# End Source File
# Begin Source File
SOURCE=.\test_homg_plane_3d_io.cxx
# End Source File
# Begin Source File
SOURCE=.\test_homg_point_1d_io.cxx
# End Source File
# Begin Source File
SOURCE=.\test_homg_point_2d_io.cxx
# End Source File
# Begin Source File
SOURCE=.\test_homg_point_3d_io.cxx
# End Source File
# Begin Source File
SOURCE=.\test_includes.cxx
# End Source File
# Begin Source File
SOURCE=.\test_line_2d_io.cxx
# End Source File
# Begin Source File
SOURCE=.\test_line_segment_2d_io.cxx
# End Source File
# Begin Source File
SOURCE=.\test_line_segment_3d_io.cxx
# End Source File
# Begin Source File
SOURCE=.\test_point_2d_io.cxx
# End Source File
# Begin Source File
SOURCE=.\test_point_3d_io.cxx
# End Source File
# Begin Source File
SOURCE=.\test_polygon_io.cxx
# End Source File
# Begin Source File
SOURCE=.\test_vector_2d_io.cxx
# End Source File
# Begin Source File
SOURCE=.\test_vector_3d_io.cxx
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# End Group
# End Target
# End Project
