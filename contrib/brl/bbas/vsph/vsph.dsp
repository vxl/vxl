# Microsoft Developer Studio Project File - Name="vsph" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# variables to REPLACE
# 
# /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/core" /I "$(VXLROOT)/vcl/config.win32" /I "$(VXLROOT)/contrib/mul" == include path
#   -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE == compiler defines
#  == override in output directory
# vsph  == name of output library

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=vsph - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "vsph.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "vsph.mak" CFG="vil2 - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "vsph - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "vsph - Win32 Release" (based on "Win32 (x86) Static Library")
# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "vsph - Win32 Release"

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
# ADD CPP /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/core" /I "$(VXLROOT)/vcl/config.win32" /I "$(VXLROOT)/contrib/mul" -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE /D "vsph_EXPORTS"
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

!ELSEIF  "$(CFG)" == "vsph - Win32 Debug"

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
# ADD CPP /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/core" /I "$(VXLROOT)/vcl/config.win32" /I "$(VXLROOT)/contrib/mul" -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE /D "vsph_EXPORTS"
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

# Name "vsph - Win32 Release"
# Name "vsph - Win32 Debug"

# Begin Group "Source Files"
# PROP Default_Filter "cxx"
# Begin Source File
SOURCE=.\vsph_camera_bounds.cxx
# End Source File
# Begin Source File
SOURCE=.\vsph_spherical_coord.cxx
# End Source File
# Begin Source File
SOURCE=.\vsph_sph_point_3d.cxx
# End Source File
# Begin Source File
SOURCE=.\vsph_sph_point_2d.cxx
# End Source File
# Begin Source File
SOURCE=.\vsph_sph_box_2d.cxx
# End Source File
# Begin Source File
SOURCE=.\vsph_utils.cxx
# End Source File
# Begin Source File
SOURCE=.\vsph_unit_sphere.cxx
# End Source File
# Begin Source File
SOURCE=.\vsph_grid_index_2d.cxx
# End Source File
# Begin Source File
SOURCE=.\vsph_segment_sphere.cxx
# End Source File
# Begin Source File
SOURCE=.\vsph_sph_cover_2d.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vsph_view_sphere+vsph_view_point+double--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vsph_view_sphere+vsph_view_point+vcl_string--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vbl_smart_ptr+vsph_spherical_coord-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vbl_smart_ptr+vsph_unit_sphere-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vsl_vector_io+vsph_edge-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vsl_vector_io+vsph_sph_point_2d-.cxx
# End Source File
# End Group
# Begin Group "Header Files"
# PROP Default_Filter "h;txx"
# Begin Source File
SOURCE=.\vsph_camera_bounds.h
# End Source File
# Begin Source File
SOURCE=.\vsph_view_sphere.h
# End Source File
# Begin Source File
SOURCE=.\vsph_sph_point_3d.h
# End Source File
# Begin Source File
SOURCE=.\vsph_spherical_coord_sptr.h
# End Source File
# Begin Source File
SOURCE=.\vsph_view_sphere.txx
# End Source File
# Begin Source File
SOURCE=.\vsph_spherical_coord.h
# End Source File
# Begin Source File
SOURCE=.\vsph_view_point.h
# End Source File
# Begin Source File
SOURCE=.\vsph_utils.h
# End Source File
# Begin Source File
SOURCE=.\vsph_sph_point_2d.h
# End Source File
# Begin Source File
SOURCE=.\vsph_sph_box_2d.h
# End Source File
# Begin Source File
SOURCE=.\vsph_unit_sphere.h
# End Source File
# Begin Source File
SOURCE=.\vsph_unit_sphere_sptr.h
# End Source File
# Begin Source File
SOURCE=.\vsph_grid_index_2d.h
# End Source File
# Begin Source File
SOURCE=.\vsph_defs.h
# End Source File
# Begin Source File
SOURCE=.\vsph_segment_sphere.h
# End Source File
# Begin Source File
SOURCE=.\vsph_sph_cover_2d.h
# End Source File
# End Group
# End Target
# End Project
