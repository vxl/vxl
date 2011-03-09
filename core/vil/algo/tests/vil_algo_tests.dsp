# Microsoft Developer Studio Project File - Name="vil_algo_tests" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# variables to REPLACE
#
# /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/core" /I "$(VXLROOT)/vcl/config.win32" == include path
#   -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE -DHAS_OPENGL == compiler defines
#  == override in output directory
# vil_algo_tests  == name of output library

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=vil_algo_tests - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE
!MESSAGE NMAKE /f "vil_algo_tests.mak".
!MESSAGE
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE
!MESSAGE NMAKE /f "vil_algo_tests.mak" CFG="vil_algo_tests - Win32 Debug"
!MESSAGE
!MESSAGE Possible choices for configuration are:
!MESSAGE
!MESSAGE "vil_algo_tests - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "vil_algo_tests - Win32 Release" (based on "Win32 (x86) Static Library")
# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "vil_algo_tests - Win32 Release"

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
# ADD CPP /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/core" /I "$(VXLROOT)/vcl/config.win32" -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE -DHAS_OPENGL /D "vil_algo_tests_EXPORTS"
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

!ELSEIF  "$(CFG)" == "vil_algo_tests - Win32 Debug"

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
# ADD CPP /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/core" /I "$(VXLROOT)/vcl/config.win32" -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE -DHAS_OPENGL /D "vil_algo_tests_EXPORTS"
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

# Name "vil_algo_tests - Win32 Release"
# Name "vil_algo_tests - Win32 Debug"

# Begin Group "Source Files"
# PROP Default_Filter "cxx"
# Begin Source File
SOURCE=.\test_driver.cxx
# End Source File
# Begin Source File
SOURCE=.\test_algo_gauss_reduce.cxx
# End Source File
# Begin Source File
SOURCE=.\test_algo_convolve_1d.cxx
# End Source File
# Begin Source File
SOURCE=.\test_algo_convolve_2d.cxx
# End Source File
# Begin Source File
SOURCE=.\test_algo_exp_filter_1d.cxx
# End Source File
# Begin Source File
SOURCE=.\test_binary_dilate.cxx
# End Source File
# Begin Source File
SOURCE=.\test_binary_erode.cxx
# End Source File
# Begin Source File
SOURCE=.\test_greyscale_dilate.cxx
# End Source File
# Begin Source File
SOURCE=.\test_greyscale_erode.cxx
# End Source File
# Begin Source File
SOURCE=.\test_median.cxx
# End Source File
# Begin Source File
SOURCE=.\test_algo_exp_grad_filter_1d.cxx
# End Source File
# Begin Source File
SOURCE=.\test_algo_gauss_filter.cxx
# End Source File
# Begin Source File
SOURCE=.\test_algo_line_filter.cxx
# End Source File
# Begin Source File
SOURCE=.\test_algo_correlate_1d.cxx
# End Source File
# Begin Source File
SOURCE=.\test_algo_correlate_2d.cxx
# End Source File
# Begin Source File
SOURCE=.\test_algo_threshold.cxx
# End Source File
# Begin Source File
SOURCE=.\test_algo_grid_merge.cxx
# End Source File
# Begin Source File
SOURCE=.\test_algo_find_4con_boundary.cxx
# End Source File
# Begin Source File
SOURCE=.\test_algo_fft.cxx
# End Source File
# Begin Source File
SOURCE=.\test_suppress_non_max.cxx
# End Source File
# Begin Source File
SOURCE=.\test_algo_histogram.cxx
# End Source File
# Begin Source File
SOURCE=.\test_algo_histogram_equalise.cxx
# End Source File
# Begin Source File
SOURCE=.\test_algo_distance_transform.cxx
# End Source File
# Begin Source File
SOURCE=.\test_algo_trace_4con_boundary.cxx
# End Source File
# Begin Source File
SOURCE=.\test_algo_trace_8con_boundary.cxx
# End Source File
# Begin Source File
SOURCE=.\test_algo_blob.cxx
# End Source File
# Begin Source File
SOURCE=.\test_algo_find_peaks.cxx
# End Source File
# Begin Source File
SOURCE=.\test_algo_region_finder.cxx
# End Source File
# Begin Source File
SOURCE=.\test_algo_cartesian_differential_invariants.cxx
# End Source File
# Begin Source File
SOURCE=.\test_algo_corners.cxx
# End Source File
# Begin Source File
SOURCE=.\test_algo_sobel.cxx
# End Source File
# Begin Source File
SOURCE=.\test_algo_abs_shuffle_distance.cxx
# End Source File
# Begin Source File
SOURCE=.\test_algo_suppress_non_max_edges.cxx
# End Source File
# Begin Source File
SOURCE=.\test_algo_checker_board.cxx
# End Source File
# Begin Source File
SOURCE=.\test_algo_colour_space.cxx
# End Source File
# Begin Source File
SOURCE=.\test_algo_quad_distance_function.cxx
# End Source File
# Begin Source File
SOURCE=.\test_algo_find_plateaus.cxx
# End Source File
# Begin Source File
SOURCE=.\test_algo_suppress_non_plateau.cxx
# End Source File
# Begin Source File
SOURCE=.\test_algo_flood_fill.cxx
# End Source File
# Begin Source File
SOURCE=.\test_include.cxx
# End Source File
# Begin Source File
SOURCE=.\test_template_include.cxx
# End Source File
# End Group
# End Target
# End Project
