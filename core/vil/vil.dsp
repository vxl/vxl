# Microsoft Developer Studio Project File - Name="vil2" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# ITK DSP Header file
# This file is read by the build system of itk, and is used as the top part of
# a microsoft project dsp header file
# IF this is in a dsp file, then it is not the header, but has
# already been used, so do not edit here...

# variables to REPLACE
# 
# /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/vxl" /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/vcl/config.win32-VC60" /I "$(VXLROOT)/vxl" /I "$(VXLROOT)/mul"  == include path
#   -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE == compiler defines
#  == override in output directory
# vil2  == name of output library

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=vil2 - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "vil2.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "vil2.mak" CFG="vil2 - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "vil2 - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "vil2 - Win32 Release" (based on "Win32 (x86) Static Library")
# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "vil2 - Win32 Release"

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
# ADD CPP /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/vxl" /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/vcl/config.win32-VC60" /I "$(VXLROOT)/vxl" /I "$(VXLROOT)/mul"    -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE /D "vil2_EXPORTS"
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

!ELSEIF  "$(CFG)" == "vil2 - Win32 Debug"

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
# ADD CPP /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/vxl" /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/vcl/config.win32-VC60" /I "$(VXLROOT)/vxl" /I "$(VXLROOT)/mul"    -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE /D "vil2_EXPORTS"
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

# Name "vil2 - Win32 Release"
# Name "vil2 - Win32 Debug"

