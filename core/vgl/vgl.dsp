# Microsoft Developer Studio Project File - Name="vgl" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=vgl - Win32 StaticDebug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "vgl.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "vgl.mak" CFG="vgl - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "vgl - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "vgl - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "vgl - Win32 StaticDebug" (based on "Win32 (x86) Static Library")
!MESSAGE "vgl - Win32 StaticRelease" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=xicl6.exe
RSC=rc.exe

!IF  "$(CFG)" == "vgl - Win32 Release"

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
MTL=midl.exe
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /Ob2 /I "$(IUEROOT)\vcl\config.win32-vc60" /I "$(IUEROOT)\vcl" /I "$(IUEROOT)\vxl" /D "WIN32" /D "NDEBUG" /D "_LIB" /YX /FD /c /Zl
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=xilink6.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\Release\vgl.lib"

!ELSEIF  "$(CFG)" == "vgl - Win32 Debug"

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
MTL=midl.exe
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /Ob2 /I "$(IUEROOT)\vcl\config.win32-vc60" /I "$(IUEROOT)\vcl" /I "$(IUEROOT)\vxl" /D "WIN32" /D "_DEBUG" /D "_LIB" /FD /GZ /c /Zl
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=xilink6.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\Debug\vgl.lib"

!ELSEIF  "$(CFG)" == "vgl - Win32 StaticDebug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "StaticDebug"
# PROP BASE Intermediate_Dir "StaticDebug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "StaticDebug"
# PROP Intermediate_Dir "StaticDebug"
# PROP Target_Dir ""
MTL=midl.exe
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /Ob2 /D "WIN32" /D "_DEBUG" /D "_LIB" /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /Ob2 /I "$(IUEROOT)\vcl\config.win32-vc60" /I "$(IUEROOT)\vcl" /I "$(IUEROOT)\vxl" /D "WIN32" /D "_DEBUG" /D "_LIB" /FD /GZ /c /Zl
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=xilink6.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\StaticDebug\vgl.lib"

!ELSEIF  "$(CFG)" == "vgl - Win32 StaticRelease"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "StaticRelease"
# PROP BASE Intermediate_Dir "StaticRelease"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "StaticRelease"
# PROP Intermediate_Dir "StaticRelease"
# PROP Target_Dir ""
MTL=midl.exe
# ADD BASE CPP /nologo /W3 /GX /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /Ob2 /I "$(IUEROOT)\vcl\config.win32-vc60" /I "$(IUEROOT)\vcl" /I "$(IUEROOT)\vxl" /D "WIN32" /D "NDEBUG" /D "_LIB" /YX /FD /c /Zl
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=xilink6.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\StaticRelease\vgl.lib"

!ENDIF 

# Begin Target

# Name "vgl - Win32 Release"
# Name "vgl - Win32 Debug"
# Name "vgl - Win32 StaticDebug"
# Name "vgl - Win32 StaticRelease"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\vgl_1d_basis.cxx
# End Source File
# Begin Source File

SOURCE=".\Templates\vcl_vector+vcl_vector+vgl_point_2d+float---.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vcl_vector+vgl_point_2d+double--.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vcl_vector+vgl_point_2d+float--.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vcl_vector+vgl_polygon-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vgl_box_2d+double-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vgl_box_2d+float-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vgl_box_2d+int-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vgl_box_3d+double-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vgl_box_3d+float-.cxx"
# End Source File
# Begin Source File

SOURCE=.\vgl_clip.cxx
# End Source File
# Begin Source File

SOURCE=.\vgl_distance.cxx
# End Source File
# Begin Source File

SOURCE=".\Templates\vgl_homg_line_2d+double-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vgl_homg_line_2d+float-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vgl_homg_line_3d_2_points+double-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vgl_homg_line_3d_2_points+float-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vgl_homg_plane_3d+double-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vgl_homg_plane_3d+float-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vgl_homg_point_2d+double-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vgl_homg_point_2d+float-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vgl_homg_point_2d+int-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vgl_homg_point_3d+double-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vgl_homg_point_3d+float-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vgl_line_2d+double-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vgl_line_2d+float-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vgl_line_segment_2d+double-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vgl_line_segment_2d+float-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vgl_line_segment_3d+double-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vgl_line_segment_3d+float-.cxx"
# End Source File
# Begin Source File

