# Microsoft Developer Studio Project File - Name="vil" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# ITK DSP Header file
# This file is read by the build system of itk, and is used as the top part of
# a microsoft project dsp header file
# IF this is in a dsp file, then it is not the header, but has
# already been used, so do not edit here...

# variables to REPLACE
#
# /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/core" /I "$(VXLROOT)/vcl/config.win32" == include path
#   -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE == compiler defines
#  == override in output directory
# vil  == name of output library

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=vil - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE
!MESSAGE NMAKE /f "vil.mak".
!MESSAGE
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE
!MESSAGE NMAKE /f "vil.mak" CFG="vil - Win32 Debug"
!MESSAGE
!MESSAGE Possible choices for configuration are:
!MESSAGE
!MESSAGE "vil - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "vil - Win32 Release" (based on "Win32 (x86) Static Library")
# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "vil - Win32 Release"

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
# ADD CPP /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/core" /I "$(VXLROOT)/vcl/config.win32" -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE /D "vil_EXPORTS"
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

!ELSEIF  "$(CFG)" == "vil - Win32 Debug"

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
# ADD CPP /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/core" /I "$(VXLROOT)/vcl/config.win32" -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE /D "vil_EXPORTS"
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

# Name "vil - Win32 Release"
# Name "vil - Win32 Debug"

