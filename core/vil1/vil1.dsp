# Microsoft Developer Studio Project File - Name="vil1" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# ITK DSP Header file
# This file is read by the build system of itk, and is used as the top part of
# a microsoft project dsp header file
# IF this is in a dsp file, then it is not the header, but has
# already been used, so do not edit here...

# variables to REPLACE
# 
# /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/core" /I "$(VXLROOT)/vcl/config.win32" /I "$(VXLROOT)/v3p/jpeg" /I "$(VXLROOT)/v3p/png" /I "$(VXLROOT)/v3p/zlib" /I "$(VXLROOT)/v3p/tiff" == include path
#   -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE -DHAS_JPEG -DHAS_PNG -DHAS_ZLIB -DHAS_TIFF == compiler defines
#  == override in output directory
# vil1  == name of output library

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=vil1 - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "vil1.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "vil1.mak" CFG="vil1 - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "vil1 - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "vil1 - Win32 Release" (based on "Win32 (x86) Static Library")
# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "vil1 - Win32 Release"

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
# ADD CPP /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/core" /I "$(VXLROOT)/vcl/config.win32" /I "$(VXLROOT)/v3p/jpeg" /I "$(VXLROOT)/v3p/png" /I "$(VXLROOT)/v3p/zlib" /I "$(VXLROOT)/v3p/tiff" -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE -DHAS_JPEG -DHAS_PNG -DHAS_ZLIB -DHAS_TIFF /D "vil1_EXPORTS"
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

!ELSEIF  "$(CFG)" == "vil1 - Win32 Debug"

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
# ADD CPP /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/core" /I "$(VXLROOT)/vcl/config.win32" /I "$(VXLROOT)/v3p/jpeg" /I "$(VXLROOT)/v3p/png" /I "$(VXLROOT)/v3p/zlib" /I "$(VXLROOT)/v3p/tiff" -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE -DHAS_JPEG -DHAS_PNG -DHAS_ZLIB -DHAS_TIFF /D "vil1_EXPORTS"
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

# Name "vil1 - Win32 Release"
# Name "vil1 - Win32 Debug"