SOURCE=.\vgl_lineseg_test.cxx
# End Source File
# Begin Source File

SOURCE=".\Templates\vgl_plane_3d+double-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vgl_plane_3d+float-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vgl_point_2d+double-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vgl_point_2d+float-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vgl_point_2d+int-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vgl_point_3d+double-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vgl_point_3d+float-.cxx"
# End Source File
# Begin Source File

SOURCE=.\vgl_polygon.cxx
# End Source File
# Begin Source File

SOURCE=.\vgl_polygon_scan_iterator.cxx
# End Source File
# Begin Source File

SOURCE=.\vgl_polygon_test.cxx
# End Source File
# Begin Source File

SOURCE=.\vgl_region_scan_iterator.cxx
# End Source File
# Begin Source File

SOURCE=.\vgl_test.cxx
# End Source File
# Begin Source File

SOURCE=.\vgl_triangle_scan_iterator.cxx
# End Source File
# Begin Source File

SOURCE=.\vgl_triangle_test.cxx
# End Source File
# Begin Source File

SOURCE=.\vgl_window_scan_iterator.cxx
# End Source File
# Begin Source File

SOURCE=".\Templates\vgl_vector_3d+int-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vgl_vector_3d+float-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vgl_vector_3d+double-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vgl_vector_2d+int-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vgl_vector_2d+float-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vgl_vector_2d+double-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vgl_point_3d+int-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vgl_homg_point_3d+int-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vgl_homg_point_1d+int-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vgl_homg_point_1d+float-.cxx"
# End Source File
# Begin Source File
SOURCE=".\Templates\vgl_homg_point_1d+double-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vgl_homg_plane_3d+int-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vgl_homg_line_3d_2_points+int-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vgl_homg_line_2d+int-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vgl_box_3d+int-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vcl_vector+vgl_point_3d+int--.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vcl_vector+vgl_point_3d+float--.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vcl_vector+vgl_point_3d+double--.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vcl_vector+vgl_point_2d+int--.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vcl_vector+vgl_homg_point_3d+int--.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vcl_vector+vgl_homg_point_2d+int--.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vcl_vector+vgl_homg_point_1d+float--.cxx"
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\vgl_box_2d.h
# End Source File
# Begin Source File

SOURCE=.\vgl_box_3d.h
# End Source File
# Begin Source File

SOURCE=.\vgl_clip.h
# End Source File
# Begin Source File

SOURCE=.\vgl_distance.h
# End Source File
# Begin Source File

SOURCE=.\vgl_homg_line_2d.h
# End Source File
# Begin Source File

SOURCE=.\vgl_homg_line_3d_2_points.h
# End Source File
# Begin Source File

SOURCE=.\vgl_homg_plane_3d.h
# End Source File
# Begin Source File

SOURCE=.\vgl_homg_point_2d.h
# End Source File
# Begin Source File

SOURCE=.\vgl_homg_point_3d.h
# End Source File
# Begin Source File

SOURCE=.\vgl_line_2d.h
# End Source File
# Begin Source File

SOURCE=.\vgl_line_segment_3d.h
# End Source File
# Begin Source File

SOURCE=.\vgl_plane_3d.h
# End Source File
# Begin Source File

SOURCE=.\vgl_point_2d.h
# End Source File
# Begin Source File

SOURCE=.\vgl_point_3d.h
# End Source File
# Begin Source File

SOURCE=.\vgl_polygon.h
# End Source File
# Begin Source File

SOURCE=.\vgl_polygon_scan_iterator.h
# End Source File
# Begin Source File

SOURCE=.\vgl_test.h
# End Source File
# End Group
# End Target
# End Project