# Begin Group "Source Files"
# PROP Default_Filter "c;cxx"
# Begin Source File
SOURCE=.\file_formats/vil_pnm.cxx
# End Source File
# Begin Source File
SOURCE=.\file_formats/vil_ras.cxx
# End Source File
# Begin Source File
SOURCE=.\vil_image_resource.cxx
# End Source File
# Begin Source File
SOURCE=.\vil_memory_chunk.cxx
# End Source File
# Begin Source File
SOURCE=.\vil_file_format.cxx
# End Source File
# Begin Source File
SOURCE=.\file_formats/vil_png.cxx
# End Source File
# Begin Source File
SOURCE=.\file_formats/vil_tiff.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil_image_view+bool-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil_smart_ptr+vil_image_resource-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil_smart_ptr+vil_memory_chunk-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil_image_view+float-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil_image_view+vil_rgb+float--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil_image_view+vcl_complex+double--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil_image_view+vcl_complex+float--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil_smart_ptr+vil_stream-.cxx
# End Source File
# Begin Source File
SOURCE=.\vil_crop.cxx
# End Source File
# Begin Source File
SOURCE=.\vil_load.cxx
# End Source File
# Begin Source File
SOURCE=.\vil_decimate.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil_sample_profile_bilin+float.double-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil_sample_grid_bilin+float.double-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil_smart_ptr+vil_image_view_base-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil_image_view+byte-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil_image_view+int_16-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil_image_view+int_32-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil_image_view+uint_32-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil_image_view+vil_rgb+int_16--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil_image_view+vil_rgb+int_32--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil_image_view+vil_rgb+uint_32--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil_sample_grid_bilin+byte.double-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil_sample_profile_bilin+byte.double-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil_image_view+uint_16-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil_image_view+vil_rgb+byte--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil_image_view+vil_rgb+uint_16--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil_image_view+vil_rgb+double--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil_image_view+vil_rgba+byte--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil_image_view+double-.cxx
# End Source File
# Begin Source File
SOURCE=.\vil_copy.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil_copy+bool-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil_copy+byte-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil_copy+double-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil_copy+float-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil_copy+int_16-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil_copy+int_32-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil_copy+sbyte-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil_copy+uint_16-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil_copy+uint_32-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil_copy+vil_rgb+byte--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil_image_view+sbyte-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil_image_view+vil_rgb+sbyte--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil_resample_bilin+byte.double-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil_resample_bilin+byte.float-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil_resample_bilin+float.double-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil_sample_grid_bilin+float.float-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil_sample_grid_bilin+byte.float-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil_image_view+vil_rgba+float--.cxx
# End Source File
# Begin Source File
SOURCE=.\file_formats/vil_bmp.cxx
# End Source File
# Begin Source File
SOURCE=.\file_formats/vil_bmp_core_header.cxx
# End Source File
# Begin Source File
SOURCE=.\file_formats/vil_bmp_file_header.cxx
# End Source File
# Begin Source File
SOURCE=.\file_formats/vil_bmp_info_header.cxx
# End Source File
# Begin Source File
SOURCE=.\vil_memory_image.cxx
# End Source File
# Begin Source File
SOURCE=.\vil_new.cxx
# End Source File
# Begin Source File
SOURCE=.\vil_pixel_format.cxx
# End Source File
# Begin Source File
SOURCE=.\vil_save.cxx
# End Source File
# Begin Source File
SOURCE=.\vil_clamp.cxx
# End Source File
# Begin Source File
SOURCE=.\vil_transpose.cxx
# End Source File
# Begin Source File
SOURCE=.\vil_flip.cxx
# End Source File
# Begin Source File
SOURCE=.\vil_plane.cxx
# End Source File
# Begin Source File
SOURCE=.\vil_print.cxx
# End Source File
# Begin Source File
SOURCE=.\file_formats/vil_jpeg.cxx
# End Source File
# Begin Source File
SOURCE=.\file_formats/vil_jpeg_compressor.cxx
# End Source File
# Begin Source File
SOURCE=.\file_formats/vil_jpeg_decompressor.cxx
# End Source File
# Begin Source File
SOURCE=.\file_formats/vil_jpeg_destination_mgr.cxx
# End Source File
# Begin Source File
SOURCE=.\file_formats/vil_jpeg_source_mgr.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil_rgb+double-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil_rgb+float-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil_rgba+float-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil_resample_bilin+byte.byte-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil_resample_bicub+byte.byte-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil_resample_bicub+byte.double-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil_resample_bicub+byte.float-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil_resample_bicub+float.double-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil_sample_grid_bicub+byte.double-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil_sample_grid_bicub+float.double-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil_sample_grid_bicub+float.float-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil_sample_profile_bicub+byte.double-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil_sample_profile_bicub+float.double-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil_bicub_interp+byte-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil_bicub_interp+double-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil_bicub_interp+float-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil_copy+vcl_complex+double--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil_copy+vcl_complex+float--.cxx
# End Source File
# Begin Source File
SOURCE=.\vil_open.cxx
# End Source File
# Begin Source File
SOURCE=.\vil_stream.cxx
# End Source File
# Begin Source File
SOURCE=.\vil_stream_read.cxx
# End Source File
# Begin Source File
SOURCE=.\vil_stream_write.cxx
# End Source File
# Begin Source File
SOURCE=.\vil_stream_core.cxx
# End Source File
# Begin Source File
SOURCE=.\vil_stream_fstream.cxx
# End Source File
# Begin Source File
SOURCE=.\vil_stream_fstream64.cxx
# End Source File
# Begin Source File
SOURCE=.\vil_stream_section.cxx
# End Source File
# Begin Source File
SOURCE=.\vil_stream_url.cxx
# End Source File
# Begin Source File
SOURCE=.\vil_image_resource_plugin.cxx
# End Source File
# Begin Source File
SOURCE=.\vil_block_cache.cxx
# End Source File
# Begin Source File
SOURCE=.\vil_blocked_image_facade.cxx
# End Source File
# Begin Source File
SOURCE=.\vil_blocked_image_resource.cxx
# End Source File
# Begin Source File
SOURCE=.\vil_cached_image_resource.cxx
# End Source File
# Begin Source File
SOURCE=.\file_formats/vil_dicom.cxx
# End Source File
# Begin Source File
SOURCE=.\file_formats/vil_dicom_header.cxx
# End Source File
# Begin Source File
SOURCE=.\file_formats/vil_dicom_stream.cxx
# End Source File
# Begin Source File
SOURCE=.\file_formats/vil_iris.cxx
# End Source File
# Begin Source File
SOURCE=.\file_formats/vil_mit.cxx
# End Source File
# Begin Source File
SOURCE=.\file_formats/vil_viff.cxx
# End Source File
# Begin Source File
SOURCE=.\file_formats/vil_viff_support.c
# End Source File
# Begin Source File
SOURCE=.\file_formats/vil_nitf2_array_field.cxx
# End Source File
# Begin Source File
SOURCE=.\file_formats/vil_nitf2_classification.cxx
# End Source File
# Begin Source File
SOURCE=.\file_formats/vil_nitf2_compound_field_value.cxx
# End Source File
# Begin Source File
SOURCE=.\file_formats/vil_nitf2_data_mask_table.cxx
# End Source File
# Begin Source File
SOURCE=.\file_formats/vil_nitf2_field.cxx
# End Source File
# Begin Source File
SOURCE=.\file_formats/vil_nitf2_field_definition.cxx
# End Source File
# Begin Source File
SOURCE=.\file_formats/vil_nitf2_field_formatter.cxx
# End Source File
# Begin Source File
SOURCE=.\file_formats/vil_nitf2_field_functor.cxx
# End Source File
# Begin Source File
SOURCE=.\file_formats/vil_nitf2_field_sequence.cxx
# End Source File
# Begin Source File
SOURCE=.\file_formats/vil_nitf2_header.cxx
# End Source File
# Begin Source File
SOURCE=.\file_formats/vil_nitf2_image.cxx
# End Source File
# Begin Source File
SOURCE=.\file_formats/vil_nitf2_image_subheader.cxx
# End Source File
# Begin Source File
SOURCE=.\file_formats/vil_nitf2_scalar_field.cxx
# End Source File
# Begin Source File
SOURCE=.\file_formats/vil_nitf2_tagged_record.cxx
# End Source File
# Begin Source File
SOURCE=.\file_formats/vil_nitf2_tagged_record_definition.cxx
# End Source File
# Begin Source File
SOURCE=.\file_formats/vil_nitf2_typed_field_formatter.cxx
# End Source File
# Begin Source File
SOURCE=.\file_formats/vil_nitf2_des.cxx
# End Source File
# Begin Source File
SOURCE=.\file_formats/vil_tiff_header.cxx
# End Source File
# Begin Source File
SOURCE=.\file_formats/vil_nitf2.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil_resample_bilin+float.float-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil_rotate+byte.byte-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil_rotate+float.float-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil_image_view+vil_rgba+double--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil_image_view+vil_rgba+int_16--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil_image_view+vil_rgba+int_32--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil_image_view+vil_rgba+sbyte--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil_image_view+vil_rgba+uint_16--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil_image_view+vil_rgba+uint_32--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil_rgb+byte-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil_rgb+int_16-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil_rgb+int_32-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil_rgb+uint_16-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil_rgb+uint_32-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil_rgb+sbyte-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil_rgba+byte-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil_rgba+double-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil_rgba+int_16-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil_rgba+int_32-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil_rgba+sbyte-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil_rgba+uint_16-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil_rgba+uint_32-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil_rgb+int_64-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil_rgb+uint_64-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil_rgba+int_64-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil_rgba+uint_64-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil_copy+int_64-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil_copy+uint_64-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil_image_view+int_64-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil_image_view+uint_64-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil_image_view+vil_rgb+int_64--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil_image_view+vil_rgb+uint_64--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil_image_view+vil_rgba+int_64--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil_image_view+vil_rgba+uint_64--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil_sample_grid_bilin+uint_16.double-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil_sample_profile_bilin+uint_16.double-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil_sample_grid_bilin+uint_16.float-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil_smart_ptr+vil_blocked_image_resource-.cxx
# End Source File
# End Group
# Begin Group "Header Files"
# PROP Default_Filter "h;txx"
# Begin Source File
SOURCE=.\file_formats/vil_pnm.h
# End Source File
# Begin Source File
SOURCE=.\file_formats/vil_ras.h
# End Source File
# Begin Source File
SOURCE=.\vil_image_resource.h
# End Source File
# Begin Source File
SOURCE=.\vil_image_resource_sptr.h
# End Source File
# Begin Source File
SOURCE=.\vil_image_view.h
# End Source File
# Begin Source File
SOURCE=.\vil_image_view_base.h
# End Source File
# Begin Source File
SOURCE=.\vil_smart_ptr.h
# End Source File
# Begin Source File
SOURCE=.\vil_smart_ptr.txx
# End Source File
# Begin Source File
SOURCE=.\vil_image_view.txx
# End Source File
# Begin Source File
SOURCE=.\vil_memory_chunk.h
# End Source File
# Begin Source File
SOURCE=.\vil_file_format.h
# End Source File
# Begin Source File
SOURCE=.\vil_load.h
# End Source File
# Begin Source File
SOURCE=.\vil_crop.h
# End Source File
# Begin Source File
SOURCE=.\vil_pixel_format.h
# End Source File
# Begin Source File
SOURCE=.\vil_memory_image.h
# End Source File
# Begin Source File
SOURCE=.\vil_new.h
# End Source File
# Begin Source File
SOURCE=.\vil_sample_profile_bilin.h
# End Source File
# Begin Source File
SOURCE=.\vil_bilin_interp.h
# End Source File
# Begin Source File
SOURCE=.\vil_sample_profile_bilin.txx
# End Source File
# Begin Source File
SOURCE=.\vil_save.h
# End Source File
# Begin Source File
SOURCE=.\vil_sample_grid_bilin.h
# End Source File
# Begin Source File
SOURCE=.\vil_sample_grid_bilin.txx
# End Source File
# Begin Source File
SOURCE=.\vil_copy.h
# End Source File
# Begin Source File
SOURCE=.\vil_copy.txx
# End Source File
# Begin Source File
SOURCE=.\vil_math.h
# End Source File
# Begin Source File
SOURCE=.\vil_property.h
# End Source File
# Begin Source File
SOURCE=.\vil_clamp.h
# End Source File
# Begin Source File
SOURCE=.\vil_transpose.h
# End Source File
# Begin Source File
SOURCE=.\vil_flip.h
# End Source File
# Begin Source File
SOURCE=.\vil_plane.h
# End Source File
# Begin Source File
SOURCE=.\vil_print.h
# End Source File
# Begin Source File
SOURCE=.\vil_view_as.h
# End Source File
# Begin Source File
SOURCE=.\vil_fill.h
# End Source File
# Begin Source File
SOURCE=.\vil_convert.h
# End Source File
# Begin Source File
SOURCE=.\vil_transform.h
# End Source File
# Begin Source File
SOURCE=.\file_formats/vil_jpeg.h
# End Source File
# Begin Source File
SOURCE=.\file_formats/vil_jpeg_compressor.h
# End Source File
# Begin Source File
SOURCE=.\file_formats/vil_jpeg_decompressor.h
# End Source File
# Begin Source File
SOURCE=.\file_formats/vil_jpeg_destination_mgr.h
# End Source File
# Begin Source File
SOURCE=.\file_formats/vil_jpeg_source_mgr.h
# End Source File
# Begin Source File
SOURCE=.\file_formats/vil_jpeglib.h
# End Source File
# Begin Source File
SOURCE=.\vil_decimate.h
# End Source File
# Begin Source File
SOURCE=.\vil_resample_bilin.txx
# End Source File
# Begin Source File
SOURCE=.\vil_resample_bilin.h
# End Source File
# Begin Source File
SOURCE=.\vil_bicub_interp.h
# End Source File
# Begin Source File
SOURCE=.\vil_resample_bicub.h
# End Source File
# Begin Source File
SOURCE=.\vil_resample_bicub.txx
# End Source File
# Begin Source File
SOURCE=.\vil_sample_grid_bicub.h
# End Source File
# Begin Source File
SOURCE=.\vil_sample_grid_bicub.txx
# End Source File
# Begin Source File
SOURCE=.\vil_sample_profile_bicub.h
# End Source File
# Begin Source File
SOURCE=.\vil_sample_profile_bicub.txx
# End Source File
# Begin Source File
SOURCE=.\vil_bicub_interp.txx
# End Source File
# Begin Source File
SOURCE=.\file_formats/vil_bmp.h
# End Source File
# Begin Source File
SOURCE=.\file_formats/vil_bmp_core_header.h
# End Source File
# Begin Source File
SOURCE=.\file_formats/vil_bmp_file_header.h
# End Source File
# Begin Source File
SOURCE=.\file_formats/vil_bmp_info_header.h
# End Source File
# Begin Source File
SOURCE=.\file_formats/vil_png.h
# End Source File
# Begin Source File
SOURCE=.\file_formats/vil_tiff.h
# End Source File
# Begin Source File
SOURCE=.\vil_pixel_traits.h
# End Source File
# Begin Source File
SOURCE=.\vil_fwd.h
# End Source File
# Begin Source File
SOURCE=.\vil_open.h
# End Source File
# Begin Source File
SOURCE=.\vil_rgb.h
# End Source File
# Begin Source File
SOURCE=.\vil_rgb.txx
# End Source File
# Begin Source File
SOURCE=.\vil_rgba.h
# End Source File
# Begin Source File
SOURCE=.\vil_rgba.txx
# End Source File
# Begin Source File
SOURCE=.\vil_stream.h
# End Source File
# Begin Source File
SOURCE=.\vil_stream_read.h
# End Source File
# Begin Source File
SOURCE=.\vil_stream_write.h
# End Source File
# Begin Source File
SOURCE=.\vil_stream_core.h
# End Source File
# Begin Source File
SOURCE=.\vil_stream_fstream.h
# End Source File
# Begin Source File
SOURCE=.\vil_stream_section.h
# End Source File
# Begin Source File
SOURCE=.\vil_stream_url.h
# End Source File
# Begin Source File
SOURCE=.\vil_image_resource_plugin.h
# End Source File
# Begin Source File
SOURCE=.\file_formats/vil_dicom.h
# End Source File
# Begin Source File
SOURCE=.\file_formats/vil_dicom_header.h
# End Source File
# Begin Source File
SOURCE=.\file_formats/vil_dicom_stream.h
# End Source File
# Begin Source File
SOURCE=.\vil_rotate.h
# End Source File
# Begin Source File
SOURCE=.\vil_rotate.txx
# End Source File
# Begin Source File
SOURCE=.\file_formats/vil_iris.h
# End Source File
# Begin Source File
SOURCE=.\file_formats/vil_mit.h
# End Source File
# Begin Source File
SOURCE=.\file_formats/vil_viff.h
# End Source File
# Begin Source File
SOURCE=.\file_formats/vil_viff_support.h
# End Source File
# Begin Source File
SOURCE=.\file_formats/vil_viffheader.h
# End Source File
# Begin Source File
SOURCE=.\file_formats/vil_nitf2.h
# End Source File
# Begin Source File
SOURCE=.\file_formats/vil_nitf2_array_field.h
# End Source File
# Begin Source File
SOURCE=.\file_formats/vil_nitf2_classification.h
# End Source File
# Begin Source File
SOURCE=.\file_formats/vil_nitf2_compound_field_value.h
# End Source File
# Begin Source File
SOURCE=.\file_formats/vil_nitf2_data_mask_table.h
# End Source File
# Begin Source File
SOURCE=.\file_formats/vil_nitf2_field.h
# End Source File
# Begin Source File
SOURCE=.\file_formats/vil_nitf2_field_definition.h
# End Source File
# Begin Source File
SOURCE=.\file_formats/vil_nitf2_field_formatter.h
# End Source File
# Begin Source File
SOURCE=.\file_formats/vil_nitf2_field_functor.h
# End Source File
# Begin Source File
SOURCE=.\file_formats/vil_nitf2_field_sequence.h
# End Source File
# Begin Source File
SOURCE=.\file_formats/vil_nitf2_header.h
# End Source File
# Begin Source File
SOURCE=.\file_formats/vil_nitf2_image.h
# End Source File
# Begin Source File
SOURCE=.\file_formats/vil_nitf2_image_subheader.h
# End Source File
# Begin Source File
SOURCE=.\file_formats/vil_nitf2_index_vector.h
# End Source File
# Begin Source File
SOURCE=.\file_formats/vil_nitf2_scalar_field.h
# End Source File
# Begin Source File
SOURCE=.\file_formats/vil_nitf2_tagged_record.h
# End Source File
# Begin Source File
SOURCE=.\file_formats/vil_nitf2_tagged_record_definition.h
# End Source File
# Begin Source File
SOURCE=.\file_formats/vil_nitf2_typed_array_field.h
# End Source File
# Begin Source File
SOURCE=.\file_formats/vil_nitf2_typed_field_formatter.h
# End Source File
# Begin Source File
SOURCE=.\file_formats/vil_nitf2_typed_scalar_field.h
# End Source File
# Begin Source File
SOURCE=.\file_formats/vil_nitf2_des.h
# End Source File
# Begin Source File
SOURCE=.\vil_nearest_neighbour_interp.h
# End Source File
# Begin Source File
SOURCE=.\vil_stream_fstream64.h
# End Source File
# Begin Source File
SOURCE=.\vil_color_table.h
# End Source File
# Begin Source File
SOURCE=.\vil_warp.h
# End Source File
# Begin Source File
SOURCE=.\vil_exception.h
# End Source File
# Begin Source File
SOURCE=.\file_formats/vil_tiff_header.h
# End Source File
# Begin Source File
SOURCE=.\vil_block_cache.h
# End Source File
# Begin Source File
SOURCE=.\vil_blocked_image_facade.h
# End Source File
# Begin Source File
SOURCE=.\vil_blocked_image_resource.h
# End Source File
# Begin Source File
SOURCE=.\vil_blocked_image_resource_sptr.h
# End Source File
# Begin Source File
SOURCE=.\vil_cached_image_resource.h
# End Source File
# Begin Source File
SOURCE=.\vil_config.h.in
# End Source File
# End Group
# End Target
# End Project
