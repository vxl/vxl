# Microsoft Developer Studio Project File - Name="mil_algo" - Package Owner=<4>
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
# $(VXLROOT)/lib/ == override in output directory
# mil_algo  == name of output library

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=mil_algo - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "mil_algo.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "mil_algo.mak" CFG="mil_algo - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "mil_algo - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "mil_algo - Win32 Release" (based on "Win32 (x86) Static Library")
# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "mil_algo - Win32 Release"

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
# ADD CPP /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/vxl" /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/vcl/config.win32-VC60" /I "$(VXLROOT)/vxl" /I "$(VXLROOT)/mul"    -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE /D "mil_algo_EXPORTS"
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

!ELSEIF  "$(CFG)" == "mil_algo - Win32 Debug"

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
# ADD CPP /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/vxl" /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/vcl/config.win32-VC60" /I "$(VXLROOT)/vxl" /I "$(VXLROOT)/mul"    -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE /D "mil_algo_EXPORTS"
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

# Name "mil_algo - Win32 Release"
# Name "mil_algo - Win32 Debug"

# Begin Group "Source Files"
# PROP Default_Filter ""
# Begin Source File

SOURCE=.\mil_algo_exp_filter_1d.cxx

# End Source File
# Begin Source File

SOURCE=.\mil_algo_grad_1x3.cxx

# End Source File
# Begin Source File

SOURCE=.\mil_algo_grad_3x3.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/mil_algo_exp_filter_2d+float.float-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/mil_algo_exp_filter_2d+uchar.float-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/mil_algo_exp_filter_2d+uchar.uchar-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/mil_algo_line_filter+float-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/mil_algo_line_filter+uchar-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/mil_algo_grad_filter_2d+float.float-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/mil_algo_grad_filter_2d+uchar.float-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/mil_algo_gaussian_filter+float.float-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/mil_algo_gaussian_filter+vil_byte.float-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/mil_algo_brighten_image_2d+float.float.float-.cxx

# End Source File
# End Group
# Begin Group "Header Files"
# PROP Default_Filter ""
# Begin Source File

SOURCE=.\mil_algo_exp_filter_1d.h

# End Source File
# Begin Source File

SOURCE=.\mil_algo_exp_filter_2d.h

# End Source File
# Begin Source File

SOURCE=.\mil_algo_exp_filter_2d.txx

# End Source File
# Begin Source File

SOURCE=.\mil_algo_line_filter.h

# End Source File
# Begin Source File

SOURCE=.\mil_algo_line_filter.txx

# End Source File
# Begin Source File

SOURCE=.\mil_algo_grad_1x3.h

# End Source File
# Begin Source File

SOURCE=.\mil_algo_grad_3x3.h

# End Source File
# Begin Source File

SOURCE=.\mil_algo_grad_filter_2d.h

# End Source File
# Begin Source File

SOURCE=.\mil_algo_grad_filter_2d.txx

# End Source File
# Begin Source File

SOURCE=.\mil_algo_gaussian_filter.h

# End Source File
# Begin Source File

SOURCE=.\mil_algo_gaussian_filter.txx

# End Source File
# Begin Source File

SOURCE=.\mil_algo_brighten_image_2d.txx

# End Source File
# Begin Source File

SOURCE=.\mil_algo_brighten_image_2d.h

# End Source File
# End Group
# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
