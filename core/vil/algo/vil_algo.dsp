# Microsoft Developer Studio Project File - Name="vil2_algo" - Package Owner=<4>
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
# vil2_algo  == name of output library

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=vil2_algo - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "vil2_algo.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "vil2_algo.mak" CFG="vil2_algo - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "vil2_algo - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "vil2_algo - Win32 Release" (based on "Win32 (x86) Static Library")
# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "vil2_algo - Win32 Release"

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
# ADD CPP /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/core" /I "$(VXLROOT)/vcl/config.win32" -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE /D "vil2_algo_EXPORTS"
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

!ELSEIF  "$(CFG)" == "vil2_algo - Win32 Debug"

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
# ADD CPP /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/core" /I "$(VXLROOT)/vcl/config.win32" -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE /D "vil2_algo_EXPORTS"
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

# Name "vil2_algo - Win32 Release"
# Name "vil2_algo - Win32 Debug"

# Begin Group "Source Files"
# PROP Default_Filter ""
# Begin Source File
SOURCE=.\vil2_sobel_1x3.cxx
# End Source File
# Begin Source File
SOURCE=.\vil2_sobel_3x3.cxx
# End Source File
# Begin Source File
SOURCE=.\vil2_gauss_reduce.cxx
# End Source File
# Begin Source File
SOURCE=.\vil2_gauss_filter.cxx
# End Source File
# Begin Source File
SOURCE=.\vil2_structuring_element.cxx
# End Source File
# Begin Source File
SOURCE=.\vil2_binary_dilate.cxx
# End Source File
# Begin Source File
SOURCE=.\vil2_binary_erode.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil2_fft+double-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil2_fft+float-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil2_gauss_reduce+byte-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil2_gauss_filter+byte.float-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil2_gauss_filter+float.float-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil2_gauss_reduce+float-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil2_sobel_1x3+byte.float-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil2_sobel_1x3+double.double-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil2_sobel_1x3+float.float-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil2_sobel_3x3+byte.float-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil2_sobel_3x3+float.float-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil2_greyscale_dilate+byte-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil2_greyscale_dilate+float-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil2_greyscale_erode+byte-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil2_greyscale_erode+float-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil2_median+byte-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil2_median+float-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil2_sobel_3x3+double.double-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil2_line_filter+byte-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil2_threshold+byte-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil2_threshold+float-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil2_threshold+int-.cxx
# End Source File
# End Group
# Begin Group "Header Files"
# PROP Default_Filter ""
# Begin Source File
SOURCE=.\vil2_sobel_1x3.h
# End Source File
# Begin Source File
SOURCE=.\vil2_sobel_1x3.txx
# End Source File
# Begin Source File
SOURCE=.\vil2_sobel_3x3.h
# End Source File
# Begin Source File
SOURCE=.\vil2_sobel_3x3.txx
# End Source File
# Begin Source File
SOURCE=.\vil2_gauss_filter.h
# End Source File
# Begin Source File
SOURCE=.\vil2_gauss_filter.txx
# End Source File
# Begin Source File
SOURCE=.\vil2_gauss_reduce.h
# End Source File
# Begin Source File
SOURCE=.\vil2_gauss_reduce.txx
# End Source File
# Begin Source File
SOURCE=.\vil2_convolve_1d.h
# End Source File
# Begin Source File
SOURCE=.\vil2_convolve_2d.h
# End Source File
# Begin Source File
SOURCE=.\vil2_correlate_1d.h
# End Source File
# Begin Source File
SOURCE=.\vil2_correlate_2d.h
# End Source File
# Begin Source File
SOURCE=.\vil2_exp_filter_1d.h
# End Source File
# Begin Source File
SOURCE=.\vil2_binary_erode.h
# End Source File
# Begin Source File
SOURCE=.\vil2_binary_dilate.h
# End Source File
# Begin Source File
SOURCE=.\vil2_structuring_element.h
# End Source File
# Begin Source File
SOURCE=.\vil2_binary_closing.h
# End Source File
# Begin Source File
SOURCE=.\vil2_binary_opening.h
# End Source File
# Begin Source File
SOURCE=.\vil2_greyscale_closing.h
# End Source File
# Begin Source File
SOURCE=.\vil2_greyscale_dilate.h
# End Source File
# Begin Source File
SOURCE=.\vil2_greyscale_dilate.txx
# End Source File
# Begin Source File
SOURCE=.\vil2_greyscale_erode.h
# End Source File
# Begin Source File
SOURCE=.\vil2_greyscale_erode.txx
# End Source File
# Begin Source File
SOURCE=.\vil2_greyscale_opening.h
# End Source File
# Begin Source File
SOURCE=.\vil2_median.h
# End Source File
# Begin Source File
SOURCE=.\vil2_median.txx
# End Source File
# Begin Source File
SOURCE=.\vil2_normalised_correlation_2d.h
# End Source File
# Begin Source File
SOURCE=.\vil2_exp_grad_filter_1d.h
# End Source File
# Begin Source File
SOURCE=.\vil2_line_filter.h
# End Source File
# Begin Source File
SOURCE=.\vil2_line_filter.txx
# End Source File
# Begin Source File
SOURCE=.\vil2_threshold.h
# End Source File
# Begin Source File
SOURCE=.\vil2_threshold.txx
# End Source File
# Begin Source File
SOURCE=.\vil2_grid_merge.h
# End Source File
# Begin Source File
SOURCE=.\vil2_find_4con_boundary.h
# End Source File
# Begin Source File
SOURCE=.\vil2_fft.h
# End Source File
# Begin Source File
SOURCE=.\vil2_fft.txx
# End Source File
# End Group
# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
