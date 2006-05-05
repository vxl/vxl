# Microsoft Developer Studio Project File - Name="vil_algo" - Package Owner=<4>
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
# vil_algo  == name of output library

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=vil_algo - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE
!MESSAGE NMAKE /f "vil_algo.mak".
!MESSAGE
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE
!MESSAGE NMAKE /f "vil_algo.mak" CFG="vil_algo - Win32 Debug"
!MESSAGE
!MESSAGE Possible choices for configuration are:
!MESSAGE
!MESSAGE "vil_algo - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "vil_algo - Win32 Release" (based on "Win32 (x86) Static Library")
# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "vil_algo - Win32 Release"

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
# ADD CPP /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/core" /I "$(VXLROOT)/vcl/config.win32" -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE /D "vil_algo_EXPORTS"
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

!ELSEIF  "$(CFG)" == "vil_algo - Win32 Debug"

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
# ADD CPP /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/core" /I "$(VXLROOT)/vcl/config.win32" -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE /D "vil_algo_EXPORTS"
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

# Name "vil_algo - Win32 Release"
# Name "vil_algo - Win32 Debug"

# Begin Group "Source Files"
# PROP Default_Filter "c;cxx"
# Begin Source File
SOURCE=.\vil_sobel_1x3.cxx
# End Source File
# Begin Source File
SOURCE=.\vil_sobel_3x3.cxx
# End Source File
# Begin Source File
SOURCE=.\vil_gauss_reduce.cxx
# End Source File
# Begin Source File
SOURCE=.\vil_gauss_filter.cxx
# End Source File
# Begin Source File
SOURCE=.\vil_structuring_element.cxx
# End Source File
# Begin Source File
SOURCE=.\vil_binary_dilate.cxx
# End Source File
# Begin Source File
SOURCE=.\vil_binary_erode.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil_fft+double-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil_fft+float-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil_gauss_reduce+byte-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil_gauss_filter+byte.float-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil_gauss_filter+float.float-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil_gauss_reduce+float-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil_sobel_1x3+byte.float-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil_sobel_1x3+double.double-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil_sobel_1x3+float.float-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil_sobel_3x3+byte.float-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil_sobel_3x3+float.float-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil_greyscale_dilate+byte-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil_greyscale_dilate+float-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil_greyscale_erode+byte-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil_greyscale_erode+float-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil_median+byte-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil_median+float-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil_median+int-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil_median+bool-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil_sobel_3x3+double.double-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil_line_filter+byte-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil_threshold+byte-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil_threshold+float-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil_threshold+int-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil_gauss_filter+byte.byte-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil_cartesian_differential_invariants+float.float-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil_cartesian_differential_invariants+byte.float-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil_gauss_filter+byte.double-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil_sobel_1x3+byte.double-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil_sobel_3x3+byte.double-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil_greyscale_dilate+double-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil_greyscale_erode+double-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil_median+double-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil_histogram+double-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil_histogram+float-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil_histogram+short-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil_histogram+signed_char-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil_histogram+unsigned_short-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil_abs_shuffle_distance+byte.byte-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil_gauss_reduce+int_16-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil_suppress_non_max_edges+float.float-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil_checker_board+byte.byte-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil_suppress_non_max_edges+double.double-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil_gauss_filter+uint_16.uint_16-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil_threshold+uint_16-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil_gauss_filter+uint_16.uint_16-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil_gauss_reduce+double-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil_gauss_reduce+uint_16-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil_sobel_1x3+uint_16.double-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil_sobel_1x3+uint_16.float-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil_sobel_3x3+uint_16.double-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil_sobel_3x3+uint_16.float-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil_median+uint_16-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil_sobel_3x3+float.double-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil_gauss_filter+int_16.int_16-.cxx
# End Source File
# Begin Source File
SOURCE=.\vil_histogram.cxx
# End Source File
# Begin Source File
SOURCE=.\vil_histogram_equalise.cxx
# End Source File
# Begin Source File
SOURCE=.\vil_distance_transform.cxx
# End Source File
# Begin Source File
SOURCE=.\vil_blob_finder.cxx
# End Source File
# Begin Source File
SOURCE=.\vil_trace_4con_boundary.cxx
# End Source File
# Begin Source File
SOURCE=.\vil_trace_8con_boundary.cxx
# End Source File
# Begin Source File
SOURCE=.\vil_corners.cxx
# End Source File
# Begin Source File
SOURCE=.\vil_orientations.cxx
# End Source File
# Begin Source File
SOURCE=.\vil_colour_space.cxx
# End Source File
# End Group
# Begin Group "Header Files"
# PROP Default_Filter "h;txx"
# Begin Source File
SOURCE=.\vil_sobel_1x3.h
# End Source File
# Begin Source File
SOURCE=.\vil_sobel_1x3.txx
# End Source File
# Begin Source File
SOURCE=.\vil_sobel_3x3.h
# End Source File
# Begin Source File
SOURCE=.\vil_sobel_3x3.txx
# End Source File
# Begin Source File
SOURCE=.\vil_gauss_filter.h
# End Source File
# Begin Source File
SOURCE=.\vil_gauss_filter.txx
# End Source File
# Begin Source File
SOURCE=.\vil_gauss_reduce.h
# End Source File
# Begin Source File
SOURCE=.\vil_gauss_reduce.txx
# End Source File
# Begin Source File
SOURCE=.\vil_convolve_1d.h
# End Source File
# Begin Source File
SOURCE=.\vil_convolve_2d.h
# End Source File
# Begin Source File
SOURCE=.\vil_correlate_1d.h
# End Source File
# Begin Source File
SOURCE=.\vil_correlate_2d.h
# End Source File
# Begin Source File
SOURCE=.\vil_exp_filter_1d.h
# End Source File
# Begin Source File
SOURCE=.\vil_binary_erode.h
# End Source File
# Begin Source File
SOURCE=.\vil_binary_dilate.h
# End Source File
# Begin Source File
SOURCE=.\vil_structuring_element.h
# End Source File
# Begin Source File
SOURCE=.\vil_binary_closing.h
# End Source File
# Begin Source File
SOURCE=.\vil_binary_opening.h
# End Source File
# Begin Source File
SOURCE=.\vil_greyscale_closing.h
# End Source File
# Begin Source File
SOURCE=.\vil_greyscale_dilate.h
# End Source File
# Begin Source File
SOURCE=.\vil_greyscale_dilate.txx
# End Source File
# Begin Source File
SOURCE=.\vil_greyscale_erode.h
# End Source File
# Begin Source File
SOURCE=.\vil_greyscale_erode.txx
# End Source File
# Begin Source File
SOURCE=.\vil_greyscale_opening.h
# End Source File
# Begin Source File
SOURCE=.\vil_median.h
# End Source File
# Begin Source File
SOURCE=.\vil_median.txx
# End Source File
# Begin Source File
SOURCE=.\vil_normalised_correlation_2d.h
# End Source File
# Begin Source File
SOURCE=.\vil_exp_grad_filter_1d.h
# End Source File
# Begin Source File
SOURCE=.\vil_line_filter.h
# End Source File
# Begin Source File
SOURCE=.\vil_line_filter.txx
# End Source File
# Begin Source File
SOURCE=.\vil_threshold.h
# End Source File
# Begin Source File
SOURCE=.\vil_threshold.txx
# End Source File
# Begin Source File
SOURCE=.\vil_grid_merge.h
# End Source File
# Begin Source File
SOURCE=.\vil_find_4con_boundary.h
# End Source File
# Begin Source File
SOURCE=.\vil_find_peaks.h
# End Source File
# Begin Source File
SOURCE=.\vil_fft.h
# End Source File
# Begin Source File
SOURCE=.\vil_fft.txx
# End Source File
# Begin Source File
SOURCE=.\vil_exp_filter_2d.h
# End Source File
# Begin Source File
SOURCE=.\vil_suppress_non_max.h
# End Source File
# Begin Source File
SOURCE=.\vil_histogram.h
# End Source File
# Begin Source File
SOURCE=.\vil_histogram_equalise.h
# End Source File
# Begin Source File
SOURCE=.\vil_distance_transform.h
# End Source File
# Begin Source File
SOURCE=.\vil_blob_finder.h
# End Source File
# Begin Source File
SOURCE=.\vil_trace_4con_boundary.h
# End Source File
# Begin Source File
SOURCE=.\vil_trace_8con_boundary.h
# End Source File
# Begin Source File
SOURCE=.\vil_corners.h
# End Source File
# Begin Source File
SOURCE=.\vil_region_finder.txx
# End Source File
# Begin Source File
SOURCE=.\vil_region_finder.h
# End Source File
# Begin Source File
SOURCE=.\vil_cartesian_differential_invariants.h
# End Source File
# Begin Source File
SOURCE=.\vil_cartesian_differential_invariants.txx
# End Source File
# Begin Source File
SOURCE=.\vil_dog_filter_5tap.h
# End Source File
# Begin Source File
SOURCE=.\vil_dog_pyramid.h
# End Source File
# Begin Source File
SOURCE=.\vil_tile_images.h
# End Source File
# Begin Source File
SOURCE=.\vil_orientations.h
# End Source File
# Begin Source File
SOURCE=.\vil_abs_shuffle_distance.h
# End Source File
# Begin Source File
SOURCE=.\vil_abs_shuffle_distance.txx
# End Source File
# Begin Source File
SOURCE=.\vil_colour_space.h
# End Source File
# Begin Source File
SOURCE=.\vil_suppress_non_max_edges.h
# End Source File
# Begin Source File
SOURCE=.\vil_suppress_non_max_edges.txx
# End Source File
# Begin Source File
SOURCE=.\vil_checker_board.h
# End Source File
# Begin Source File
SOURCE=.\vil_checker_board.txx
# End Source File
# End Group
# End Target
# End Project
