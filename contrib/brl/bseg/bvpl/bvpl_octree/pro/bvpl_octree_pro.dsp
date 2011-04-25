# Microsoft Developer Studio Project File - Name="bvpl_octree_pro" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# variables to REPLACE
#
# /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/core" /I "$(VXLROOT)/vcl/config.win32" /I "$(VXLROOT)/contrib/brl/bseg" == include path
#   -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE == compiler defines
#  == override in output directory
# bvpl_octree_pro  == name of output library

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=bvpl_octree_pro - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE
!MESSAGE NMAKE /f "bvpl_octree_pro.mak".
!MESSAGE
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE
!MESSAGE NMAKE /f "bvpl_octree_pro.mak" CFG="bmrf - Win32 Debug"
!MESSAGE
!MESSAGE Possible choices for configuration are:
!MESSAGE
!MESSAGE "bvpl_octree_pro - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "bvpl_octree_pro - Win32 Release" (based on "Win32 (x86) Static Library")
# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "bvpl_octree_pro - Win32 Release"

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
# ADD CPP /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/core" /I "$(VXLROOT)/vcl/config.win32" /I "$(VXLROOT)/contrib/brl/bseg" -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE /D "bvpl_octree_pro_EXPORTS"
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

!ELSEIF  "$(CFG)" == "bvpl_octree_pro - Win32 Debug"

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
# ADD CPP /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/core" /I "$(VXLROOT)/vcl/config.win32" /I "$(VXLROOT)/contrib/brl/bseg" -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE /D "bvpl_octree_pro_EXPORTS"
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

# Name "bvpl_octree_pro - Win32 Release"
# Name "bvpl_octree_pro - Win32 Debug"

# Begin Group "Source Files"
# PROP Default_Filter "cxx"
# Begin Source File
SOURCE=.\processes/bvpl_save_vrml_process.cxx
# End Source File
# Begin Source File
SOURCE=.\bvpl_octree_register.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/bvpl_scene_vector_operator_process.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/bvpl_create_scene_process.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/bvpl_scene_kernel_operator_process.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/bvpl_plane_propagate_process.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/bvpl_nonmax_supp_process.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/bvpl_compute_gauss_gradients.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/pca/bvpl_discover_pca_features_process.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/pca/bvpl_compute_pca_test_error_process.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/bvpl_block_kernel_operator_process.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/taylor/bvpl_add_taylor_errors_process.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/pca/bvpl_add_pca_errors_process.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/pca/bvpl_load_pca_error_scenes_process.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/taylor/bvpl_compute_taylor_error_process.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/pca/bvpl_pca_project_process.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/taylor/bvpl_load_taylor_scenes_process.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/bvpl_block_avg_value_process.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/bvpl_scene_histogram_process.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/pca/bvpl_compute_pca_error_block_process.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/pca/bvpl_compute_pca_error_scene_process.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/pca/bvpl_load_pca_info_process.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/pca/bvpl_init_global_pca_process.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/pca/bvpl_pca_global_statistics_process.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/pca/bvpl_global_pca_process.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/pca/bvpl_normalize_pca_training_error_process.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/pca/bvpl_combine_pairwise_statistics_process.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/taylor/bvpl_init_global_taylor_process.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/taylor/bvpl_explore_coefficient_scene_process.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/taylor/bvpl_compute_taylor_coefficients_process.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/brdb_value_t+bvpl_pca_error_scenes_sptr-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/brdb_value_t+bvpl_taylor_scenes_map_sptr-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/brdb_value_t+bvpl_discover_pca_kernels_sptr-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/brdb_value_t+boxm_scene_base_sptr-.cxx
# End Source File
# End Group
# Begin Group "Header Files"
# PROP Default_Filter "h;txx"
# Begin Source File
SOURCE=.\bvpl_octree_register.h
# End Source File
# Begin Source File
SOURCE=.\bvpl_octree_processes.h
# End Source File
# End Group
# End Target
# End Project
