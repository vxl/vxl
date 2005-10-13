# Microsoft Developer Studio Project File - Name="vil3d" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# ITK DSP Header file
# This file is read by the build system of itk, and is used as the top part of
# a microsoft project dsp header file
# IF this is in a dsp file, then it is not the header, but has
# already been used, so do not edit here...

# variables to REPLACE
# 
# /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/core" /I "$(VXLROOT)/vcl/config.win32" /I "$(VXLROOT)/contrib/mul" == include path
#   -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE == compiler defines
#  == override in output directory
# vil3d  == name of output library

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=vil3d - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "vil3d.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "vil3d.mak" CFG="vil3d - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "vil3d - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "vil3d - Win32 Release" (based on "Win32 (x86) Static Library")
# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "vil3d - Win32 Release"

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
# ADD CPP /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/core" /I "$(VXLROOT)/vcl/config.win32" /I "$(VXLROOT)/contrib/mul" -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE /D "vil3d_EXPORTS"
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

!ELSEIF  "$(CFG)" == "vil3d - Win32 Debug"

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
# ADD CPP /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/core" /I "$(VXLROOT)/vcl/config.win32" /I "$(VXLROOT)/contrib/mul" -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE /D "vil3d_EXPORTS"
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

# Name "vil3d - Win32 Release"
# Name "vil3d - Win32 Debug"

# Begin Group "Source Files"
# PROP Default_Filter "c;cxx"
# Begin Source File
SOURCE=.\Templates/vil3d_image_view+byte-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil3d_image_view+double-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil3d_image_view+float-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil3d_image_view+int_16-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil3d_image_view+int_32-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil3d_image_view+uint_16-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil3d_image_view+uint_32-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil3d_sample_profile_trilin+byte.double-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil3d_sample_profile_trilin+float.double-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil3d_image_view+bool-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil_smart_ptr+vil3d_image_view_base-.cxx
# End Source File
# Begin Source File
SOURCE=.\file_formats/vil3d_gipl_format.cxx
# End Source File
# Begin Source File
SOURCE=.\vil3d_file_format.cxx
# End Source File
# Begin Source File
SOURCE=.\vil3d_load.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil_smart_ptr+vil3d_image_resource-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil3d_copy+bool-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil3d_copy+byte-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil3d_copy+double-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil3d_copy+float-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil3d_copy+int_16-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil3d_copy+int_32-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil3d_copy+sbyte-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil3d_copy+uint_16-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil3d_copy+uint_32-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil3d_image_view+sbyte-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil3d_resample_simple+int-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil3d_resample_trilinear+int-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil3d_resample_simple+bool-.cxx
# End Source File
# Begin Source File
SOURCE=.\vil3d_memory_image.cxx
# End Source File
# Begin Source File
SOURCE=.\vil3d_new.cxx
# End Source File
# Begin Source File
SOURCE=.\file_formats/vil3d_dicom.cxx
# End Source File
# Begin Source File
SOURCE=.\file_formats/vil3d_slice_list.cxx
# End Source File
# Begin Source File
SOURCE=.\file_formats/vil3d_analyze_format.cxx
# End Source File
# Begin Source File
SOURCE=.\file_formats/vil3d_gen_synthetic.cxx
# End Source File
# Begin Source File
SOURCE=.\vil3d_copy.cxx
# End Source File
# Begin Source File
SOURCE=.\vil3d_save.cxx
# End Source File
# End Group
# Begin Group "Header Files"
# PROP Default_Filter "h;txx"
# Begin Source File
SOURCE=.\vil3d_image_view.h
# End Source File
# Begin Source File
SOURCE=.\vil3d_image_view.txx
# End Source File
# Begin Source File
SOURCE=.\vil3d_image_view_base.h
# End Source File
# Begin Source File
SOURCE=.\vil3d_plane.h
# End Source File
# Begin Source File
SOURCE=.\vil3d_print.h
# End Source File
# Begin Source File
SOURCE=.\vil3d_sample_profile_trilin.h
# End Source File
# Begin Source File
SOURCE=.\vil3d_sample_profile_trilin.txx
# End Source File
# Begin Source File
SOURCE=.\vil3d_slice.h
# End Source File
# Begin Source File
SOURCE=.\vil3d_trilin_interp.h
# End Source File
# Begin Source File
SOURCE=.\vil3d_crop.h
# End Source File
# Begin Source File
SOURCE=.\vil3d_switch_axes.h
# End Source File
# Begin Source File
SOURCE=.\vil3d_math.h
# End Source File
# Begin Source File
SOURCE=.\file_formats/vil3d_gipl_format.h
# End Source File
# Begin Source File
SOURCE=.\vil3d_file_format.h
# End Source File
# Begin Source File
SOURCE=.\vil3d_load.h
# End Source File
# Begin Source File
SOURCE=.\vil3d_transform.h
# End Source File
# Begin Source File
SOURCE=.\vil3d_convert.h
# End Source File
# Begin Source File
SOURCE=.\vil3d_copy.h
# End Source File
# Begin Source File
SOURCE=.\vil3d_copy.txx
# End Source File
# Begin Source File
SOURCE=.\vil3d_image_resource.h
# End Source File
# Begin Source File
SOURCE=.\vil3d_memory_image.h
# End Source File
# Begin Source File
SOURCE=.\vil3d_new.h
# End Source File
# Begin Source File
SOURCE=.\file_formats/vil3d_dicom.h
# End Source File
# Begin Source File
SOURCE=.\file_formats/vil3d_slice_list.h
# End Source File
# Begin Source File
SOURCE=.\file_formats/vil3d_analyze_format.h
# End Source File
# Begin Source File
SOURCE=.\file_formats/vil3d_gen_synthetic.h
# End Source File
# Begin Source File
SOURCE=.\vil3d_property.h
# End Source File
# Begin Source File
SOURCE=.\vil3d_fwd.h
# End Source File
# Begin Source File
SOURCE=.\vil3d_save.h
# End Source File
# Begin Source File
SOURCE=.\vil3d_reflect.h
# End Source File
# Begin Source File
SOURCE=.\vil3d_from_image_2d.h
# End Source File
# Begin Source File
SOURCE=.\vil3d_resample_simple.h
# End Source File
# Begin Source File
SOURCE=.\vil3d_resample_simple.txx
# End Source File
# Begin Source File
SOURCE=.\vil3d_resample_trilinear.h
# End Source File
# Begin Source File
SOURCE=.\vil3d_resample_trilinear.txx
# End Source File
# Begin Source File
SOURCE=.\vil3d_decimate.h
# End Source File
# End Group
# End Target
# End Project
