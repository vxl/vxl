# Microsoft Developer Studio Project File - Name="strk" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# vxl DSP Header file
# This file is read by the build system of vxl, and is used as the top part of
# a microsoft project dsp header file
# If this is in a dsp file, then it is not the header, but has
# already been used, so do not edit here...

# variables to REPLACE
# 
# /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/core" /I "$(VXLROOT)/vcl/config.win32" /I "$(VXLROOT)/contrib/mul" == include path
#   -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE == compiler defines
#  == override in output directory
# strk  == name of output library

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=strk - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "strk.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "strk.mak" CFG="vil2 - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "strk - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "strk - Win32 Release" (based on "Win32 (x86) Static Library")
# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "strk - Win32 Release"

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
# ADD CPP /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/core" /I "$(VXLROOT)/vcl/config.win32" /I "$(VXLROOT)/contrib/mul" -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE /D "strk_EXPORTS"
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

!ELSEIF  "$(CFG)" == "strk - Win32 Debug"

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
# ADD CPP /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/core" /I "$(VXLROOT)/vcl/config.win32" /I "$(VXLROOT)/contrib/mul" -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE /D "strk_EXPORTS"
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

# Name "strk - Win32 Release"
# Name "strk - Win32 Debug"

# Begin Group "Source Files"
# PROP Default_Filter "cxx;c"
# Begin Source File
SOURCE=.\Templates/vbl_smart_ptr+strk_art_info_model-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vbl_smart_ptr+strk_tracking_face_2d-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vbl_smart_ptr+strk_epi_point-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vbl_smart_ptr+strk_epi_seg-.cxx
# End Source File
# Begin Source File
SOURCE=.\strk_art_model_display_process.cxx
# End Source File
# Begin Source File
SOURCE=.\strk_art_info_model.cxx
# End Source File
# Begin Source File
SOURCE=.\strk_corr_tracker_process.cxx
# End Source File
# Begin Source File
SOURCE=.\strk_info_model_tracker.cxx
# End Source File
# Begin Source File
SOURCE=.\strk_info_model_tracker_params.cxx
# End Source File
# Begin Source File
SOURCE=.\strk_info_model_tracker_process.cxx
# End Source File
# Begin Source File
SOURCE=.\strk_info_tracker.cxx
# End Source File
# Begin Source File
SOURCE=.\strk_info_tracker_params.cxx
# End Source File
# Begin Source File
SOURCE=.\strk_info_tracker_process.cxx
# End Source File
# Begin Source File
SOURCE=.\strk_track_display_process.cxx
# End Source File
# Begin Source File
SOURCE=.\strk_tracker.cxx
# End Source File
# Begin Source File
SOURCE=.\strk_tracker_params.cxx
# End Source File
# Begin Source File
SOURCE=.\strk_tracking_face_2d.cxx
# End Source File
# Begin Source File
SOURCE=.\strk_epi_point.cxx
# End Source File
# Begin Source File
SOURCE=.\strk_epi_seg.cxx
# End Source File
# Begin Source File
SOURCE=.\strk_epipolar_grouper.cxx
# End Source File
# Begin Source File
SOURCE=.\strk_epipolar_grouper_params.cxx
# End Source File
# Begin Source File
SOURCE=.\strk_epipolar_grouper_process.cxx
# End Source File
# Begin Source File
SOURCE=.\strk_region_info.cxx
# End Source File
# Begin Source File
SOURCE=.\strk_region_info_params.cxx
# End Source File
# End Group
# Begin Group "Header Files"
# PROP Default_Filter "h;txx"
# Begin Source File
SOURCE=.\strk_art_info_model.h
# End Source File
# Begin Source File
SOURCE=.\strk_art_info_model_sptr.h
# End Source File
# Begin Source File
SOURCE=.\strk_art_model_display_process.h
# End Source File
# Begin Source File
SOURCE=.\strk_corr_tracker_process.h
# End Source File
# Begin Source File
SOURCE=.\strk_info_model_tracker.h
# End Source File
# Begin Source File
SOURCE=.\strk_info_model_tracker_params.h
# End Source File
# Begin Source File
SOURCE=.\strk_info_model_tracker_process.h
# End Source File
# Begin Source File
SOURCE=.\strk_info_tracker.h
# End Source File
# Begin Source File
SOURCE=.\strk_info_tracker_params.h
# End Source File
# Begin Source File
SOURCE=.\strk_info_tracker_process.h
# End Source File
# Begin Source File
SOURCE=.\strk_track_display_process.h
# End Source File
# Begin Source File
SOURCE=.\strk_tracker.h
# End Source File
# Begin Source File
SOURCE=.\strk_tracker_params.h
# End Source File
# Begin Source File
SOURCE=.\strk_tracking_face_2d.h
# End Source File
# Begin Source File
SOURCE=.\strk_tracking_face_2d_sptr.h
# End Source File
# Begin Source File
SOURCE=.\strk_epi_point.h
# End Source File
# Begin Source File
SOURCE=.\strk_epi_point_sptr.h
# End Source File
# Begin Source File
SOURCE=.\strk_epi_seg.h
# End Source File
# Begin Source File
SOURCE=.\strk_epi_seg_sptr.h
# End Source File
# Begin Source File
SOURCE=.\strk_epipolar_grouper.h
# End Source File
# Begin Source File
SOURCE=.\strk_epipolar_grouper_params.h
# End Source File
# Begin Source File
SOURCE=.\strk_epipolar_grouper_process.h
# End Source File
# Begin Source File
SOURCE=.\strk_region_info.h
# End Source File
# Begin Source File
SOURCE=.\strk_region_info_params.h
# End Source File
# End Group
# End Target
# End Project
