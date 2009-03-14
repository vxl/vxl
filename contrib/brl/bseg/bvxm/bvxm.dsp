# Microsoft Developer Studio Project File - Name="bvxm" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# variables to REPLACE
# 
# /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/core" /I "$(VXLROOT)/vcl/config.win32" /I "$(VXLROOT)/contrib/brl/bseg" == include path
#   -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE == compiler defines
#  == override in output directory
# bvxm  == name of output library

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=bvxm - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "bvxm.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "bvxm.mak" CFG="bmrf - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "bvxm - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "bvxm - Win32 Release" (based on "Win32 (x86) Static Library")
# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "bvxm - Win32 Release"

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
# ADD CPP /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/core" /I "$(VXLROOT)/vcl/config.win32" /I "$(VXLROOT)/contrib/brl/bseg" -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE /D "bvxm_EXPORTS"
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

!ELSEIF  "$(CFG)" == "bvxm - Win32 Debug"

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
# ADD CPP /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/core" /I "$(VXLROOT)/vcl/config.win32" /I "$(VXLROOT)/contrib/brl/bseg" -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE /D "bvxm_EXPORTS"
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

# Name "bvxm - Win32 Release"
# Name "bvxm - Win32 Debug"

# Begin Group "Source Files"
# PROP Default_Filter "cxx"
# Begin Source File
SOURCE=.\bvxm_image_metadata.cxx
# End Source File
# Begin Source File
SOURCE=.\bvxm_memory_chunk.cxx
# End Source File
# Begin Source File
SOURCE=.\bvxm_mog_grey_processor.cxx
# End Source File
# Begin Source File
SOURCE=.\bvxm_mog_rgb_processor.cxx
# End Source File
# Begin Source File
SOURCE=.\bvxm_util.cxx
# End Source File
# Begin Source File
SOURCE=.\bvxm_voxel_world.cxx
# End Source File
# Begin Source File
SOURCE=.\bvxm_world_params.cxx
# End Source File
# Begin Source File
SOURCE=.\bvxm_lidar_processor.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bvxm_voxel_grid+float-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bvxm_voxel_slab+bool-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bvxm_voxel_slab+float-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bvxm_voxel_slab+uint-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bvxm_voxel_slab_iterator+float-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bvxm_voxel_storage_disk+float-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bvxm_voxel_storage_mem+float-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vbl_smart_ptr+bvxm_memory_chunk-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vbl_smart_ptr+bvxm_voxel_grid_base-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vbl_smart_ptr+bvxm_voxel_slab_base-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vbl_smart_ptr+bvxm_voxel_world-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vbl_smart_ptr+bvxm_world_params-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bvxm_voxel_grid+bsta_num_obs+bsta_mixture_fixed+bsta_num_obs+bsta_gauss_f1-.3---.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bvxm_voxel_grid+bsta_num_obs+bsta_mixture_fixed+bsta_num_obs+bsta_gauss_if3-.3---.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bvxm_voxel_slab+bsta_num_obs+bsta_mixture_fixed+bsta_num_obs+bsta_gauss_f1-.3---.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bvxm_voxel_slab+bsta_num_obs+bsta_mixture_fixed+bsta_num_obs+bsta_gauss_if3-.3---.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bvxm_voxel_slab+vnl_vector_fixed+float.3--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bvxm_voxel_slab_iterator+bsta_num_obs+bsta_mixture_fixed+bsta_num_obs+bsta_gauss_f1-.3---.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bvxm_voxel_slab_iterator+bsta_num_obs+bsta_mixture_fixed+bsta_num_obs+bsta_gauss_if3-.3---.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bvxm_voxel_storage_disk+bsta_num_obs+bsta_mixture_fixed+bsta_num_obs+bsta_gauss_f1-.3---.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bvxm_voxel_storage_disk+bsta_num_obs+bsta_mixture_fixed+bsta_num_obs+bsta_gauss_if3-.3---.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bvxm_voxel_storage_mem+bsta_num_obs+bsta_mixture_fixed+bsta_num_obs+bsta_gauss_f1-.3---.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bvxm_voxel_storage_mem+bsta_num_obs+bsta_mixture_fixed+bsta_num_obs+bsta_gauss_if3-.3---.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bvxm_voxel_storage_disk_cached+bsta_num_obs+bsta_mixture_fixed+bsta_num_obs+bsta_gauss_f1-.3---.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bvxm_voxel_storage_disk_cached+bsta_num_obs+bsta_mixture_fixed+bsta_num_obs+bsta_gauss_if3-.3---.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bvxm_voxel_storage_disk_cached+float-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bvxm_mog_mc_processor+2.3-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bvxm_mog_mc_processor+3.3-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bvxm_mog_mc_processor+4.3-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bvxm_voxel_grid+bsta_num_obs+bsta_mixture_fixed+bsta_num_obs+bsta_gaussian_indep+float.2--.3---.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bvxm_voxel_grid+bsta_num_obs+bsta_mixture_fixed+bsta_num_obs+bsta_gaussian_indep+float.4--.3---.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bvxm_voxel_slab+bsta_num_obs+bsta_mixture_fixed+bsta_num_obs+bsta_gaussian_indep+float.2--.3---.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bvxm_voxel_slab+bsta_num_obs+bsta_mixture_fixed+bsta_num_obs+bsta_gaussian_indep+float.4--.3---.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bvxm_voxel_slab+vnl_vector_fixed+float.2--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bvxm_voxel_slab+vnl_vector_fixed+float.4--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bvxm_voxel_slab_iterator+bsta_num_obs+bsta_mixture_fixed+bsta_num_obs+bsta_gaussian_indep+float.2--.3---.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bvxm_voxel_slab_iterator+bsta_num_obs+bsta_mixture_fixed+bsta_num_obs+bsta_gaussian_indep+float.4--.3---.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bvxm_voxel_storage_disk+bsta_num_obs+bsta_mixture_fixed+bsta_num_obs+bsta_gaussian_indep+float.2--.3---.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bvxm_voxel_storage_disk+bsta_num_obs+bsta_mixture_fixed+bsta_num_obs+bsta_gaussian_indep+float.4--.3---.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bvxm_voxel_storage_disk_cached+bsta_num_obs+bsta_mixture_fixed+bsta_num_obs+bsta_gaussian_indep+float.2--.3---.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bvxm_voxel_storage_disk_cached+bsta_num_obs+bsta_mixture_fixed+bsta_num_obs+bsta_gaussian_indep+float.4--.3---.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bvxm_voxel_storage_mem+bsta_num_obs+bsta_mixture_fixed+bsta_num_obs+bsta_gaussian_indep+float.2--.3---.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bvxm_voxel_storage_mem+bsta_num_obs+bsta_mixture_fixed+bsta_num_obs+bsta_gaussian_indep+float.4--.3---.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bvxm_voxel_slab_iterator+bsta_num_obs+bsta_mixture_fixed+bsta_num_obs+bsta_gaussian_indep+float.3--.3---.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bvxm_voxel_grid+bsta_num_obs+bsta_mixture_fixed+bsta_num_obs+bsta_gaussian_sphere+float.1--.3---.cxx
# End Source File
# End Group
# Begin Group "Header Files"
# PROP Default_Filter "h;txx"
# Begin Source File
# End Source File
# Begin Source File
SOURCE=.\bvxm_image_metadata.h
# End Source File
# Begin Source File
SOURCE=.\bvxm_memory_chunk.h
# End Source File
# Begin Source File
SOURCE=.\bvxm_mog_grey_processor.h
# End Source File
# Begin Source File
SOURCE=.\bvxm_mog_rgb_processor.h
# End Source File
# Begin Source File
SOURCE=.\bvxm_util.h
# End Source File
# Begin Source File
SOURCE=.\bvxm_voxel_grid.h
# End Source File
# Begin Source File
SOURCE=.\bvxm_voxel_grid.txx
# End Source File
# Begin Source File
SOURCE=.\bvxm_voxel_grid_base.h
# End Source File
# Begin Source File
SOURCE=.\bvxm_voxel_slab.h
# End Source File
# Begin Source File
SOURCE=.\bvxm_voxel_slab.txx
# End Source File
# Begin Source File
SOURCE=.\bvxm_voxel_slab_base.h
# End Source File
# Begin Source File
SOURCE=.\bvxm_voxel_slab_iterator.h
# End Source File
# Begin Source File
SOURCE=.\bvxm_voxel_slab_iterator.txx
# End Source File
# Begin Source File
SOURCE=.\bvxm_voxel_storage.h
# End Source File
# Begin Source File
SOURCE=.\bvxm_voxel_storage_disk.h
# End Source File
# Begin Source File
SOURCE=.\bvxm_voxel_storage_disk.txx
# End Source File
# Begin Source File
SOURCE=.\bvxm_voxel_storage_mem.h
# End Source File
# Begin Source File
SOURCE=.\bvxm_voxel_storage_mem.txx
# End Source File
# Begin Source File
SOURCE=.\bvxm_voxel_traits.h
# End Source File
# Begin Source File
SOURCE=.\bvxm_voxel_world.h
# End Source File
# Begin Source File
SOURCE=.\bvxm_world_params.h
# End Source File
# Begin Source File
SOURCE=.\bvxm_lidar_processor.h
# End Source File
# Begin Source File
SOURCE=.\bvxm_voxel_storage_disk_cached.h
# End Source File
# Begin Source File
SOURCE=.\bvxm_voxel_storage_disk_cached.txx
# End Source File
# Begin Source File
SOURCE=.\bvxm_mog_mc_processor.txx
# End Source File
# Begin Source File
SOURCE=.\bvxm_mog_mc_processor.h
# End Source File
# Begin Source File
SOURCE=.\bvxm_float_processor.h
# End Source File
# End Group
# End Target
# End Project
