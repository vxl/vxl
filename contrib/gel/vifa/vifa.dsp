# Microsoft Developer Studio Project File - Name="vifa" - Package Owner=<4>
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
# vifa  == name of output library

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=vifa - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "vifa.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "vifa.mak" CFG="vifa - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "vifa - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "vifa - Win32 Release" (based on "Win32 (x86) Static Library")
# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "vifa - Win32 Release"

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
# ADD CPP /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/core" /I "$(VXLROOT)/vcl/config.win32" /I "$(VXLROOT)/contrib/gel" -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE -DHAS_OPENGL /D "vifa_EXPORTS"
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

!ELSEIF  "$(CFG)" == "vifa - Win32 Debug"

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
# ADD CPP /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/core" /I "$(VXLROOT)/vcl/config.win32" /I "$(VXLROOT)/contrib/gel" -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE -DHAS_OPENGL /D "vifa_EXPORTS"
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

# Name "vifa - Win32 Release"
# Name "vifa - Win32 Debug"

# Begin Group "Source Files"
# PROP Default_Filter "c;cxx"
# Begin Source File
SOURCE=.\Templates/vbl_smart_ptr+vifa_imp_line+double--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vbl_smart_ptr+vifa_bbox-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vbl_smart_ptr+vifa_coll_lines-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vbl_smart_ptr+vifa_coll_lines_params-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vbl_smart_ptr+vifa_group_pgram_params-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vbl_smart_ptr+vifa_histogram-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vbl_smart_ptr+vifa_incr_var-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vbl_smart_ptr+vifa_int_face_attr-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vbl_smart_ptr+vifa_int_faces_attr-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vbl_smart_ptr+vifa_line_cover-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vbl_smart_ptr+vifa_norm_params-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vifa_imp_line+double-.cxx
# End Source File
# Begin Source File
SOURCE=.\vifa_coll_lines.cxx
# End Source File
# Begin Source File
SOURCE=.\vifa_coll_lines_params.cxx
# End Source File
# Begin Source File
SOURCE=.\vifa_gaussian.cxx
# End Source File
# Begin Source File
SOURCE=.\vifa_group_pgram.cxx
# End Source File
# Begin Source File
SOURCE=.\vifa_group_pgram_params.cxx
# End Source File
# Begin Source File
SOURCE=.\vifa_histogram.cxx
# End Source File
# Begin Source File
SOURCE=.\vifa_image_histogram.cxx
# End Source File
# Begin Source File
SOURCE=.\vifa_incr_var.cxx
# End Source File
# Begin Source File
SOURCE=.\vifa_int_face_attr.cxx
# End Source File
# Begin Source File
SOURCE=.\vifa_int_face_attr_common.cxx
# End Source File
# Begin Source File
SOURCE=.\vifa_int_face_attr_common_params.cxx
# End Source File
# Begin Source File
SOURCE=.\vifa_int_faces_adj_attr.cxx
# End Source File
# Begin Source File
SOURCE=.\vifa_int_faces_attr.cxx
# End Source File
# Begin Source File
SOURCE=.\vifa_line_cover.cxx
# End Source File
# Begin Source File
SOURCE=.\vifa_norm_params.cxx
# End Source File
# Begin Source File
SOURCE=.\vifa_parallel.cxx
# End Source File
# Begin Source File
SOURCE=.\vifa_parallel_params.cxx
# End Source File
# End Group
# Begin Group "Header Files"
# PROP Default_Filter "h;txx"
# Begin Source File
SOURCE=.\vifa_bbox.h
# End Source File
# Begin Source File
SOURCE=.\vifa_coll_lines.h
# End Source File
# Begin Source File
SOURCE=.\vifa_coll_lines_params.h
# End Source File
# Begin Source File
SOURCE=.\vifa_gaussian.h
# End Source File
# Begin Source File
SOURCE=.\vifa_group_pgram.h
# End Source File
# Begin Source File
SOURCE=.\vifa_group_pgram_params.h
# End Source File
# Begin Source File
SOURCE=.\vifa_histogram.h
# End Source File
# Begin Source File
SOURCE=.\vifa_image_histogram.h
# End Source File
# Begin Source File
SOURCE=.\vifa_imp_line.h
# End Source File
# Begin Source File
SOURCE=.\vifa_imp_line.txx
# End Source File
# Begin Source File
SOURCE=.\vifa_incr_var.h
# End Source File
# Begin Source File
SOURCE=.\vifa_int_face_attr.h
# End Source File
# Begin Source File
SOURCE=.\vifa_int_face_attr_common.h
# End Source File
# Begin Source File
SOURCE=.\vifa_int_face_attr_common_params.h
# End Source File
# Begin Source File
SOURCE=.\vifa_int_face_attr_factory.h
# End Source File
# Begin Source File
SOURCE=.\vifa_int_faces_adj_attr.h
# End Source File
# Begin Source File
SOURCE=.\vifa_int_faces_attr.h
# End Source File
# Begin Source File
SOURCE=.\vifa_line_cover.h
# End Source File
# Begin Source File
SOURCE=.\vifa_norm_params.h
# End Source File
# Begin Source File
SOURCE=.\vifa_parallel.h
# End Source File
# Begin Source File
SOURCE=.\vifa_parallel_params.h
# End Source File
# Begin Source File
SOURCE=.\vifa_typedefs.h
# End Source File
# End Group
# End Target
# End Project
