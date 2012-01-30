# Microsoft Developer Studio Project File - Name="sdet" - Package Owner=<4>
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
# sdet  == name of output library

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=sdet - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "sdet.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "sdet.mak" CFG="vil2 - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "sdet - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "sdet - Win32 Release" (based on "Win32 (x86) Static Library")
# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "sdet - Win32 Release"

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
# ADD CPP /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/core" /I "$(VXLROOT)/vcl/config.win32" /I "$(VXLROOT)/contrib/mul" -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE /D "sdet_EXPORTS"
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

!ELSEIF  "$(CFG)" == "sdet - Win32 Debug"

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
# ADD CPP /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/core" /I "$(VXLROOT)/vcl/config.win32" /I "$(VXLROOT)/contrib/mul" -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE /D "sdet_EXPORTS"
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

# Name "sdet - Win32 Release"
# Name "sdet - Win32 Debug"

# Begin Group "Source Files"
# PROP Default_Filter "cxx;c"
# Begin Source File
SOURCE=.\sdet_contour.cxx
# End Source File
# Begin Source File
SOURCE=.\sdet_detector.cxx
# End Source File
# Begin Source File
SOURCE=.\sdet_detector_params.cxx
# End Source File
# Begin Source File
SOURCE=.\sdet_edge_champher.cxx
# End Source File
# Begin Source File
SOURCE=.\sdet_edgel_regions.cxx
# End Source File
# Begin Source File
SOURCE=.\sdet_region_edge.cxx
# End Source File
# Begin Source File
SOURCE=.\sdet_region_proc.cxx
# End Source File
# Begin Source File
SOURCE=.\sdet_region_proc_params.cxx
# End Source File
# Begin Source File
SOURCE=.\sdet_harris_detector.cxx
# End Source File
# Begin Source File
SOURCE=.\sdet_harris_detector_params.cxx
# End Source File
# Begin Source File
SOURCE=.\sdet_fit_lines.cxx
# End Source File
# Begin Source File
SOURCE=.\sdet_fit_lines_params.cxx
# End Source File
# Begin Source File
SOURCE=.\sdet_grid_finder.cxx
# End Source File
# Begin Source File
SOURCE=.\sdet_grid_finder_params.cxx
# End Source File
# Begin Source File
SOURCE=.\sdet_region.cxx
# End Source File
# Begin Source File
SOURCE=.\sdet_watershed_region_proc.cxx
# End Source File
# Begin Source File
SOURCE=.\sdet_watershed_region_proc_params.cxx
# End Source File
# Begin Source File
SOURCE=.\sdet_fit_conics.cxx
# End Source File
# Begin Source File
SOURCE=.\sdet_fit_conics_params.cxx
# End Source File
# Begin Source File
SOURCE=.\sdet_nonmax_suppression.cxx
# End Source File
# Begin Source File
SOURCE=.\sdet_nonmax_suppression_params.cxx
# End Source File
# Begin Source File
SOURCE=.\sdet_gauss_fit.cxx
# End Source File
# Begin Source File
SOURCE=.\sdet_img_edge.cxx
# End Source File
# Begin Source File
SOURCE=.\sdet_third_order_edge_det.cxx
# End Source File
# Begin Source File
SOURCE=.\sdet_third_order_edge_det_params.cxx
# End Source File
# Begin Source File
SOURCE=.\sdet_nms.cxx
# End Source File
# Begin Source File
SOURCE=.\sdet_image_mesh.cxx
# End Source File
# Begin Source File
SOURCE=.\sdet_image_mesh_params.cxx
# End Source File
# Begin Source File
SOURCE=.\sdet_denoise_mrf.cxx
# End Source File
# Begin Source File
SOURCE=.\sdet_denoise_mrf_params.cxx
# End Source File
# Begin Source File
SOURCE=.\sdet_denoise_mrf_bp.cxx
# End Source File
# Begin Source File
SOURCE=.\sdet_denoise_mrf_bp_params.cxx
# End Source File
# Begin Source File
SOURCE=.\sdet_mrf_site_bp.cxx
# End Source File
# Begin Source File
SOURCE=.\sdet_mrf_bp.cxx
# End Source File
# Begin Source File
SOURCE=.\sdet_vrml_display.cxx
# End Source File
# Begin Source File
SOURCE=.\sdet_texture_classifier_params.cxx
# End Source File
# Begin Source File
SOURCE=.\sdet_k_means.cxx
# End Source File
# Begin Source File
SOURCE=.\sdet_texture_classifier.cxx
# End Source File
# Begin Source File
SOURCE=.\sdet_atmospheric_image_classifier.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vbl_smart_ptr+sdet_region_edge-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vbl_smart_ptr+sdet_region-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vbl_smart_ptr+sdet_mrf_site_bp-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vbl_array_2d+sdet_mrf_site_bp_sptr-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vbl_smart_ptr+sdet_mrf_bp-.cxx
# End Source File
# End Group
# Begin Group "Header Files"
# PROP Default_Filter "h;txx"
# Begin Source File
SOURCE=.\sdet_contour.h
# End Source File
# Begin Source File
SOURCE=.\sdet_detector.h
# End Source File
# Begin Source File
SOURCE=.\sdet_detector_params.h
# End Source File
# Begin Source File
SOURCE=.\sdet_edge_champher.h
# End Source File
# Begin Source File
SOURCE=.\sdet_edgel_regions.h
# End Source File
# Begin Source File
SOURCE=.\sdet_region_edge.h
# End Source File
# Begin Source File
SOURCE=.\sdet_region_proc.h
# End Source File
# Begin Source File
SOURCE=.\sdet_region_proc_params.h
# End Source File
# Begin Source File
SOURCE=.\sdet_region_edge_sptr.h
# End Source File
# Begin Source File
SOURCE=.\sdet_harris_detector.h
# End Source File
# Begin Source File
SOURCE=.\sdet_fit_lines.h
# End Source File
# Begin Source File
SOURCE=.\sdet_fit_lines_params.h
# End Source File
# Begin Source File
SOURCE=.\sdet_harris_detector_params.h
# End Source File
# Begin Source File
SOURCE=.\sdet_grid_finder.h
# End Source File
# Begin Source File
SOURCE=.\sdet_grid_finder_params.h
# End Source File
# Begin Source File
SOURCE=.\sdet_region.h
# End Source File
# Begin Source File
SOURCE=.\sdet_region_sptr.h
# End Source File
# Begin Source File
SOURCE=.\sdet_watershed_region_proc.h
# End Source File
# Begin Source File
SOURCE=.\sdet_watershed_region_proc_params.h
# End Source File
# Begin Source File
SOURCE=.\sdet_fit_conics.h
# End Source File
# Begin Source File
SOURCE=.\sdet_fit_conics_params.h
# End Source File
# Begin Source File
SOURCE=.\sdet_nonmax_suppression.h
# End Source File
# Begin Source File
SOURCE=.\sdet_nonmax_suppression_params.h
# End Source File
# Begin Source File
SOURCE=.\sdet_gauss_fit.h
# End Source File
# Begin Source File
SOURCE=.\sdet_img_edge.h
# End Source File
# Begin Source File
SOURCE=.\sdet_third_order_edge_det_params.h
# End Source File
# Begin Source File
SOURCE=.\sdet_third_order_edge_det.h
# End Source File
# Begin Source File
SOURCE=.\sdet_nms.h
# End Source File
# Begin Source File
SOURCE=.\sdet_image_mesh.h
# End Source File
# Begin Source File
SOURCE=.\sdet_image_mesh_params.h
# End Source File
# Begin Source File
SOURCE=.\sdet_denoise_mrf.h
# End Source File
# Begin Source File
SOURCE=.\sdet_denoise_mrf_params.h
# End Source File
# Begin Source File
SOURCE=.\sdet_mrf_site_bp.h
# End Source File
# Begin Source File
SOURCE=.\sdet_mrf_site_bp_sptr.h
# End Source File
# Begin Source File
SOURCE=.\sdet_denoise_mrf_bp_params.h
# End Source File
# Begin Source File
SOURCE=.\sdet_mrf_bp.h
# End Source File
# Begin Source File
SOURCE=.\sdet_denoise_mrf_bp.h
# End Source File
# Begin Source File
SOURCE=.\sdet_mrf_bp_sptr.h
# End Source File
# Begin Source File
SOURCE=.\sdet_vrml_display.h
# End Source File
# Begin Source File
SOURCE=.\sdet_k_means.h
# End Source File
# Begin Source File
SOURCE=.\sdet_texture_classifier.h
# End Source File
# Begin Source File
SOURCE=.\sdet_texture_classifier_params.h
# End Source File
# Begin Source File
SOURCE=.\sdet_atmospheric_image_classifier.h
# End Source File
# End Group
# End Target
# End Project
