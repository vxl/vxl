# Microsoft Developer Studio Project File - Name="vsrl" - Package Owner=<4>
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
# vsrl  == name of output library

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=vsrl - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "vsrl.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "vsrl.mak" CFG="vsrl - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "vsrl - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "vsrl - Win32 Release" (based on "Win32 (x86) Static Library")
# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "vsrl - Win32 Release"

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
# ADD CPP /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/core" /I "$(VXLROOT)/vcl/config.win32" /I "$(VXLROOT)/contrib/gel" -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE -DHAS_OPENGL /D "vsrl_EXPORTS"
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

!ELSEIF  "$(CFG)" == "vsrl - Win32 Debug"

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
# ADD CPP /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/core" /I "$(VXLROOT)/vcl/config.win32" /I "$(VXLROOT)/contrib/gel" -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE -DHAS_OPENGL /D "vsrl_EXPORTS"
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

# Name "vsrl - Win32 Release"
# Name "vsrl - Win32 Debug"

# Begin Group "Source Files"
# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Templates/vcl_vector+vsrl_intensity_token~-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_vector+vsrl_token~-.cxx

# End Source File
# Begin Source File

SOURCE=.\vsrl_3d_output.cxx

# End Source File
# Begin Source File

SOURCE=.\vsrl_dense_matcher.cxx

# End Source File
# Begin Source File

SOURCE=.\vsrl_diffusion.cxx

# End Source File
# Begin Source File

SOURCE=.\vsrl_dynamic_program.cxx

# End Source File
# Begin Source File

SOURCE=.\vsrl_image_correlation.cxx

# End Source File
# Begin Source File

SOURCE=.\vsrl_intensity_token.cxx

# End Source File
# Begin Source File

SOURCE=.\vsrl_null_token.cxx

# End Source File
# Begin Source File

SOURCE=.\vsrl_parameters.cxx

# End Source File
# Begin Source File

SOURCE=.\vsrl_raster_dp_setup.cxx

# End Source File
# Begin Source File

SOURCE=.\vsrl_results_dense_matcher.cxx

# End Source File
# Begin Source File

SOURCE=.\vsrl_saliency_diffusion.cxx

# End Source File
# Begin Source File

SOURCE=.\vsrl_step_diffusion.cxx

# End Source File
# Begin Source File

SOURCE=.\vsrl_stereo_dense_matcher.cxx

# End Source File
# Begin Source File

SOURCE=.\vsrl_token.cxx

# End Source File
# Begin Source File

SOURCE=.\vsrl_token_saliency.cxx

# End Source File
# Begin Source File

SOURCE=.\vsrl_window_accumulator.cxx

# End Source File
# End Group
# Begin Group "Header Files"
# PROP Default_Filter ""
# Begin Source File

SOURCE=.\vsrl_3d_output.h

# End Source File
# Begin Source File

SOURCE=.\vsrl_dense_matcher.h

# End Source File
# Begin Source File

SOURCE=.\vsrl_diffusion.h

# End Source File
# Begin Source File

SOURCE=.\vsrl_dynamic_program.h

# End Source File
# Begin Source File

SOURCE=.\vsrl_image_correlation.h

# End Source File
# Begin Source File

SOURCE=.\vsrl_intensity_token.h

# End Source File
# Begin Source File

SOURCE=.\vsrl_null_token.h

# End Source File
# Begin Source File

SOURCE=.\vsrl_parameters.h

# End Source File
# Begin Source File

SOURCE=.\vsrl_raster_dp_setup.h

# End Source File
# Begin Source File

SOURCE=.\vsrl_results_dense_matcher.h

# End Source File
# Begin Source File

SOURCE=.\vsrl_saliency_diffusion.h

# End Source File
# Begin Source File

SOURCE=.\vsrl_step_diffusion.h

# End Source File
# Begin Source File

SOURCE=.\vsrl_stereo_dense_matcher.h

# End Source File
# Begin Source File

SOURCE=.\vsrl_token.h

# End Source File
# Begin Source File

SOURCE=.\vsrl_token_saliency.h

# End Source File
# Begin Source File

SOURCE=.\vsrl_window_accumulator.h

# End Source File
# End Group
# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project

