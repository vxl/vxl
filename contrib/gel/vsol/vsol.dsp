# Microsoft Developer Studio Project File - Name="vsol" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# ITK DSP Header file
# This file is read by the build system of itk, and is used as the top part of
# a microsoft project dsp header file
# IF this is in a dsp file, then it is not the header, but has
# already been used, so do not edit here...

# variables to REPLACE
# 
# /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/core" /I "$(VXLROOT)/vcl/config.win32" /I "$(VXLROOT)/contrib/gel" == include path
#   -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE -DHAS_OPENGL == compiler defines
#  == override in output directory
# vsol  == name of output library

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=vsol - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "vsol.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "vsol.mak" CFG="vsol - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "vsol - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "vsol - Win32 Release" (based on "Win32 (x86) Static Library")
# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "vsol - Win32 Release"

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
# ADD BASE CPP /nologo /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /FD /c
# ADD CPP /nologo /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /FD /c
# ADD CPP /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/core" /I "$(VXLROOT)/vcl/config.win32" /I "$(VXLROOT)/contrib/gel" -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE -DHAS_OPENGL /D "vsol_EXPORTS"
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

!ELSEIF  "$(CFG)" == "vsol - Win32 Debug"

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
# ADD BASE CPP /nologo /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /FD /c
# ADD CPP /nologo /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /FD /GZ /c
# ADD CPP /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/core" /I "$(VXLROOT)/vcl/config.win32" /I "$(VXLROOT)/contrib/gel" -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE -DHAS_OPENGL /D "vsol_EXPORTS"
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

# Name "vsol - Win32 Release"
# Name "vsol - Win32 Debug"

