# Microsoft Developer Studio Project File - Name="vidl_vil1" - Package Owner=<4>
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
# vidl_vil1  == name of output library

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=vidl_vil1 - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "vidl_vil1.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "vidl_vil1.mak" CFG="vidl_vil1 - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "vidl_vil1 - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "vidl_vil1 - Win32 Release" (based on "Win32 (x86) Static Library")
# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "vidl_vil1 - Win32 Release"

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
# ADD CPP /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/core" /I "$(VXLROOT)/vcl/config.win32" -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE /D "vidl_vil1_EXPORTS"
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

!ELSEIF  "$(CFG)" == "vidl_vil1 - Win32 Debug"

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
# ADD CPP /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/core" /I "$(VXLROOT)/vcl/config.win32" -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE /D "vidl_vil1_EXPORTS"
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

# Name "vidl_vil1 - Win32 Release"
# Name "vidl_vil1 - Win32 Debug"

# Begin Group "Source Files"
# PROP Default_Filter ""
# Begin Source File
SOURCE=.\vidl_vil1_mpegcodec.cxx
# End Source File
# Begin Source File
SOURCE=.\vidl_vil1_mpegcodec_helper.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vbl_smart_ptr+vidl_vil1_clip-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vbl_smart_ptr+vidl_vil1_codec-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vbl_smart_ptr+vidl_vil1_frame-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vbl_smart_ptr+vidl_vil1_image_list_codec-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vbl_smart_ptr+vidl_vil1_movie-.cxx
# End Source File
# Begin Source File
SOURCE=.\vidl_vil1_avicodec.cxx
# End Source File
# Begin Source File
SOURCE=.\vidl_vil1_clip.cxx
# End Source File
# Begin Source File
SOURCE=.\vidl_vil1_frame.cxx
# End Source File
# Begin Source File
SOURCE=.\vidl_vil1_frame_as_image.cxx
# End Source File
# Begin Source File
SOURCE=.\vidl_vil1_image_list_codec.cxx
# End Source File
# Begin Source File
SOURCE=.\vidl_vil1_io.cxx
# End Source File
# Begin Source File
SOURCE=.\vidl_vil1_movie.cxx
# End Source File
# Begin Source File
SOURCE=.\vidl_vil1_file_sequence.cxx
# End Source File
# Begin Source File
SOURCE=.\vidl_vil1_vob_frame_index.cxx
# End Source File
# End Group
# Begin Group "Header Files"
# PROP Default_Filter ""
# Begin Source File
SOURCE=.\vidl_vil1_avicodec.h
# End Source File
# Begin Source File
SOURCE=.\vidl_vil1_clip.h
# End Source File
# Begin Source File
SOURCE=.\vidl_vil1_frame.h
# End Source File
# Begin Source File
SOURCE=.\vidl_vil1_frame_as_image.h
# End Source File
# Begin Source File
SOURCE=.\vidl_vil1_image_list_codec.h
# End Source File
# Begin Source File
SOURCE=.\vidl_vil1_io.h
# End Source File
# Begin Source File
SOURCE=.\vidl_vil1_movie.h
# End Source File
# Begin Source File
SOURCE=.\vidl_vil1_avicodec_sptr.h
# End Source File
# Begin Source File
SOURCE=.\vidl_vil1_clip_sptr.h
# End Source File
# Begin Source File
SOURCE=.\vidl_vil1_codec.h
# End Source File
# Begin Source File
SOURCE=.\vidl_vil1_mpegcodec.h
# End Source File
# Begin Source File
SOURCE=.\vidl_vil1_mpegcodec_helper.h
# End Source File
# Begin Source File
SOURCE=.\vidl_vil1_yuv_2_rgb.h
# End Source File
# Begin Source File
SOURCE=.\vidl_vil1_codec_sptr.h
# End Source File
# Begin Source File
SOURCE=.\vidl_vil1_frame_as_image_sptr.h
# End Source File
# Begin Source File
SOURCE=.\vidl_vil1_frame_sptr.h
# End Source File
# Begin Source File
SOURCE=.\vidl_vil1_image_list_codec_sptr.h
# End Source File
# Begin Source File
SOURCE=.\vidl_vil1_movie_sptr.h
# End Source File
# Begin Source File
SOURCE=.\vidl_vil1_mpegcodec_sptr.h
# End Source File
# Begin Source File
SOURCE=.\vidl_vil1_file_sequence.h
# End Source File
# Begin Source File
SOURCE=.\vidl_vil1_vob_frame_index.h
# End Source File
# End Group
# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
