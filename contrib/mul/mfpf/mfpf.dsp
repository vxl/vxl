# Microsoft Developer Studio Project File - Name="mfpf" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# variables to REPLACE
# 
# /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/core" /I "$(VXLROOT)/vcl/config.win32" == include path
#   -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE == compiler defines
#  == override in output directory
# mfpf  == name of output library

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=mfpf - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "mfpf.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "mfpf.mak" CFG="mfpf - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "mfpf - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "mfpf - Win32 Release" (based on "Win32 (x86) Static Library")
# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "mfpf - Win32 Release"

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
# ADD CPP /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/core" /I "$(VXLROOT)/vcl/config.win32" -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE /D "mfpf_EXPORTS"
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

!ELSEIF  "$(CFG)" == "mfpf - Win32 Debug"

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
# ADD CPP /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/core" /I "$(VXLROOT)/vcl/config.win32" -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE /D "mfpf_EXPORTS"
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
# Name "mfpf - Win32 Release"
# Name "mfpf - Win32 Debug"
# Begin Group "Source Files"
# PROP Default_Filter "cxx"
# Begin Source File
SOURCE=.\Templates/mbl_cloneables_factory+mfpf_point_finder-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/mbl_cloneables_factory+mfpf_point_finder_builder-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vsl_binary_loader+mfpf_point_finder-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vsl_binary_loader+mfpf_point_finder_builder-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/mbl_cloneables_factory+mfpf_vec_cost_builder-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vsl_binary_loader+mfpf_vec_cost-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vsl_binary_loader+mfpf_vec_cost_builder-.cxx
# End Source File
# Begin Source File
SOURCE=.\mfpf_add_all_loaders.cxx
# End Source File
# Begin Source File
SOURCE=.\mfpf_edge_finder.cxx
# End Source File
# Begin Source File
SOURCE=.\mfpf_edge_finder_builder.cxx
# End Source File
# Begin Source File
SOURCE=.\mfpf_norm_corr1d.cxx
# End Source File
# Begin Source File
SOURCE=.\mfpf_norm_corr1d_builder.cxx
# End Source File
# Begin Source File
SOURCE=.\mfpf_norm_corr2d.cxx
# End Source File
# Begin Source File
SOURCE=.\mfpf_norm_corr2d_builder.cxx
# End Source File
# Begin Source File
SOURCE=.\mfpf_point_finder.cxx
# End Source File
# Begin Source File
SOURCE=.\mfpf_point_finder_builder.cxx
# End Source File
# Begin Source File
SOURCE=.\mfpf_profile_pdf.cxx
# End Source File
# Begin Source File
SOURCE=.\mfpf_profile_pdf_builder.cxx
# End Source File
# Begin Source File
SOURCE=.\mfpf_region_pdf.cxx
# End Source File
# Begin Source File
SOURCE=.\mfpf_region_pdf_builder.cxx
# End Source File
# Begin Source File
SOURCE=.\mfpf_draw_pose_cross.cxx
# End Source File
# Begin Source File
SOURCE=.\mfpf_searcher.cxx
# End Source File
# Begin Source File
SOURCE=.\mfpf_draw_pose_lines.cxx
# End Source File
# Begin Source File
SOURCE=.\mfpf_draw_matches.cxx
# End Source File
# Begin Source File
SOURCE=.\mfpf_mr_point_finder.cxx
# End Source File
# Begin Source File
SOURCE=.\mfpf_mr_point_finder_builder.cxx
# End Source File
# Begin Source File
SOURCE=.\mfpf_prune_overlaps.cxx
# End Source File
# Begin Source File
SOURCE=.\mfpf_sort_matches.cxx
# End Source File
# Begin Source File
SOURCE=.\mfpf_region_finder.cxx
# End Source File
# Begin Source File
SOURCE=.\mfpf_region_finder_builder.cxx
# End Source File
# Begin Source File
SOURCE=.\mfpf_sad_vec_cost.cxx
# End Source File
# Begin Source File
SOURCE=.\mfpf_sad_vec_cost_builder.cxx
# End Source File
# Begin Source File
SOURCE=.\mfpf_vec_cost.cxx
# End Source File
# Begin Source File
SOURCE=.\mfpf_vec_cost_builder.cxx
# End Source File
# Begin Source File
SOURCE=.\mfpf_dp_snake.cxx
# End Source File
# Begin Source File
SOURCE=.\mfpf_max_finder.cxx
# End Source File
# Begin Source File
SOURCE=.\mfpf_ssd_vec_cost.cxx
# End Source File
# Begin Source File
SOURCE=.\mfpf_ssd_vec_cost_builder.cxx
# End Source File
# Begin Source File
SOURCE=.\mfpf_hog_box_finder.cxx
# End Source File
# Begin Source File
SOURCE=.\mfpf_hog_box_finder_builder.cxx
# End Source File
# End Group
# Begin Group "Header Files"
# PROP Default_Filter "h;txx"
# Begin Source File
SOURCE=.\mfpf_add_all_loaders.h
# End Source File
# Begin Source File
SOURCE=.\mfpf_edge_finder.h
# End Source File
# Begin Source File
SOURCE=.\mfpf_edge_finder_builder.h
# End Source File
# Begin Source File
SOURCE=.\mfpf_norm_corr1d.h
# End Source File
# Begin Source File
SOURCE=.\mfpf_norm_corr1d_builder.h
# End Source File
# Begin Source File
SOURCE=.\mfpf_norm_corr2d.h
# End Source File
# Begin Source File
SOURCE=.\mfpf_norm_corr2d_builder.h
# End Source File
# Begin Source File
SOURCE=.\mfpf_norm_vec.h
# End Source File
# Begin Source File
SOURCE=.\mfpf_point_finder.h
# End Source File
# Begin Source File
SOURCE=.\mfpf_point_finder_builder.h
# End Source File
# Begin Source File
SOURCE=.\mfpf_profile_pdf.h
# End Source File
# Begin Source File
SOURCE=.\mfpf_profile_pdf_builder.h
# End Source File
# Begin Source File
SOURCE=.\mfpf_region_pdf.h
# End Source File
# Begin Source File
SOURCE=.\mfpf_region_pdf_builder.h
# End Source File
# Begin Source File
SOURCE=.\mfpf_sample_region.h
# End Source File
# Begin Source File
SOURCE=.\mfpf_draw_pose_cross.h
# End Source File
# Begin Source File
SOURCE=.\mfpf_pose.h
# End Source File
# Begin Source File
SOURCE=.\mfpf_searcher.h
# End Source File
# Begin Source File
SOURCE=.\mfpf_draw_pose_lines.h
# End Source File
# Begin Source File
SOURCE=.\mfpf_draw_matches.h
# End Source File
# Begin Source File
SOURCE=.\mfpf_pose_set.h
# End Source File
# Begin Source File
SOURCE=.\mfpf_region_form.h
# End Source File
# Begin Source File
SOURCE=.\mfpf_mr_point_finder.h
# End Source File
# Begin Source File
SOURCE=.\mfpf_mr_point_finder_builder.h
# End Source File
# Begin Source File
SOURCE=.\mfpf_prune_overlaps.h
# End Source File
# Begin Source File
SOURCE=.\mfpf_sort_matches.h
# End Source File
# Begin Source File
SOURCE=.\mfpf_region_finder.h
# End Source File
# Begin Source File
SOURCE=.\mfpf_region_finder_builder.h
# End Source File
# Begin Source File
SOURCE=.\mfpf_sad_vec_cost.h
# End Source File
# Begin Source File
SOURCE=.\mfpf_sad_vec_cost_builder.h
# End Source File
# Begin Source File
SOURCE=.\mfpf_vec_cost.h
# End Source File
# Begin Source File
SOURCE=.\mfpf_vec_cost_builder.h
# End Source File
# Begin Source File
SOURCE=.\mfpf_feature_vec.h
# End Source File
# Begin Source File
SOURCE=.\mfpf_dp_snake.h
# End Source File
# Begin Source File
SOURCE=.\mfpf_max_finder.h
# End Source File
# Begin Source File
SOURCE=.\mfpf_ssd_vec_cost.h
# End Source File
# Begin Source File
SOURCE=.\mfpf_ssd_vec_cost_builder.h
# End Source File
# Begin Source File
SOURCE=.\mfpf_hog_box_finder.h
# End Source File
# Begin Source File
SOURCE=.\mfpf_hog_box_finder_builder.h
# End Source File
# End Group
# End Target
# End Project