# Begin Group "Source Files"
# PROP Default_Filter ""
# Begin Source File
SOURCE=.\Templates/vbl_bounding_box+double.3-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vbl_smart_ptr+vsol_box_2d-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vbl_smart_ptr+vsol_box_3d-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vbl_smart_ptr+vsol_conic_2d-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vbl_smart_ptr+vsol_curve_2d-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vbl_smart_ptr+vsol_curve_3d-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vbl_smart_ptr+vsol_group_2d-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vbl_smart_ptr+vsol_group_3d-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vbl_smart_ptr+vsol_line_2d-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vbl_smart_ptr+vsol_line_3d-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vbl_smart_ptr+vsol_point_2d-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vbl_smart_ptr+vsol_point_3d-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vbl_smart_ptr+vsol_polygon_2d-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vbl_smart_ptr+vsol_polygon_3d-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vbl_smart_ptr+vsol_rectangle_2d-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vbl_smart_ptr+vsol_rectangle_3d-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vbl_smart_ptr+vsol_region_2d-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vbl_smart_ptr+vsol_region_3d-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vbl_smart_ptr+vsol_spatial_object_2d-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vbl_smart_ptr+vsol_spatial_object_3d-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vbl_smart_ptr+vsol_surface_3d-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vbl_smart_ptr+vsol_tetrahedron-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vbl_smart_ptr+vsol_triangle_2d-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vbl_smart_ptr+vsol_triangle_3d-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vcl_list+vsol_point_2d_sptr-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vcl_list+vsol_spatial_object_2d_sptr-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vcl_list+vsol_spatial_object_3d_sptr-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vcl_vector+vsol_point_2d_sptr-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vcl_vector+vsol_point_3d_sptr-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vcl_vector+vsol_spatial_object_2d~-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vcl_vector+vsol_spatial_object_3d~-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vcl_deque+vcl_vector+vsol_spatial_object_2d_sptr--.cxx
# End Source File
# Begin Source File
SOURCE=.\vsol_box_2d.cxx
# End Source File
# Begin Source File
SOURCE=.\vsol_box_3d.cxx
# End Source File
# Begin Source File
SOURCE=.\vsol_conic_2d.cxx
# End Source File
# Begin Source File
SOURCE=.\vsol_curve_2d.cxx
# End Source File
# Begin Source File
SOURCE=.\vsol_curve_3d.cxx
# End Source File
# Begin Source File
SOURCE=.\vsol_group_2d.cxx
# End Source File
# Begin Source File
SOURCE=.\vsol_group_3d.cxx
# End Source File
# Begin Source File
SOURCE=.\vsol_line_2d.cxx
# End Source File
# Begin Source File
SOURCE=.\vsol_line_3d.cxx
# End Source File
# Begin Source File
SOURCE=.\vsol_point_2d.cxx
# End Source File
# Begin Source File
SOURCE=.\vsol_point_3d.cxx
# End Source File
# Begin Source File
SOURCE=.\vsol_polygon_2d.cxx
# End Source File
# Begin Source File
SOURCE=.\vsol_polygon_3d.cxx
# End Source File
# Begin Source File
SOURCE=.\vsol_polyhedron.cxx
# End Source File
# Begin Source File
SOURCE=.\vsol_rectangle_2d.cxx
# End Source File
# Begin Source File
SOURCE=.\vsol_rectangle_3d.cxx
# End Source File
# Begin Source File
SOURCE=.\vsol_region_2d.cxx
# End Source File
# Begin Source File
SOURCE=.\vsol_region_3d.cxx
# End Source File
# Begin Source File
SOURCE=.\vsol_spatial_object_2d.cxx
# End Source File
# Begin Source File
SOURCE=.\vsol_spatial_object_3d.cxx
# End Source File
# Begin Source File
SOURCE=.\vsol_surface_3d.cxx
# End Source File
# Begin Source File
SOURCE=.\vsol_tetrahedron.cxx
# End Source File
# Begin Source File
SOURCE=.\vsol_triangle_2d.cxx
# End Source File
# Begin Source File
SOURCE=.\vsol_triangle_3d.cxx
# End Source File
# Begin Source File
SOURCE=.\vsol_volume_3d.cxx
# End Source File
# End Group
# Begin Group "Header Files"
# PROP Default_Filter ""
# Begin Source File
SOURCE=.\vsol_box_2d.h
# End Source File
# Begin Source File
SOURCE=.\vsol_box_3d.h
# End Source File
# Begin Source File
SOURCE=.\vsol_conic_2d.h
# End Source File
# Begin Source File
SOURCE=.\vsol_curve_2d.h
# End Source File
# Begin Source File
SOURCE=.\vsol_curve_3d.h
# End Source File
# Begin Source File
SOURCE=.\vsol_group_2d.h
# End Source File
# Begin Source File
SOURCE=.\vsol_group_3d.h
# End Source File
# Begin Source File
SOURCE=.\vsol_line_2d.h
# End Source File
# Begin Source File
SOURCE=.\vsol_line_3d.h
# End Source File
# Begin Source File
SOURCE=.\vsol_point_2d.h
# End Source File
# Begin Source File
SOURCE=.\vsol_point_3d.h
# End Source File
# Begin Source File
SOURCE=.\vsol_polygon_2d.h
# End Source File
# Begin Source File
SOURCE=.\vsol_polygon_3d.h
# End Source File
# Begin Source File
SOURCE=.\vsol_polyhedron.h
# End Source File
# Begin Source File
SOURCE=.\vsol_rectangle_2d.h
# End Source File
# Begin Source File
SOURCE=.\vsol_rectangle_3d.h
# End Source File
# Begin Source File
SOURCE=.\vsol_region_2d.h
# End Source File
# Begin Source File
SOURCE=.\vsol_region_3d.h
# End Source File
# Begin Source File
SOURCE=.\vsol_spatial_object_2d.h
# End Source File
# Begin Source File
SOURCE=.\vsol_spatial_object_3d.h
# End Source File
# Begin Source File
SOURCE=.\vsol_surface_3d.h
# End Source File
# Begin Source File
SOURCE=.\vsol_tetrahedron.h
# End Source File
# Begin Source File
SOURCE=.\vsol_triangle_2d.h
# End Source File
# Begin Source File
SOURCE=.\vsol_triangle_3d.h
# End Source File
# Begin Source File
SOURCE=.\vsol_volume_3d.h
# End Source File
# Begin Source File
SOURCE=.\vsol_box_2d_sptr.h
# End Source File
# Begin Source File
SOURCE=.\vsol_box_3d_sptr.h
# End Source File
# Begin Source File
SOURCE=.\vsol_conic_2d_sptr.h
# End Source File
# Begin Source File
SOURCE=.\vsol_curve_2d_sptr.h
# End Source File
# Begin Source File
SOURCE=.\vsol_curve_3d_sptr.h
# End Source File
# Begin Source File
SOURCE=.\vsol_group_2d_sptr.h
# End Source File
# Begin Source File
SOURCE=.\vsol_group_3d_sptr.h
# End Source File
# Begin Source File
SOURCE=.\vsol_line_2d_sptr.h
# End Source File
# Begin Source File
SOURCE=.\vsol_line_3d_sptr.h
# End Source File
# Begin Source File
SOURCE=.\vsol_point_2d_sptr.h
# End Source File
# Begin Source File
SOURCE=.\vsol_point_3d_sptr.h
# End Source File
# Begin Source File
SOURCE=.\vsol_polygon_2d_sptr.h
# End Source File
# Begin Source File
SOURCE=.\vsol_polygon_3d_sptr.h
# End Source File
# Begin Source File
SOURCE=.\vsol_polyhedron_sptr.h
# End Source File
# Begin Source File
SOURCE=.\vsol_rectangle_2d_sptr.h
# End Source File
# Begin Source File
SOURCE=.\vsol_rectangle_3d_sptr.h
# End Source File
# Begin Source File
SOURCE=.\vsol_region_2d_sptr.h
# End Source File
# Begin Source File
SOURCE=.\vsol_region_3d_sptr.h
# End Source File
# Begin Source File
SOURCE=.\vsol_spatial_object_2d_sptr.h
# End Source File
# Begin Source File
SOURCE=.\vsol_spatial_object_3d_sptr.h
# End Source File
# Begin Source File
SOURCE=.\vsol_surface_3d_sptr.h
# End Source File
# Begin Source File
SOURCE=.\vsol_tetrahedron_sptr.h
# End Source File
# Begin Source File
SOURCE=.\vsol_triangle_2d_sptr.h
# End Source File
# Begin Source File
SOURCE=.\vsol_triangle_3d_sptr.h
# End Source File
# Begin Source File
SOURCE=.\vsol_volume_3d_sptr.h
# End Source File
# End Group
# End Target
# End Project
