# Microsoft Developer Studio Project File - Name="vgl" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# ITK DSP Header file
# This file is read by the build system of itk, and is used as the top part of
# a microsoft project dsp header file
# IF this is in a dsp file, then it is not the header, but has
# already been used, so do not edit here...

# variables to REPLACE
# 
# /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/vxl" /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/vcl/config.win32-VC60" /I "$(VXLROOT)/vxl"  == include path
#   -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE == compiler defines
#  == override in output directory
# vgl  == name of output library

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=vgl - Win32 Debug
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
!MESSAGE "vgl - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "vgl - Win32 Release" (based on "Win32 (x86) Static Library")
# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
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
# ADD BASE CPP /nologo /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB"  /FD /c
# ADD CPP /nologo /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB"  /FD /c
# ADD CPP /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/vxl" /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/vcl/config.win32-VC60" /I "$(VXLROOT)/vxl"    -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE /D "vgl_EXPORTS"
# ADD CPP /W3 /Zm1000 /GX /GR
# ADD CPP /MD /O2
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

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
# ADD BASE CPP /nologo /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB"  /FD /c
# ADD CPP /nologo /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "_LIB"  /FD /GZ /c
# ADD CPP /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/vxl" /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/vcl/config.win32-VC60" /I "$(VXLROOT)/vxl"    -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE /D "vgl_EXPORTS"
# ADD CPP /W3 /Zm1000 /GX /GR
# ADD CPP /MDd /Zi /Od /GZ
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
!ENDIF 

# Begin Target

# Name "vgl - Win32 Release"
# Name "vgl - Win32 Debug"

# Begin Group "Source Files"
# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Templates/vcl_vector+vcl_vector+vgl_point_2d+float---.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_vector+vgl_homg_point_1d+float--.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_vector+vgl_homg_point_2d+int--.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_vector+vgl_homg_point_3d+int--.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_vector+vgl_point_2d+double--.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_vector+vgl_point_2d+float--.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_vector+vgl_point_2d+int--.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_vector+vgl_point_3d+double--.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_vector+vgl_point_3d+float--.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_vector+vgl_point_3d+int--.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_vector+vgl_polygon-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vgl_box_2d+double-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vgl_box_2d+float-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vgl_box_2d+int-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vgl_box_3d+double-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vgl_box_3d+float-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vgl_box_3d+int-.cxx

# End Source File
# Begin Source File

SOURCE=.\vgl_homg.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vgl_homg_line_2d+double-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vgl_homg_line_2d+float-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vgl_homg_line_2d+int-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vgl_homg_line_3d_2_points+double-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vgl_homg_line_3d_2_points+float-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vgl_homg_line_3d_2_points+int-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vgl_homg_plane_3d+double-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vgl_homg_plane_3d+float-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vgl_homg_plane_3d+int-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vgl_homg_point_1d+double-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vgl_homg_point_1d+float-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vgl_homg_point_1d+int-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vgl_homg_point_2d+double-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vgl_homg_point_2d+float-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vgl_homg_point_2d+int-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vgl_homg_point_3d+double-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vgl_homg_point_3d+float-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vgl_homg_point_3d+int-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vgl_line_2d+double-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vgl_line_2d+float-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vgl_line_segment_2d+double-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vgl_line_segment_2d+float-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vgl_line_segment_3d+double-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vgl_line_segment_3d+float-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vgl_plane_3d+double-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vgl_plane_3d+float-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vgl_point_2d+double-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vgl_point_2d+float-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vgl_point_2d+int-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vgl_point_3d+double-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vgl_point_3d+float-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vgl_point_3d+int-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vgl_vector_2d+double-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vgl_vector_2d+float-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vgl_vector_2d+int-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vgl_vector_3d+double-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vgl_vector_3d+float-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vgl_vector_3d+int-.cxx

# End Source File
# Begin Source File

SOURCE=.\vgl_1d_basis.cxx

# End Source File
# Begin Source File

SOURCE=.\vgl_clip.cxx

# End Source File
# Begin Source File

SOURCE=.\vgl_distance.cxx

# End Source File
# Begin Source File

SOURCE=.\vgl_lineseg_test.cxx

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

SOURCE=.\internals/gpc.c

# End Source File
# Begin Source File

SOURCE=.\vgl_area.cxx

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

SOURCE=.\Templates/vgl_conic+float-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vgl_conic+double-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_list+vgl_homg_point_2d+float--.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_list+vgl_homg_point_2d+double--.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_list+vgl_homg_line_2d+float--.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_list+vgl_homg_line_2d+double--.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_list+vgl_point_2d+double--.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vgl_homg_1d+double-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vgl_homg_1d+longdouble-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vgl_homg_1d+float-.cxx

# End Source File
# Begin Source File

SOURCE=.\vgl_ellipse_scan_iterator.cxx