# Begin Group "Source Files"
# PROP Default_Filter ""
# Begin Source File
SOURCE=.\Templates/vil1_clamp+vil1_rgb+double-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil1_convolve+byte-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil1_convolve_1d+double.byte.double.double-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil1_convolve_1d+double.double.double.double-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil1_convolve_1d+double.double.double.float-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil1_convolve_1d+double.float.double.double-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil1_convolve_1d+double.uint_16.double.double-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil1_convolve_1d+double.uint_32.double.double-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil1_convolve_1d+float.byte.double.double-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil1_convolve_1d+float.double.double.float-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil1_convolve_simple+byte.double.double.float-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil1_convolve_simple+byte.float.float.byte-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil1_convolve_simple+byte.float.byte.float-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil1_convolve_simple+byte.byte.int.int-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil1_interpolate+double.double-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil1_interpolate+float.double-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil1_interpolate+float.float-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil1_interpolate+uchar.double-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil1_interpolate+uchar.uchar-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil1_interpolate+vil1_rgb+uchar-.vil1_rgb+double--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil1_interpolate+vil1_rgb+uchar-.vil1_rgb+float--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil1_interpolate+vil1_rgb+uchar-.vil1_rgb+uchar--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil1_memory_image_of+bool-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil1_memory_image_of+double-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil1_memory_image_of+float-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil1_memory_image_of+signed_char-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil1_memory_image_of+signed_int-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil1_memory_image_of+signed_short-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil1_memory_image_of+unsigned_char-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil1_memory_image_of+unsigned_int-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil1_memory_image_of+unsigned_short-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil1_memory_image_of+vcl_complex+double--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil1_memory_image_of+vcl_complex+float--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil1_memory_image_of+vil1_rgb+double--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil1_memory_image_of+vil1_rgb+float--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil1_memory_image_of+vil1_rgb+uchar--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil1_memory_image_of+vil1_rgb+vxl_uint_16--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil1_memory_image_of+vil1_rgba+float--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil1_ncc+float.float.double-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil1_ncc+uchar.uchar.double-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil1_resample_image+byte.uint-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil1_resample_image+vil1_rgb+byte-.vil1_rgb+int--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil1_rgb+double-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil1_rgb+float-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil1_rgb+long-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil1_rgb+uint-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil1_rgb+unsignedchar-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil1_rgb+vxl_uint_16-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil1_rgba+float-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil1_rgba+unsignedchar-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil1_ssd+float.float.double-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil1_ssd+uchar.uchar.int-.cxx
# End Source File
# Begin Source File
SOURCE=.\file_formats/vil1_bmp.cxx
# End Source File
# Begin Source File
SOURCE=.\file_formats/vil1_bmp_core_header.cxx
# End Source File
# Begin Source File
SOURCE=.\file_formats/vil1_bmp_file_header.cxx
# End Source File
# Begin Source File
SOURCE=.\file_formats/vil1_bmp_info_header.cxx
# End Source File
# Begin Source File
SOURCE=.\file_formats/vil1_gen.cxx
# End Source File
# Begin Source File
SOURCE=.\file_formats/vil1_gif.cxx
# End Source File
# Begin Source File
SOURCE=.\file_formats/vil1_iris.cxx
# End Source File
# Begin Source File
SOURCE=.\file_formats/vil1_jpeg.cxx
# End Source File
# Begin Source File
SOURCE=.\file_formats/vil1_jpeg_compressor.cxx
# End Source File
# Begin Source File
SOURCE=.\file_formats/vil1_jpeg_decompressor.cxx
# End Source File
# Begin Source File
SOURCE=.\file_formats/vil1_jpeg_destination_mgr.cxx
# End Source File
# Begin Source File
SOURCE=.\file_formats/vil1_jpeg_source_mgr.cxx
# End Source File
# Begin Source File
SOURCE=.\file_formats/vil1_mit.cxx
# End Source File
# Begin Source File
SOURCE=.\file_formats/vil1_png.cxx
# End Source File
# Begin Source File
SOURCE=.\file_formats/vil1_pnm.cxx
# End Source File
# Begin Source File
SOURCE=.\file_formats/vil1_tiff.cxx
# End Source File
# Begin Source File
SOURCE=.\file_formats/vil1_viff.cxx
# End Source File
# Begin Source File
SOURCE=.\file_formats/vil1_viff_support.c
# End Source File
# Begin Source File
SOURCE=.\vil1_16bit.cxx
# End Source File
# Begin Source File
SOURCE=.\vil1_32bit.cxx
# End Source File
# Begin Source File
SOURCE=.\vil1_block_cache_image_impl.cxx
# End Source File
# Begin Source File
SOURCE=.\vil1_byte_swap.cxx
# End Source File
# Begin Source File
SOURCE=.\vil1_clamp.cxx
# End Source File
# Begin Source File
SOURCE=.\vil1_clamp_image.cxx
# End Source File
# Begin Source File
SOURCE=.\vil1_clamp_image_impl.cxx
# End Source File
# Begin Source File
SOURCE=.\vil1_colour_space.cxx
# End Source File
# Begin Source File
SOURCE=.\vil1_copy.cxx
# End Source File
# Begin Source File
SOURCE=.\vil1_crop.cxx
# End Source File
# Begin Source File
SOURCE=.\vil1_crop_image_impl.cxx
# End Source File
# Begin Source File
SOURCE=.\vil1_file_format.cxx
# End Source File
# Begin Source File
SOURCE=.\vil1_file_image.cxx
# End Source File
# Begin Source File
SOURCE=.\vil1_flipud.cxx
# End Source File
# Begin Source File
SOURCE=.\vil1_flipud_impl.cxx
# End Source File
# Begin Source File
SOURCE=.\vil1_flip_components.cxx
# End Source File
# Begin Source File
SOURCE=.\vil1_flip_components_impl.cxx
# End Source File
# Begin Source File
SOURCE=.\vil1_image.cxx
# End Source File
# Begin Source File
SOURCE=.\vil1_image_as.cxx
# End Source File
# Begin Source File
SOURCE=.\vil1_image_impl.cxx
# End Source File
# Begin Source File
SOURCE=.\vil1_image_proxy.cxx
# End Source File
# Begin Source File
SOURCE=.\vil1_load.cxx
# End Source File
# Begin Source File
SOURCE=.\vil1_memory_image.cxx
# End Source File
# Begin Source File
SOURCE=.\vil1_memory_image_impl.cxx
# End Source File
# Begin Source File
SOURCE=.\vil1_memory_image_window.cxx
# End Source File
# Begin Source File
SOURCE=.\vil1_new.cxx
# End Source File
# Begin Source File
SOURCE=.\vil1_open.cxx
# End Source File
# Begin Source File
SOURCE=.\vil1_pixel.cxx
# End Source File
# Begin Source File
SOURCE=.\vil1_pyramid.cxx
# End Source File
# Begin Source File
SOURCE=.\vil1_resample.cxx
# End Source File
# Begin Source File
SOURCE=.\vil1_resample_image_impl.cxx
# End Source File
# Begin Source File
SOURCE=.\vil1_rgb_byte.cxx
# End Source File
# Begin Source File
SOURCE=.\vil1_save.cxx
# End Source File
# Begin Source File
SOURCE=.\vil1_scale_intensities.cxx
# End Source File
# Begin Source File
SOURCE=.\vil1_scale_intensities_image.cxx
# End Source File
# Begin Source File
SOURCE=.\vil1_scale_intensities_image_impl.cxx
# End Source File
# Begin Source File
SOURCE=.\vil1_skip.cxx
# End Source File
# Begin Source File
SOURCE=.\vil1_skip_image_impl.cxx
# End Source File
# Begin Source File
SOURCE=.\vil1_smooth.cxx
# End Source File
# Begin Source File
SOURCE=.\vil1_stream.cxx
# End Source File
# Begin Source File
SOURCE=.\vil1_stream_core.cxx
# End Source File
# Begin Source File
SOURCE=.\vil1_stream_fstream.cxx
# End Source File
# Begin Source File
SOURCE=.\vil1_stream_section.cxx
# End Source File
# Begin Source File
SOURCE=.\vil1_stream_url.cxx
# End Source File
# Begin Source File
SOURCE=.\vil1_warp.cxx
# End Source File
# Begin Source File
SOURCE=.\file_formats/vil1_ras.cxx
# End Source File
# End Group
# Begin Group "Header Files"
# PROP Default_Filter ""
# Begin Source File
SOURCE=.\file_formats/vil1_bmp.h
# End Source File
# Begin Source File
SOURCE=.\file_formats/vil1_bmp_core_header.h
# End Source File
# Begin Source File
SOURCE=.\file_formats/vil1_bmp_file_header.h
# End Source File
# Begin Source File
SOURCE=.\file_formats/vil1_bmp_info_header.h
# End Source File
# Begin Source File
SOURCE=.\vil1_vil.h
# End Source File
# Begin Source File
SOURCE=.\file_formats/vil1_gen.h
# End Source File
# Begin Source File
SOURCE=.\file_formats/vil1_gif.h
# End Source File
# Begin Source File
SOURCE=.\file_formats/vil1_iris.h
# End Source File
# Begin Source File
SOURCE=.\file_formats/vil1_jpeg.h
# End Source File
# Begin Source File
SOURCE=.\file_formats/vil1_jpeg_compressor.h
# End Source File
# Begin Source File
SOURCE=.\file_formats/vil1_jpeg_decompressor.h
# End Source File
# Begin Source File
SOURCE=.\file_formats/vil1_jpeg_destination_mgr.h
# End Source File
# Begin Source File
SOURCE=.\file_formats/vil1_jpeg_source_mgr.h
# End Source File
# Begin Source File
SOURCE=.\file_formats/vil1_mit.h
# End Source File
# Begin Source File
SOURCE=.\file_formats/vil1_png.h
# End Source File
# Begin Source File
SOURCE=.\file_formats/vil1_pnm.h
# End Source File
# Begin Source File
SOURCE=.\file_formats/vil1_tiff.h
# End Source File
# Begin Source File
SOURCE=.\file_formats/vil1_viff.h
# End Source File
# Begin Source File
SOURCE=.\vil1_16bit.h
# End Source File
# Begin Source File
SOURCE=.\vil1_32bit.h
# End Source File
# Begin Source File
SOURCE=.\vil1_block_cache_image_impl.h
# End Source File
# Begin Source File
SOURCE=.\vil1_byte_swap.h
# End Source File
# Begin Source File
SOURCE=.\vil1_clamp.h
# End Source File
# Begin Source File
SOURCE=.\vil1_clamp_image.h
# End Source File
# Begin Source File
SOURCE=.\vil1_clamp_image_impl.h
# End Source File
# Begin Source File
SOURCE=.\vil1_colour_space.h
# End Source File
# Begin Source File
SOURCE=.\vil1_convolve.h
# End Source File
# Begin Source File
SOURCE=.\vil1_convolve.txx
# End Source File
# Begin Source File
SOURCE=.\vil1_convolve_1d.txx
# End Source File
# Begin Source File
SOURCE=.\vil1_convolve_1d_x.txx
# End Source File
# Begin Source File
SOURCE=.\vil1_convolve_1d_y.txx
# End Source File
# Begin Source File
SOURCE=.\vil1_convolve_simple.h
# End Source File
# Begin Source File
SOURCE=.\vil1_convolve_simple.txx
# End Source File
# Begin Source File
SOURCE=.\vil1_copy.h
# End Source File
# Begin Source File
SOURCE=.\vil1_crop.h
# End Source File
# Begin Source File
SOURCE=.\vil1_crop_image_impl.h
# End Source File
# Begin Source File
SOURCE=.\vil1_file_format.h
# End Source File
# Begin Source File
SOURCE=.\vil1_file_image.h
# End Source File
# Begin Source File
SOURCE=.\vil1_flip_components.h
# End Source File
# Begin Source File
SOURCE=.\vil1_flip_components_impl.h
# End Source File
# Begin Source File
SOURCE=.\vil1_flipud.h
# End Source File
# Begin Source File
SOURCE=.\vil1_flipud_impl.h
# End Source File
# Begin Source File
SOURCE=.\vil1_image.h
# End Source File
# Begin Source File
SOURCE=.\vil1_image_as.h
# End Source File
# Begin Source File
SOURCE=.\vil1_image_impl.h
# End Source File
# Begin Source File
SOURCE=.\vil1_image_proxy.h
# End Source File
# Begin Source File
SOURCE=.\vil1_interpolate.h
# End Source File
# Begin Source File
SOURCE=.\vil1_interpolate.txx
# End Source File
# Begin Source File
SOURCE=.\vil1_load.h
# End Source File
# Begin Source File
SOURCE=.\vil1_memory_image.h
# End Source File
# Begin Source File
SOURCE=.\vil1_memory_image_impl.h
# End Source File
# Begin Source File
SOURCE=.\vil1_memory_image_of.h
# End Source File
# Begin Source File
SOURCE=.\vil1_memory_image_of.txx
# End Source File
# Begin Source File
SOURCE=.\vil1_memory_image_of_format.txx
# End Source File
# Begin Source File
SOURCE=.\vil1_memory_image_window.h
# End Source File
# Begin Source File
SOURCE=.\vil1_ncc.h
# End Source File
# Begin Source File
SOURCE=.\vil1_ncc.txx
# End Source File
# Begin Source File
SOURCE=.\vil1_new.h
# End Source File
# Begin Source File
SOURCE=.\vil1_open.h
# End Source File
# Begin Source File
SOURCE=.\vil1_pixel.h
# End Source File
# Begin Source File
SOURCE=.\vil1_pyramid.h
# End Source File
# Begin Source File
SOURCE=.\vil1_resample.h
# End Source File
# Begin Source File
SOURCE=.\vil1_resample_image.h
# End Source File
# Begin Source File
SOURCE=.\vil1_resample_image.txx
# End Source File
# Begin Source File
SOURCE=.\vil1_resample_image_impl.h
# End Source File
# Begin Source File
SOURCE=.\vil1_rgb.h
# End Source File
# Begin Source File
SOURCE=.\vil1_rgb.txx
# End Source File
# Begin Source File
SOURCE=.\vil1_rgb_byte.h
# End Source File
# Begin Source File
SOURCE=.\vil1_rgba.h
# End Source File
# Begin Source File
SOURCE=.\vil1_rgba.txx
# End Source File
# Begin Source File
SOURCE=.\vil1_save.h
# End Source File
# Begin Source File
SOURCE=.\vil1_scale_intensities.h
# End Source File
# Begin Source File
SOURCE=.\vil1_scale_intensities_image.h
# End Source File
# Begin Source File
SOURCE=.\vil1_scale_intensities_image_impl.h
# End Source File
# Begin Source File
SOURCE=.\vil1_skip.h
# End Source File
# Begin Source File
SOURCE=.\vil1_skip_image_impl.h
# End Source File
# Begin Source File
SOURCE=.\vil1_smooth.h
# End Source File
# Begin Source File
SOURCE=.\vil1_ssd.h
# End Source File
# Begin Source File
SOURCE=.\vil1_ssd.txx
# End Source File
# Begin Source File
SOURCE=.\vil1_stream.h
# End Source File
# Begin Source File
SOURCE=.\vil1_stream_core.h
# End Source File
# Begin Source File
SOURCE=.\vil1_stream_fstream.h
# End Source File
# Begin Source File
SOURCE=.\vil1_stream_section.h
# End Source File
# Begin Source File
SOURCE=.\vil1_stream_url.h
# End Source File
# Begin Source File
SOURCE=.\vil1_warp.h
# End Source File
# Begin Source File
SOURCE=.\vil1_warp.txx
# End Source File
# Begin Source File
SOURCE=.\file_formats/vil1_viff_support.h
# End Source File
# Begin Source File
SOURCE=.\file_formats/vil1_viffheader.h
# End Source File
# Begin Source File
SOURCE=.\vil1_fwd.h
# End Source File
# Begin Source File
SOURCE=.\vil1_ip_traits.h
# End Source File
# Begin Source File
SOURCE=.\vil1_jpeglib.h
# End Source File
# Begin Source File
SOURCE=.\vil1_property.h
# End Source File
# Begin Source File
SOURCE=.\file_formats/vil1_ras.h
# End Source File
# End Group
# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
