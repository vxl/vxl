# Microsoft Developer Studio Project File - Name="vgl_test_all" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=vgl_test_all - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "vgl_test_all.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "vgl_test_all.mak" CFG="vgl_test_all - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "vgl_test_all - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "vgl_test_all - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "vgl_test_all - Win32 Release"

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
# ADD CPP /nologo /MD /W3 /GX /O2 /Ob2 /I "$(VXLROOT)\vcl\config.win32" /I "$(VXLROOT)\vcl" /I "$(VXLROOT)\core" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 vcl.lib /libpath:"$(VXLROOT)/vcl/$(OUTDIR)" /nologo /subsystem:console /machine:I386

!ELSEIF  "$(CFG)" == "vgl_test_all - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /Ob0 /I "$(VXLROOT)\vcl\config.win32" /I "$(VXLROOT)\vcl" /I "$(VXLROOT)\core" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 vcl.lib /libpath:"$(VXLROOT)/vcl/$(OUTDIR)" /nologo /subsystem:console /machine:I386

!ENDIF 

# Begin Target

# Name "vgl_test_all - Win32 Release"
# Name "vgl_test_all - Win32 Debug"
# Begin Group "Source Files"
# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File
SOURCE=.\test_driver.cxx
# End Source File
# Begin Source File
SOURCE=.\test_include.cxx
# End Source File
# Begin Source File
SOURCE=.\test_area.cxx
# End Source File
# Begin Source File
SOURCE=.\test_cartesian.cxx
# End Source File
# Begin Source File
SOURCE=.\test_clip.cxx
# End Source File
# Begin Source File
SOURCE=.\test_conic.cxx
# End Source File
# Begin Source File
SOURCE=.\test_distance.cxx
# End Source File
# Begin Source File
SOURCE=.\test_ellipse_scan_iterator.cxx
# End Source File
# Begin Source File
SOURCE=.\test_homg.cxx
# End Source File
# Begin Source File
SOURCE=.\test_polygon.cxx
# End Source File
# Begin Source File
SOURCE=.\test_polygon_scan_iterator.cxx
# End Source File
# Begin Source File
SOURCE=.\test_h_matrix_1d.cxx
# End Source File
# Begin Source File
SOURCE=.\test_h_matrix_2d.cxx
# End Source File
# Begin Source File
SOURCE=.\test_h_matrix_3d.cxx
# End Source File
# Begin Source File
SOURCE=.\test_p_matrix.cxx
# End Source File
# Begin Source File
SOURCE=.\test_triangle_scan_iterator.cxx
# End Source File
# Begin Source File
SOURCE=.\test_fit_lines_2d.cxx
# End Source File
# Begin Source File
SOURCE=.\test_closest_point.cxx
# End Source File
# Begin Source File
SOURCE=.\test_window_scan_iterator.cxx
# End Source File
# Begin Source File
SOURCE=.\test_convex.cxx
# End Source File
# Begin Source File
SOURCE=.\test_convex_hull_2d.cxx
# End Source File
# Begin Source File
SOURCE=.\test_fit_conics_2d.cxx
# End Source File
# Begin Source File
SOURCE=.\test_line_3d_2_points.cxx
# End Source File
# Begin Source File
SOURCE=.\test_sphere.cxx
# End Source File
# Begin Source File
SOURCE=.\test_line_segment_3d.cxx
# End Source File
# Begin Source File
SOURCE=.\test_orient_box_3d.cxx
# End Source File
# Begin Source File
SOURCE=.\test_fit_plane_3d.cxx
# End Source File
# End Group
# End Target
# End Project
