# Microsoft Developer Studio Project File - Name="vpro" - Package Owner=<4>
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
# vpro  == name of output library

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=vpro - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "vpro.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "vpro.mak" CFG="vil2 - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "vpro - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "vpro - Win32 Release" (based on "Win32 (x86) Static Library")
# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "vpro - Win32 Release"

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
# ADD CPP /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/core" /I "$(VXLROOT)/vcl/config.win32" /I "$(VXLROOT)/contrib/mul" -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE /D "vpro_EXPORTS"
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

!ELSEIF  "$(CFG)" == "vpro - Win32 Debug"

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
# ADD CPP /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/core" /I "$(VXLROOT)/vcl/config.win32" /I "$(VXLROOT)/contrib/mul" -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE /D "vpro_EXPORTS"
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

# Name "vpro - Win32 Release"
# Name "vpro - Win32 Debug"

# Begin Group "Source Files"
# PROP Default_Filter "cxx;c"
# Begin Source File
SOURCE=.\Templates/vbl_smart_ptr+vpro_video_process-.cxx
# End Source File
# Begin Source File
SOURCE=.\vpro_capture_process.cxx
# End Source File
# Begin Source File
SOURCE=.\vpro_curve_tracking_process.cxx
# End Source File
# Begin Source File
SOURCE=.\vpro_edge_line_process.cxx
# End Source File
# Begin Source File
SOURCE=.\vpro_edge_process.cxx
# End Source File
# Begin Source File
SOURCE=.\vpro_epipolar_space_process.cxx
# End Source File
# Begin Source File
SOURCE=.\vpro_frame_diff_process.cxx
# End Source File
# Begin Source File
SOURCE=.\vpro_grid_finder_process.cxx
# End Source File
# Begin Source File
SOURCE=.\vpro_harris_corner_process.cxx
# End Source File
# Begin Source File
SOURCE=.\vpro_line_fit_process.cxx
# End Source File
# Begin Source File
SOURCE=.\vpro_lucas_kanade_process.cxx
# End Source File
# Begin Source File
SOURCE=.\vpro_motion_process.cxx
# End Source File
# Begin Source File
SOURCE=.\vpro_region_process.cxx
# End Source File
# Begin Source File
SOURCE=.\vpro_video_process.cxx
# End Source File
# End Group
# Begin Group "Header Files"
# PROP Default_Filter "h;txx"
# Begin Source File
SOURCE=.\vpro_capture_process.h
# End Source File
# Begin Source File
SOURCE=.\vpro_curve_tracking_process.h
# End Source File
# Begin Source File
SOURCE=.\vpro_edge_line_process.h
# End Source File
# Begin Source File
SOURCE=.\vpro_edge_process.h
# End Source File
# Begin Source File
SOURCE=.\vpro_epipolar_space_process.h
# End Source File
# Begin Source File
SOURCE=.\vpro_frame_diff_process.h
# End Source File
# Begin Source File
SOURCE=.\vpro_grid_finder_process.h
# End Source File
# Begin Source File
SOURCE=.\vpro_harris_corner_process.h
# End Source File
# Begin Source File
SOURCE=.\vpro_line_fit_process.h
# End Source File
# Begin Source File
SOURCE=.\vpro_lucas_kanade_process.h
# End Source File
# Begin Source File
SOURCE=.\vpro_motion_process.h
# End Source File
# Begin Source File
SOURCE=.\vpro_region_process.h
# End Source File
# Begin Source File
SOURCE=.\vpro_video_process.h
# End Source File
# Begin Source File
SOURCE=.\vpro_video_process_sptr.h
# End Source File
# End Group
# End Target
# End Project
