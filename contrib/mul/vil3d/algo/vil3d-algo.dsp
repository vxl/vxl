# Microsoft Developer Studio Project File - Name="vil3d_algo" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# variables to REPLACE
# 
# /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/core" /I "$(VXLROOT)/vcl/config.win32" /I "$(VXLROOT)/contrib/mul" == include path
#   -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE == compiler defines
#  == override in output directory
# vil3d_algo  == name of output library

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=vil3d_algo - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "vil3d_algo.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "vil3d_algo.mak" CFG="vil3d_algo - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "vil3d_algo - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "vil3d_algo - Win32 Release" (based on "Win32 (x86) Static Library")
# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "vil3d_algo - Win32 Release"

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
# ADD CPP /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/core" /I "$(VXLROOT)/vcl/config.win32" /I "$(VXLROOT)/contrib/mul" -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE /D "vil3d_algo_EXPORTS"
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

!ELSEIF  "$(CFG)" == "vil3d_algo - Win32 Debug"

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
# ADD CPP /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/core" /I "$(VXLROOT)/vcl/config.win32" /I "$(VXLROOT)/contrib/mul" -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE /D "vil3d_algo_EXPORTS"
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

# Name "vil3d_algo - Win32 Release"
# Name "vil3d_algo - Win32 Debug"

# Begin Group "Source Files"
# PROP Default_Filter "cxx"
# Begin Source File
SOURCE=.\vil3d_grad_1x3.cxx
# End Source File
# Begin Source File
SOURCE=.\vil3d_binary_dilate.cxx
# End Source File
# Begin Source File
SOURCE=.\vil3d_binary_erode.cxx
# End Source File
# Begin Source File
SOURCE=.\vil3d_structuring_element.cxx
# End Source File
# Begin Source File
SOURCE=.\vil3d_histogram.cxx
# End Source File
# Begin Source File
SOURCE=.\vil3d_histogram_equalise.cxx
# End Source File
# Begin Source File
SOURCE=.\vil3d_distance_transform.cxx
# End Source File
# Begin Source File
SOURCE=.\vil3d_fill_boundary.cxx
# End Source File
# Begin Source File
SOURCE=.\vil3d_make_distance_filter.cxx
# End Source File
# Begin Source File
SOURCE=.\vil3d_make_edt_filter.cxx
# End Source File
# Begin Source File
SOURCE=.\vil3d_overlap.cxx
# End Source File
# Begin Source File
SOURCE=.\vil3d_find_blobs.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil3d_grad_1x3+byte.float-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil3d_grad_1x3+float.float-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil3d_threshold+byte-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil3d_threshold+float-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil3d_threshold+int-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil3d_grad_1x3+int_32.float-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil3d_gauss_reduce+byte-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil3d_gauss_reduce+float-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil3d_gauss_reduce+int-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil3d_gauss_reduce+int_16-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil3d_corners+byte.float-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil3d_corners+float.float-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil3d_corners+int_16.float-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil3d_abs_shuffle_distance+int.int-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil3d_abs_shuffle_distance+int_16.int_16-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil3d_gauss_reduce+double-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil3d_abs_shuffle_distance+float.float-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil3d_suppress_non_max_edges+float-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil3d_world_gradients+float.float-.cxx
# End Source File
# End Group
# Begin Group "Header Files"
# PROP Default_Filter "h;txx"
# Begin Source File
SOURCE=.\vil3d_gauss_reduce.h
# End Source File
# Begin Source File
SOURCE=.\vil3d_grad_1x3.h
# End Source File
# Begin Source File
SOURCE=.\vil3d_grad_1x3.txx
# End Source File
# Begin Source File
SOURCE=.\vil3d_threshold.h
# End Source File
# Begin Source File
SOURCE=.\vil3d_threshold.txx
# End Source File
# Begin Source File
SOURCE=.\vil3d_binary_closing.h
# End Source File
# Begin Source File
SOURCE=.\vil3d_binary_dilate.h
# End Source File
# Begin Source File
SOURCE=.\vil3d_binary_erode.h
# End Source File
# Begin Source File
SOURCE=.\vil3d_binary_opening.h
# End Source File
# Begin Source File
SOURCE=.\vil3d_structuring_element.h
# End Source File
# Begin Source File
SOURCE=.\vil3d_exp_filter.h
# End Source File
# Begin Source File
SOURCE=.\vil3d_normalised_correlation_3d.h
# End Source File
# Begin Source File
SOURCE=.\vil3d_convolve_1d.h
# End Source File
# Begin Source File
SOURCE=.\vil3d_histogram.h
# End Source File
# Begin Source File
SOURCE=.\vil3d_histogram_equalise.h
# End Source File
# Begin Source File
SOURCE=.\vil3d_distance_transform.h
# End Source File
# Begin Source File
SOURCE=.\vil3d_fill_boundary.h
# End Source File
# Begin Source File
SOURCE=.\vil3d_anisotropic_filter.h
# End Source File
# Begin Source File
SOURCE=.\vil3d_gauss_reduce.txx
# End Source File
# Begin Source File
SOURCE=.\vil3d_corners.h
# End Source File
# Begin Source File
SOURCE=.\vil3d_corners.txx
# End Source File
# Begin Source File
SOURCE=.\vil3d_find_peaks.h
# End Source File
# Begin Source File
SOURCE=.\vil3d_grad_3x3x3.h
# End Source File
# Begin Source File
SOURCE=.\vil3d_quad_distance_function.h
# End Source File
# Begin Source File
SOURCE=.\vil3d_smooth_121.h
# End Source File
# Begin Source File
SOURCE=.\vil3d_abs_shuffle_distance.h
# End Source File
# Begin Source File
SOURCE=.\vil3d_abs_shuffle_distance.txx
# End Source File
# Begin Source File
SOURCE=.\vil3d_exp_distance_transform.h
# End Source File
# Begin Source File
SOURCE=.\vil3d_make_distance_filter.h
# End Source File
# Begin Source File
SOURCE=.\vil3d_make_edt_filter.h
# End Source File
# Begin Source File
SOURCE=.\vil3d_max_product_filter.h
# End Source File
# Begin Source File
SOURCE=.\vil3d_overlap.h
# End Source File
# Begin Source File
SOURCE=.\vil3d_locally_z_normalise.h
# End Source File
# Begin Source File
SOURCE=.\vil3d_find_blobs.h
# End Source File
# Begin Source File
SOURCE=.\vil3d_erode.h
# End Source File
# Begin Source File
SOURCE=.\vil3d_suppress_non_max_edges.h
# End Source File
# Begin Source File
SOURCE=.\vil3d_world_gradients.txx
# End Source File
# Begin Source File
SOURCE=.\vil3d_world_gradients.h
# End Source File
# Begin Source File
SOURCE=.\vil3d_suppress_non_max_edges.txx
# End Source File
# Begin Source File
SOURCE=.\vil3d_fill_border.h
# End Source File
# End Group
# End Target
# End Project
