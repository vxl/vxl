# Microsoft Developer Studio Project File - Name="bvxm_pro" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# variables to REPLACE
# 
# /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/core" /I "$(VXLROOT)/vcl/config.win32" /I "$(VXLROOT)/contrib/brl/bseg" == include path
#   -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE == compiler defines
#  == override in output directory
# bvxm_pro  == name of output library

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=bvxm_pro - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "bvxm_pro.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "bvxm_pro.mak" CFG="bmrf - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "bvxm_pro - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "bvxm_pro - Win32 Release" (based on "Win32 (x86) Static Library")
# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "bvxm_pro - Win32 Release"

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
# ADD CPP /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/core" /I "$(VXLROOT)/vcl/config.win32" /I "$(VXLROOT)/contrib/brl/bseg" -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE /D "bvxm_pro_EXPORTS"
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

!ELSEIF  "$(CFG)" == "bvxm_pro - Win32 Debug"

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
# ADD CPP /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/core" /I "$(VXLROOT)/vcl/config.win32" /I "$(VXLROOT)/contrib/brl/bseg" -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE /D "bvxm_pro_EXPORTS"
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

# Name "bvxm_pro - Win32 Release"
# Name "bvxm_pro - Win32 Debug"

# Begin Group "Source Files"
# PROP Default_Filter "cxx"
# Begin Source File
SOURCE=.\bvxm_register.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/bvxm_change_detection_display_process.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/bvxm_clean_world_process.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/bvxm_compare_rpc_process.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/bvxm_create_normalized_image_process.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/bvxm_render_virtual_view_process.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/bvxm_ocp_hist_process.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/bvxm_heightmap_process.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/bvxm_pmap_ratio_process.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/bvxm_lidar_init_process.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/bvxm_illum_index_process.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/bvxm_pmap_hist_process.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/bvxm_render_expected_image_process.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/bvxm_lidar_edge_detection_process.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/bvxm_create_local_rpc_process.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/bvxm_detect_changes_process.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/bvxm_ocp_compare_process.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/bvxm_create_voxel_world_process.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/bvxm_normalize_image_process.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/bvxm_gen_synthetic_world_process.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/bvxm_save_occupancy_raw_process.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/bvxm_update_edges_lidar_process.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/bvxm_rpc_registration_process.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/bvxm_roc_process.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/bvxm_update_lidar_process.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/bvxm_save_occupancy_vff_process.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/bvxm_compare_3d_voxels_process.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/bvxm_roi_init_process.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/bvxm_normalization_util.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/bvxm_update_process.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/bvxm_detect_scale_process.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/bvxm_save_edges_raw_process.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/bvxm_update_edges_process.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/bvxm_create_mog_image_process.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/bvxm_detect_edges_process.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/bvxm_synth_world_generator.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/bvxm_locate_region_process.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/brdb_value_t+bvxm_voxel_world_sptr-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/brdb_value_t+bvxm_voxel_slab_base_sptr-.cxx
# End Source File
# End Group
# Begin Group "Header Files"
# PROP Default_Filter "h;txx"
# Begin Source File
SOURCE=.\bvxm_register.h
# End Source File
# Begin Source File
SOURCE=.\bvxm_processes.h
# End Source File
# Begin Source File
SOURCE=.\processes/bvxm_normalization_util.h
# End Source File
# Begin Source File
SOURCE=.\processes/bvxm_synth_world_generator.h
# End Source File
# End Group
# End Target
# End Project