# Begin Group "Source Files"
# PROP Default_Filter ""
# Begin Source File
SOURCE=.\file_formats/vil2_pnm.cxx
# End Source File
# Begin Source File
SOURCE=.\vil2_image_resource.cxx
# End Source File
# Begin Source File
SOURCE=.\vil2_memory_chunk.cxx
# End Source File
# Begin Source File
SOURCE=.\vil2_file_format.cxx
# End Source File
# Begin Source File
SOURCE=.\file_formats/vil2_png.cxx
# End Source File
# Begin Source File
SOURCE=.\file_formats/vil2_tiff.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil2_image_view+bool-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil2_smart_ptr+vil2_image_resource-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil2_smart_ptr+vil2_memory_chunk-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil2_image_view+float-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil2_image_view+vil_rgb+float--.cxx
# End Source File
# Begin Source File
SOURCE=.\vil2_crop.cxx
# End Source File
# Begin Source File
SOURCE=.\vil2_image_view_base.cxx
# End Source File
# Begin Source File
SOURCE=.\vil2_load.cxx
# End Source File
# Begin Source File
SOURCE=.\vil2_decimate.cxx
# End Source File
# Begin Source File
SOURCE=.\vil2_vil1.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil2_sample_profile_bilin+float.double-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil2_sample_grid_bilin+float.double-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil2_smart_ptr+vil2_image_view_base-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil2_image_view+byte-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil2_image_view+int_16-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil2_image_view+int_32-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil2_image_view+uint_32-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil2_image_view+vil_rgb+int_16--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil2_image_view+vil_rgb+int_32--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil2_image_view+vil_rgb+uint_32--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil2_sample_grid_bilin+byte.double-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil2_sample_profile_bilin+byte.double-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil2_image_view+uint_16-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil2_image_view+vil_rgb+byte--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil2_image_view+vil_rgb+uint_16--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil2_image_view+vil_rgb+double--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil2_image_view+vil_rgba+byte--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil2_image_view+double-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil2_copy+bool-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil2_copy+byte-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil2_copy+double-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil2_copy+float-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil2_copy+int_16-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil2_copy+int_32-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil2_copy+sbyte-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil2_copy+uint_16-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil2_copy+uint_32-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil2_image_view+sbyte-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil2_image_view+vil_rgb+sbyte--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil2_resample_bilin+byte.double-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil2_resample_bilin+byte.float-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil2_resample_bilin+float.double-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil2_sample_grid_bilin+float.float-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil2_image_view+vil_rgba+float--.cxx
# End Source File
# Begin Source File
SOURCE=.\file_formats/vil2_bmp.cxx
# End Source File
# Begin Source File
SOURCE=.\file_formats/vil2_bmp_core_header.cxx
# End Source File
# Begin Source File
SOURCE=.\file_formats/vil2_bmp_file_header.cxx
# End Source File
# Begin Source File
SOURCE=.\file_formats/vil2_bmp_info_header.cxx
# End Source File
# Begin Source File
SOURCE=.\vil2_memory_image.cxx
# End Source File
# Begin Source File
SOURCE=.\vil2_new.cxx
# End Source File
# Begin Source File
SOURCE=.\vil2_pixel_format.cxx
# End Source File
# Begin Source File
SOURCE=.\vil2_save.cxx
# End Source File
# Begin Source File
SOURCE=.\vil2_clamp.cxx
# End Source File
# Begin Source File
SOURCE=.\vil2_transpose.cxx
# End Source File
# Begin Source File
SOURCE=.\vil2_flip.cxx
# End Source File
# Begin Source File
SOURCE=.\vil2_plane.cxx
# End Source File
# Begin Source File
SOURCE=.\vil2_print.cxx
# End Source File
# Begin Source File
SOURCE=.\file_formats/vil2_jpeg.cxx
# End Source File
# Begin Source File
SOURCE=.\file_formats/vil2_jpeg_compressor.cxx
# End Source File
# Begin Source File
SOURCE=.\file_formats/vil2_jpeg_decompressor.cxx
# End Source File
# Begin Source File
SOURCE=.\file_formats/vil2_jpeg_destination_mgr.cxx
# End Source File
# Begin Source File
SOURCE=.\file_formats/vil2_jpeg_source_mgr.cxx
# End Source File
# End Group
# Begin Group "Header Files"
# PROP Default_Filter ""
# Begin Source File
SOURCE=.\file_formats/vil2_pnm.h
# End Source File
# Begin Source File
SOURCE=.\vil2_image_resource.h
# End Source File
# Begin Source File
SOURCE=.\vil2_image_view.h
# End Source File
# Begin Source File
SOURCE=.\vil2_image_view_base.h
# End Source File
# Begin Source File
SOURCE=.\vil2_smart_ptr.h
# End Source File
# Begin Source File
SOURCE=.\vil2_smart_ptr.txx
# End Source File
# Begin Source File
SOURCE=.\vil2_image_view.txx
# End Source File
# Begin Source File
SOURCE=.\vil2_memory_chunk.h
# End Source File
# Begin Source File
SOURCE=.\vil2_file_format.h
# End Source File
# Begin Source File
SOURCE=.\vil2_load.h
# End Source File
# Begin Source File
SOURCE=.\vil2_crop.h
# End Source File
# Begin Source File
SOURCE=.\vil2_pixel_format.h
# End Source File
# Begin Source File
SOURCE=.\vil2_memory_image.h
# End Source File
# Begin Source File
SOURCE=.\vil2_new.h
# End Source File
# Begin Source File
SOURCE=.\vil2_sample_profile_bilin.h
# End Source File
# Begin Source File
SOURCE=.\vil2_bilin_interp.h
# End Source File
# Begin Source File
SOURCE=.\vil2_sample_profile_bilin.txx
# End Source File
# Begin Source File
SOURCE=.\vil2_save.h
# End Source File
# Begin Source File
SOURCE=.\vil2_sample_grid_bilin.h
# End Source File
# Begin Source File
SOURCE=.\vil2_sample_grid_bilin.txx
# End Source File
# Begin Source File
SOURCE=.\vil2_copy.h
# End Source File
# Begin Source File
SOURCE=.\vil2_copy.txx
# End Source File
# Begin Source File
SOURCE=.\vil2_math.h
# End Source File
# Begin Source File
SOURCE=.\vil2_property.h
# End Source File
# Begin Source File
SOURCE=.\vil2_clamp.h
# End Source File
# Begin Source File
SOURCE=.\vil2_transpose.h
# End Source File
# Begin Source File
SOURCE=.\vil2_flip.h
# End Source File
# Begin Source File
SOURCE=.\vil2_plane.h
# End Source File
# Begin Source File
SOURCE=.\vil2_print.h
# End Source File
# Begin Source File
SOURCE=.\vil2_view_as.h
# End Source File
# Begin Source File
SOURCE=.\vil2_fill.h
# End Source File
# Begin Source File
SOURCE=.\vil2_convert.h
# End Source File
# Begin Source File
SOURCE=.\vil2_transform.h
# End Source File
# Begin Source File
SOURCE=.\file_formats/vil2_jpeg.h
# End Source File
# Begin Source File
SOURCE=.\file_formats/vil2_jpeg_compressor.h
# End Source File
# Begin Source File
SOURCE=.\file_formats/vil2_jpeg_decompressor.h
# End Source File
# Begin Source File
SOURCE=.\file_formats/vil2_jpeg_destination_mgr.h
# End Source File
# Begin Source File
SOURCE=.\file_formats/vil2_jpeg_source_mgr.h
# End Source File
# Begin Source File
SOURCE=.\file_formats/vil2_jpeglib.h
# End Source File
# Begin Source File
SOURCE=.\vil2_decimate.h
# End Source File
# Begin Source File
SOURCE=.\vil2_resample_bilin.txx
# End Source File
# Begin Source File
SOURCE=.\vil2_resample_bilin.h
# End Source File
# Begin Source File
SOURCE=.\vil2_vil1.h
# End Source File
# Begin Source File
SOURCE=.\file_formats/vil2_bmp.h
# End Source File
# Begin Source File
SOURCE=.\file_formats/vil2_bmp_core_header.h
# End Source File
# Begin Source File
SOURCE=.\file_formats/vil2_bmp_file_header.h
# End Source File
# Begin Source File
SOURCE=.\file_formats/vil2_bmp_info_header.h
# End Source File
# End Group
SOURCE=.\file_formats/vil2_png.h
# End Source File
# Begin Source File
SOURCE=.\file_formats/vil2_tiff.h
# End Source File
# Begin Source File
# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
