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
SOURCE=.\vil2_image_data.cxx
# End Source File
# Begin Source File
SOURCE=.\vil2_image_view.cxx
# End Source File
# Begin Source File
SOURCE=.\vil2_memory_chunk.cxx
# End Source File
# Begin Source File
SOURCE=.\vil2_file_format.cxx
# End Source File
# Begin Source File
SOURCE=.\vil2_image_view_functions.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil2_image_view+uchar-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil2_image_view+vil_rgb+uchar--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil2_image_view_functions+uchar-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil2_image_view_functions+vil_rgb+uchar--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil2_image_view+bool-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil2_image_view+uint-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil2_image_view+ushort-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil2_image_view_functions+bool-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil2_image_view_functions+int-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil2_image_view_functions+short-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil2_image_view_functions+uint-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil2_image_view_functions+ushort-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil2_smart_ptr+vil2_image_data-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil2_smart_ptr+vil2_memory_chunk-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil2_image_view+float-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil2_image_view+int-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil2_image_view+short-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil2_image_view+vil_rgb+float--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil2_image_view+vil_rgb+int--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil2_image_view+vil_rgb+short--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil2_image_view+vil_rgb+uint--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil2_image_view+vil_rgb+ushort--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil2_image_view_functions+float-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil2_image_view_functions+vil_rgb+float--.cxx
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
SOURCE=.\Templates/vil2_sample_profile_bilin+uchar.double-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil2_sample_profile_bilin+float.double-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil2_sample_grid_bilin+float.double-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil2_sample_grid_bilin+uchar.double-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil2_smart_ptr+vil2_image_view_base-.cxx
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
# End Group
# Begin Group "Header Files"
# PROP Default_Filter ""
# Begin Source File
SOURCE=.\file_formats/vil2_pnm.h
# End Source File
# Begin Source File
SOURCE=.\vil2_image_data.h
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
SOURCE=.\vil2_image_view_functions.h
# End Source File
# Begin Source File
SOURCE=.\vil2_image_view_functions.txx
# End Source File
# Begin Source File
SOURCE=.\vil2_load.h
# End Source File
# Begin Source File
SOURCE=.\vil2_reg_image.h
# End Source File
# Begin Source File
SOURCE=.\vil2_reg_image_2d.h
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
SOURCE=.\vil2_byte.h
# End Source File
# Begin Source File
SOURCE=.\vil2_pixel_traits.h
# End Source File
# Begin Source File
SOURCE=.\vil2_sample_grid_bilin.h
# End Source File
# Begin Source File
SOURCE=.\vil2_sample_grid_bilin.txx
# End Source File
# End Group
# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
