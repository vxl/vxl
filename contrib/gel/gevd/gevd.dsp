# Microsoft Developer Studio Project File - Name="gevd" - Package Owner=<4>
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
# gevd  == name of output library

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=gevd - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "gevd.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "gevd.mak" CFG="gevd - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "gevd - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "gevd - Win32 Release" (based on "Win32 (x86) Static Library")
# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "gevd - Win32 Release"

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
# ADD CPP /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/core" /I "$(VXLROOT)/vcl/config.win32" /I "$(VXLROOT)/contrib/gel" -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE -DHAS_OPENGL /D "gevd_EXPORTS"
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

!ELSEIF  "$(CFG)" == "gevd - Win32 Debug"

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
# ADD CPP /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/core" /I "$(VXLROOT)/vcl/config.win32" /I "$(VXLROOT)/contrib/gel" -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE -DHAS_OPENGL /D "gevd_EXPORTS"
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

# Name "gevd - Win32 Release"
# Name "gevd - Win32 Debug"

# Begin Group "Source Files"
# PROP Default_Filter ""
# Begin Source File

SOURCE=.\gevd_step.cxx

# End Source File
# Begin Source File

SOURCE=.\gevd_status_mixin.cxx

# End Source File
# Begin Source File

SOURCE=.\gevd_param_mixin.cxx

# End Source File
# Begin Source File

SOURCE=.\gevd_noise.cxx

# End Source File
# Begin Source File

SOURCE=.\gevd_memory_mixin.cxx

# End Source File
# Begin Source File

SOURCE=.\gevd_float_operators.cxx

# End Source File
# Begin Source File

SOURCE=.\gevd_detector_params.cxx

# End Source File
# Begin Source File

SOURCE=.\gevd_detector.cxx

# End Source File
# Begin Source File

SOURCE=.\gevd_contour.cxx

# End Source File
# Begin Source File

SOURCE=.\gevd_bufferxy.cxx

# End Source File
# Begin Source File

SOURCE=.\gevd_clean_edgels.cxx

# End Source File
# Begin Source File

SOURCE=.\gevd_edgel_regions.cxx

# End Source File
# Begin Source File

SOURCE=.\gevd_region_edge.cxx

# End Source File
# Begin Source File

SOURCE=.\gevd_region_proc.cxx

# End Source File
# Begin Source File

SOURCE=.\gevd_region_proc_params.cxx

# End Source File
# End Group
# Begin Group "Header Files"
# PROP Default_Filter ""
# Begin Source File

SOURCE=.\gevd_bufferxy.h

# End Source File
# Begin Source File

SOURCE=.\gevd_contour.h

# End Source File
# Begin Source File

SOURCE=.\gevd_detector.h

# End Source File
# Begin Source File

SOURCE=.\gevd_detector_params.h

# End Source File
# Begin Source File

SOURCE=.\gevd_float_operators.h

# End Source File
# Begin Source File

SOURCE=.\gevd_memory_mixin.h

# End Source File
# Begin Source File

SOURCE=.\gevd_noise.h

# End Source File
# Begin Source File

SOURCE=.\gevd_param_mixin.h

# End Source File
# Begin Source File

SOURCE=.\gevd_status_mixin.h

# End Source File
# Begin Source File

SOURCE=.\gevd_step.h

# End Source File
# Begin Source File

SOURCE=.\gevd_pixel.h

# End Source File
# Begin Source File

SOURCE=.\gevd_xpixel.h

# End Source File
# Begin Source File

SOURCE=.\gevd_clean_edgels.h

# End Source File
# Begin Source File

SOURCE=.\gevd_edgel_regions.h

# End Source File
# Begin Source File

SOURCE=.\gevd_region_edge.h

# End Source File
# Begin Source File

SOURCE=.\gevd_region_proc.h

# End Source File
# Begin Source File

SOURCE=.\gevd_region_proc_params.h

# End Source File
# End Group
# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
