# Microsoft Developer Studio Project File - Name="vgl_algo" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# variables to REPLACE
# 
# /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/core" /I "$(VXLROOT)/vcl/config.win32" == include path
#   -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE == compiler defines
#  == override in output directory
# vgl_algo  == name of output library

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=vgl_algo - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "vgl_algo.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "vgl_algo.mak" CFG="vgl_algo - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "vgl_algo - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "vgl_algo - Win32 Release" (based on "Win32 (x86) Static Library")
# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "vgl_algo - Win32 Release"

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
# ADD CPP /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/core" /I "$(VXLROOT)/vcl/config.win32" -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE /D "vgl_algo_EXPORTS"
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

!ELSEIF  "$(CFG)" == "vgl_algo - Win32 Debug"

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
# ADD CPP /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/core" /I "$(VXLROOT)/vcl/config.win32" -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE /D "vgl_algo_EXPORTS"
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
# Name "vgl_algo - Win32 Release"
# Name "vgl_algo - Win32 Debug"

# Begin Group "Source Files"
# PROP Default_Filter "cxx"
# Begin Source File
SOURCE=.\Templates/vgl_homg_operators_1d+double-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vgl_homg_operators_1d+float-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vgl_homg_operators_2d+double-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vgl_homg_operators_2d+float-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vgl_homg_operators_3d+double-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vgl_homg_operators_3d+float-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vgl_h_matrix_1d+double-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vgl_h_matrix_2d+double-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vgl_h_matrix_3d+double-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vgl_norm_trans_2d+double-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vgl_fit_lines_2d+double-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vgl_line_2d_regression+double-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vgl_p_matrix+double-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vgl_convex_hull_2d+double-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vgl_norm_trans_3d+double-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vgl_h_matrix_1d+float-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vgl_h_matrix_2d+float-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vgl_h_matrix_3d+float-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vgl_conic_2d_regression+double-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vgl_conic_2d_regression+float-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vgl_fit_conics_2d+double-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vgl_fit_conics_2d+float-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vgl_norm_trans_2d+float-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vgl_orient_box_3d+double-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vgl_orient_box_3d+float-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vgl_fit_plane_3d+double-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vgl_algo_intersection+double-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vgl_algo_intersection+float-.cxx
# End Source File
# Begin Source File
SOURCE=.\vgl_h_matrix_1d_compute_3point.cxx
# End Source File
# Begin Source File
SOURCE=.\vgl_h_matrix_1d_compute_linear.cxx
# End Source File
# Begin Source File
SOURCE=.\vgl_h_matrix_1d_compute_optimize.cxx
# End Source File
# Begin Source File
SOURCE=.\vgl_h_matrix_2d_compute_4point.cxx
# End Source File
# Begin Source File
SOURCE=.\vgl_h_matrix_2d_compute_linear.cxx
# End Source File
# Begin Source File
SOURCE=.\vgl_h_matrix_2d_optimize_lmq.cxx
# End Source File
# End Group
# Begin Group "Header Files"
# PROP Default_Filter "h;txx"
# Begin Source File
SOURCE=.\vgl_algo_fwd.h
# End Source File
# Begin Source File
SOURCE=.\vgl_homg_operators_1d.h
# End Source File
# Begin Source File
SOURCE=.\vgl_homg_operators_1d.txx
# End Source File
# Begin Source File
SOURCE=.\vgl_homg_operators_2d.h
# End Source File
# Begin Source File
SOURCE=.\vgl_homg_operators_2d.txx
# End Source File
# Begin Source File
SOURCE=.\vgl_homg_operators_3d.h
# End Source File
# Begin Source File
SOURCE=.\vgl_homg_operators_3d.txx
# End Source File
# Begin Source File
SOURCE=.\vgl_rtree.h
# End Source File
# Begin Source File
SOURCE=.\vgl_rtree.txx
# End Source File
# Begin Source File
SOURCE=.\vgl_h_matrix_1d.h
# End Source File
# Begin Source File
SOURCE=.\vgl_h_matrix_1d.txx
# End Source File
# Begin Source File
SOURCE=.\vgl_h_matrix_1d_compute.h
# End Source File
# Begin Source File
SOURCE=.\vgl_h_matrix_1d_compute_3point.h
# End Source File
# Begin Source File
SOURCE=.\vgl_h_matrix_1d_compute_linear.h
# End Source File
# Begin Source File
SOURCE=.\vgl_h_matrix_1d_compute_optimize.h
# End Source File
# Begin Source File
SOURCE=.\vgl_h_matrix_2d.h
# End Source File
# Begin Source File
SOURCE=.\vgl_h_matrix_2d.txx
# End Source File
# Begin Source File
SOURCE=.\vgl_h_matrix_2d_compute.h
# End Source File
# Begin Source File
SOURCE=.\vgl_h_matrix_2d_compute_4point.h
# End Source File
# Begin Source File
SOURCE=.\vgl_h_matrix_2d_compute_linear.h
# End Source File
# Begin Source File
SOURCE=.\vgl_h_matrix_2d_optimize.h
# End Source File
# Begin Source File
SOURCE=.\vgl_h_matrix_2d_optimize_lmq.h
# End Source File
# Begin Source File
SOURCE=.\vgl_h_matrix_3d.h
# End Source File
# Begin Source File
SOURCE=.\vgl_h_matrix_3d.txx
# End Source File
# Begin Source File
SOURCE=.\vgl_norm_trans_2d.h
# End Source File
# Begin Source File
SOURCE=.\vgl_norm_trans_2d.txx
# End Source File
# Begin Source File
SOURCE=.\vgl_fit_lines_2d.h
# End Source File
# Begin Source File
SOURCE=.\vgl_fit_lines_2d.txx
# End Source File
# Begin Source File
SOURCE=.\vgl_line_2d_regression.h
# End Source File
# Begin Source File
SOURCE=.\vgl_line_2d_regression.txx
# End Source File
# Begin Source File
SOURCE=.\vgl_p_matrix.h
# End Source File
# Begin Source File
SOURCE=.\vgl_p_matrix.txx
# End Source File
# Begin Source File
SOURCE=.\vgl_convex_hull_2d.h
# End Source File
# Begin Source File
SOURCE=.\vgl_convex_hull_2d.txx
# End Source File
# Begin Source File
SOURCE=.\vgl_norm_trans_3d.h
# End Source File
# Begin Source File
SOURCE=.\vgl_norm_trans_3d.txx
# End Source File
# Begin Source File
SOURCE=.\vgl_conic_2d_regression.h
# End Source File
# Begin Source File
SOURCE=.\vgl_conic_2d_regression.txx
# End Source File
# Begin Source File
SOURCE=.\vgl_fit_conics_2d.h
# End Source File
# Begin Source File
SOURCE=.\vgl_fit_conics_2d.txx
# End Source File
# Begin Source File
SOURCE=.\vgl_orient_box_3d.h
# End Source File
# Begin Source File
SOURCE=.\vgl_orient_box_3d.txx
# End Source File
# Begin Source File
SOURCE=.\vgl_fit_plane_3d.h
# End Source File
# Begin Source File
SOURCE=.\vgl_fit_plane_3d.txx
# End Source File
# Begin Source File
SOURCE=.\vgl_intersection.h
# End Source File
# Begin Source File
SOURCE=.\vgl_intersection.txx
# End Source File
# Begin Source File
SOURCE=.\vgl_rotation_3d.h
# End Source File
# End Group
# End Target
# End Project