# End Source File
# End Group
# Begin Group "Header Files"
# PROP Default_Filter ""
# Begin Source File

SOURCE=.\vgl_1d_basis.h

# End Source File
# Begin Source File

SOURCE=.\vgl_1d_basis.txx

# End Source File
# Begin Source File

SOURCE=.\vgl_box_2d.h

# End Source File
# Begin Source File

SOURCE=.\vgl_box_2d.txx

# End Source File
# Begin Source File

SOURCE=.\vgl_box_3d.h

# End Source File
# Begin Source File

SOURCE=.\vgl_box_3d.txx

# End Source File
# Begin Source File

SOURCE=.\vgl_clip.h

# End Source File
# Begin Source File

SOURCE=.\vgl_conic.h

# End Source File
# Begin Source File

SOURCE=.\vgl_conic.txx

# End Source File
# Begin Source File

SOURCE=.\vgl_distance.h

# End Source File
# Begin Source File

SOURCE=.\vgl_ellipse_scan_iterator.h

# End Source File
# Begin Source File

SOURCE=.\vgl_homg.h

# End Source File
# Begin Source File

SOURCE=.\vgl_homg_line_2d.h

# End Source File
# Begin Source File

SOURCE=.\vgl_homg_line_2d.txx

# End Source File
# Begin Source File

SOURCE=.\vgl_homg_line_3d_2_points.h

# End Source File
# Begin Source File

SOURCE=.\vgl_homg_line_3d_2_points.txx

# End Source File
# Begin Source File

SOURCE=.\vgl_homg_plane_3d.h

# End Source File
# Begin Source File

SOURCE=.\vgl_homg_plane_3d.txx

# End Source File
# Begin Source File

SOURCE=.\vgl_homg_point_1d.h

# End Source File
# Begin Source File

SOURCE=.\vgl_homg_point_1d.txx

# End Source File
# Begin Source File

SOURCE=.\vgl_homg_point_2d.h

# End Source File
# Begin Source File

SOURCE=.\vgl_homg_point_2d.txx

# End Source File
# Begin Source File

SOURCE=.\vgl_homg_point_3d.h

# End Source File
# Begin Source File

SOURCE=.\vgl_homg_point_3d.txx

# End Source File
# Begin Source File

SOURCE=.\vgl_line_2d.h

# End Source File
# Begin Source File

SOURCE=.\vgl_line_2d.txx

# End Source File
# Begin Source File

SOURCE=.\vgl_line_segment_2d.h

# End Source File
# Begin Source File

SOURCE=.\vgl_line_segment_2d.txx

# End Source File
# Begin Source File

SOURCE=.\vgl_line_segment_3d.h

# End Source File
# Begin Source File

SOURCE=.\vgl_line_segment_3d.txx

# End Source File
# Begin Source File

SOURCE=.\vgl_lineseg_test.h

# End Source File
# Begin Source File

SOURCE=.\vgl_plane_3d.h

# End Source File
# Begin Source File

SOURCE=.\vgl_plane_3d.txx

# End Source File
# Begin Source File

SOURCE=.\vgl_point_2d.h

# End Source File
# Begin Source File

SOURCE=.\vgl_point_2d.txx

# End Source File
# Begin Source File

SOURCE=.\vgl_point_3d.h

# End Source File
# Begin Source File

SOURCE=.\vgl_point_3d.txx

# End Source File
# Begin Source File

SOURCE=.\vgl_polygon.h

# End Source File
# Begin Source File

SOURCE=.\vgl_polygon_scan_iterator.h

# End Source File
# Begin Source File

SOURCE=.\vgl_polygon_test.h

# End Source File
# Begin Source File

SOURCE=.\vgl_region_scan_iterator.h

# End Source File
# Begin Source File

SOURCE=.\internals/gpc.h

# End Source File
# Begin Source File

SOURCE=.\vgl_area.h

# End Source File
# Begin Source File

SOURCE=.\vgl_triangle_scan_iterator.h

# End Source File
# Begin Source File

SOURCE=.\vgl_triangle_test.h

# End Source File
# Begin Source File

SOURCE=.\vgl_vector_2d.h

# End Source File
# Begin Source File

SOURCE=.\vgl_vector_2d.txx

# End Source File
# Begin Source File

SOURCE=.\vgl_vector_3d.h

# End Source File
# Begin Source File

SOURCE=.\vgl_vector_3d.txx

# End Source File
# Begin Source File

SOURCE=.\vgl_window_scan_iterator.h

# End Source File
# Begin Source File

SOURCE=.\vgl_fwd.h

# End Source File
# Begin Source File

SOURCE=.\vgl_homg_1d.h

# End Source File
# Begin Source File

SOURCE=.\vgl_homg_1d.txx

# End Source File
# End Group
# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project

