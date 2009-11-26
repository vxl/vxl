# Microsoft Developer Studio Project File - Name="bvxm_grid" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# variables to REPLACE
# 
# /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/core" /I "$(VXLROOT)/vcl/config.win32" == include path
#   -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE == compiler defines
#  == override in output directory
# bvxm_grid  == name of output library

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=bvxm_grid - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "bvxm_grid.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "bvxm_grid.mak" CFG="bvxm_grid - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "bvxm_grid - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "bvxm_grid - Win32 Release" (based on "Win32 (x86) Static Library")
# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "bvxm_grid - Win32 Release"

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
# ADD CPP /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/core" /I "$(VXLROOT)/vcl/config.win32" -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE /D "bvxm_grid_EXPORTS"
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

!ELSEIF  "$(CFG)" == "bvxm_grid - Win32 Debug"

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
# ADD CPP /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/core" /I "$(VXLROOT)/vcl/config.win32" -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE /D "bvxm_grid_EXPORTS"
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
# Name "bvxm_grid - Win32 Release"
# Name "bvxm_grid - Win32 Debug"
# Begin Group "Source Files"
# PROP Default_Filter "cxx"
# Begin Source File
SOURCE=.\bvxm_memory_chunk.cxx
# End Source File
# Begin Source File
SOURCE=.\bvxm_voxel_grid_opinion_basic_ops.cxx
# End Source File
# Begin Source File
SOURCE=.\bvxm_voxel_grid_basic_ops.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bvxm_voxel_storage_mem+bsta_num_obs+bsta_mixture_fixed+bsta_num_obs+bsta_gaussian_indep+float.2--.3---.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bvxm_voxel_storage_disk_cached+float-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bvxm_voxel_slab_iterator+bsta_num_obs+bsta_mixture_fixed+bsta_num_obs+bsta_gaussian_indep+float.4--.3---.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bvxm_voxel_storage_disk+bsta_num_obs+bsta_gauss_f1--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vbl_smart_ptr+bvxm_voxel_slab_base-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bvxm_voxel_storage_disk_cached+bsta_num_obs+bsta_mixture_fixed+bsta_num_obs+bsta_gaussian_indep+float.4--.3---.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bvxm_voxel_storage_disk_cached+bsta_num_obs+bsta_mixture_fixed+bsta_num_obs+bsta_gauss_f1-.3---.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bvxm_voxel_slab+bsta_num_obs+bsta_mixture_fixed+bsta_num_obs+bsta_gaussian_indep+float.4--.3---.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bvxm_voxel_storage_mem+float-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bvxm_voxel_slab_iterator+bsta_num_obs+bsta_mixture_fixed+bsta_num_obs+bsta_gauss_if3-.3---.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bvxm_voxel_storage_disk_cached+bsta_num_obs+bsta_mixture_fixed+bsta_num_obs+bsta_gaussian_indep+float.2--.3---.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bvxm_voxel_storage_mem+bsta_num_obs+bsta_gauss_f1--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bvxm_voxel_slab_iterator+bsta_num_obs+bsta_mixture_fixed+bsta_num_obs+bsta_gauss_f1-.3---.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bvxm_voxel_slab_iterator+float-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bvxm_voxel_storage_disk+bsta_num_obs+bsta_mixture_fixed+bsta_num_obs+bsta_gauss_f1-.3---.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bvxm_voxel_slab+bool-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bvxm_voxel_storage_disk_cached+bsta_num_obs+bsta_mixture_fixed+bsta_num_obs+bsta_gauss_if3-.3---.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bvxm_voxel_slab+bsta_num_obs+bsta_mixture_fixed+bsta_num_obs+bsta_gauss_f1-.3---.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bvxm_voxel_slab_iterator+bsta_num_obs+bsta_mixture_fixed+bsta_num_obs+bsta_gaussian_indep+float.2--.3---.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bvxm_voxel_grid+bsta_num_obs+bsta_mixture_fixed+bsta_num_obs+bsta_gauss_f1-.3---.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bvxm_voxel_storage_disk+bsta_num_obs+bsta_mixture_fixed+bsta_num_obs+bsta_gaussian_indep+float.2--.3---.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bvxm_voxel_slab_iterator+bsta_num_obs+bsta_mixture_fixed+bsta_num_obs+bsta_gaussian_indep+float.3--.3---.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bvxm_voxel_grid+float-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bvxm_voxel_storage_disk+bsta_num_obs+bsta_mixture_fixed+bsta_num_obs+bsta_gauss_if3-.3---.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bvxm_voxel_storage_mem+bsta_num_obs+bsta_mixture_fixed+bsta_num_obs+bsta_gaussian_indep+float.4--.3---.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bvxm_voxel_slab+uint-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bvxm_voxel_storage_disk+float-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bvxm_voxel_grid+bsta_num_obs+bsta_mixture_fixed+bsta_num_obs+bsta_gaussian_sphere+float.1--.3---.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bvxm_voxel_storage_disk_cached+bsta_num_obs+bsta_gauss_f1--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bvxm_voxel_grid+bsta_num_obs+bsta_mixture_fixed+bsta_num_obs+bsta_gaussian_indep+float.4--.3---.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bvxm_voxel_grid+bsta_num_obs+bsta_gauss_f1--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bvxm_voxel_storage_mem+bsta_num_obs+bsta_mixture_fixed+bsta_num_obs+bsta_gauss_if3-.3---.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bvxm_voxel_storage_disk+bsta_num_obs+bsta_mixture_fixed+bsta_num_obs+bsta_gaussian_indep+float.4--.3---.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bvxm_voxel_storage_mem+bsta_num_obs+bsta_mixture_fixed+bsta_num_obs+bsta_gauss_f1-.3---.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bvxm_voxel_grid+bsta_num_obs+bsta_mixture_fixed+bsta_num_obs+bsta_gauss_if3-.3---.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bvxm_voxel_slab_iterator+bsta_num_obs+bsta_gauss_f1--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bvxm_voxel_slab+bsta_num_obs+bsta_gauss_f1--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vbl_smart_ptr+bvxm_memory_chunk-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bvxm_voxel_slab+vnl_vector_fixed+float.4--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bvxm_voxel_slab+float-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vbl_smart_ptr+bvxm_voxel_grid_base-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bvxm_voxel_grid+bsta_num_obs+bsta_mixture_fixed+bsta_num_obs+bsta_gaussian_indep+float.2--.3---.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bvxm_voxel_slab+vnl_vector_fixed+float.3--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bvxm_voxel_slab+bsta_num_obs+bsta_mixture_fixed+bsta_num_obs+bsta_gaussian_indep+float.2--.3---.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bvxm_voxel_slab+bsta_num_obs+bsta_mixture_fixed+bsta_num_obs+bsta_gauss_if3-.3---.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bvxm_voxel_slab+vnl_vector_fixed+float.2--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bvxm_voxel_grid+bool-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bvxm_voxel_storage_mem+bool-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bvxm_voxel_slab_iterator+bool-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bvxm_voxel_storage_disk+bool-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bvxm_voxel_storage_disk_cached+bool-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bvxm_voxel_storage_disk+bvxm_opinion-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bvxm_voxel_slab+bvxm_opinion-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bvxm_voxel_grid+bvxm_opinion-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bvxm_voxel_slab_iterator+bvxm_opinion-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bvxm_voxel_storage_disk_cached+bvxm_opinion-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bvxm_voxel_storage_mem+bvxm_opinion-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bvxm_voxel_grid+vnl_vector_fixed+float.3--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bvxm_voxel_grid+vnl_float_4-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bvxm_voxel_slab+vnl_float_3-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bvxm_voxel_grid+vnl_float_3-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bvxm_voxel_grid+vnl_vector_fixed+float.4--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bvxm_voxel_slab+vnl_float_4-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bvxm_voxel_storage_disk_cached+uint-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bvxm_voxel_grid+uint-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bvxm_voxel_storage_mem+uint-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bvxm_voxel_slab_iterator+uint-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bvxm_voxel_storage_disk+uint-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bvxm_voxel_slab_iterator+vnl_vector_fixed+int.3--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bvxm_voxel_storage_mem+int-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bvxm_voxel_grid+int-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bvxm_voxel_slab_iterator+int-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bvxm_voxel_slab+int-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bvxm_voxel_storage_mem+vnl_vector_fixed+int.3--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bvxm_voxel_slab+vnl_vector_fixed+int.3--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bvxm_voxel_storage_disk+vnl_vector_fixed+int.3--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bvxm_voxel_storage_disk_cached+vnl_vector_fixed+int.3--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bvxm_voxel_grid+vnl_vector_fixed+int.3--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bvxm_voxel_storage_disk_cached+int-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bvxm_voxel_storage_disk+int-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bvxm_voxel_slab+vnl_vector_fixed+double.2--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bvxm_voxel_slab_iterator+bsta_vsum_num_obs+bsta_von_mises+float.3---.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bvxm_voxel_storage_disk_cached+bsta_vsum_num_obs+bsta_von_mises+float.3---.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bvxm_voxel_slab_iterator+bsta_vsum_num_obs+bsta_von_mises+double.3---.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bvxm_voxel_storage_disk+bsta_num_obs+bsta_gaussian_sphere+float.3---.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bvxm_voxel_storage_mem+bsta_num_obs+bsta_gaussian_sphere+double.2---.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bvxm_voxel_slab_iterator+vnl_vector_fixed+double.2--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bvxm_voxel_storage_mem+bsta_vsum_num_obs+bsta_von_mises+float.3---.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bvxm_voxel_storage_mem+bsta_vsum_num_obs+bsta_von_mises+double.3---.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bvxm_voxel_grid+bsta_vsum_num_obs+bsta_von_mises+float.3---.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bvxm_voxel_storage_disk+bsta_num_obs+bsta_gaussian_sphere+float.2---.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bvxm_voxel_storage_disk_cached+bsta_vsum_num_obs+bsta_von_mises+double.3---.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bvxm_voxel_storage_disk+bsta_num_obs+bsta_gaussian_sphere+double.2---.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bvxm_voxel_storage_mem+bsta_num_obs+bsta_gaussian_sphere+float.3---.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bvxm_voxel_slab_iterator+vnl_vector_fixed+float.3--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bvxm_voxel_storage_disk_cached+bsta_num_obs+bsta_gaussian_sphere+float.3---.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bvxm_voxel_grid+bsta_num_obs+bsta_gaussian_sphere+float.2---.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bvxm_voxel_slab_iterator+vnl_vector_fixed+float.2--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bvxm_voxel_grid+bsta_num_obs+bsta_gaussian_sphere+double.2---.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bvxm_voxel_slab_iterator+vnl_vector_fixed+double.3--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bvxm_voxel_storage_disk_cached+bsta_num_obs+bsta_gaussian_sphere+double.2---.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bvxm_voxel_storage_disk+bsta_vsum_num_obs+bsta_von_mises+float.3---.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bvxm_voxel_storage_disk+bsta_num_obs+bsta_gaussian_sphere+double.3---.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bvxm_voxel_slab+vnl_vector_fixed+double.3--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bvxm_voxel_storage_disk_cached+bsta_num_obs+bsta_gaussian_sphere+double.3---.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bvxm_voxel_slab+bsta_vsum_num_obs+bsta_von_mises+float.3---.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bvxm_voxel_grid+bsta_vsum_num_obs+bsta_von_mises+double.3---.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bvxm_voxel_storage_disk+bsta_vsum_num_obs+bsta_von_mises+double.3---.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bvxm_voxel_storage_mem+bsta_num_obs+bsta_gaussian_sphere+float.2---.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bvxm_voxel_slab_iterator_base+bsta_num_obs+bsta_gaussian_sphere+double.2---.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bvxm_voxel_slab+bsta_num_obs+bsta_gaussian_sphere+float.2---.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bvxm_voxel_storage_disk_cached+bsta_num_obs+bsta_gaussian_sphere+float.2---.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bvxm_voxel_slab_iterator+bsta_num_obs+bsta_gaussian_sphere+float.2---.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bvxm_voxel_slab+bsta_num_obs+bsta_gaussian_sphere+double.2---.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bvxm_voxel_slab+bsta_vsum_num_obs+bsta_von_mises+double.3---.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bvxm_voxel_storage_mem+bsta_num_obs+bsta_gaussian_sphere+double.3---.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bvxm_voxel_slab_iterator+bsta_vsum_num_obs+bsta_gaussian_sphere+double.3---.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bvxm_voxel_slab+bsta_num_obs+bsta_gaussian_sphere+double.3---.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bvxm_voxel_slab+bsta_num_obs+bsta_gaussian_sphere+float.3---.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bvxm_voxel_slab+bsta_vsum_num_obs+bsta_gaussian_sphere+double.3---.cxx
# End Source File
# End Group
# Begin Group "Header Files"
# PROP Default_Filter "h;txx"
# Begin Source File
SOURCE=.\bvxm_voxel_storage_disk.h
# End Source File
# Begin Source File
SOURCE=.\bvxm_voxel_grid.h
# End Source File
# Begin Source File
SOURCE=.\bvxm_voxel_slab_iterator.h
# End Source File
# Begin Source File
SOURCE=.\bvxm_voxel_storage.h
# End Source File
# Begin Source File
SOURCE=.\bvxm_voxel_storage_disk.txx
# End Source File
# Begin Source File
SOURCE=.\bvxm_voxel_storage_mem.txx
# End Source File
# Begin Source File
SOURCE=.\bvxm_voxel_grid_base.h
# End Source File
# Begin Source File
SOURCE=.\bvxm_voxel_slab_base.h
# End Source File
# Begin Source File
SOURCE=.\bvxm_voxel_slab.h
# End Source File
# Begin Source File
SOURCE=.\bvxm_voxel_grid.txx
# End Source File
# Begin Source File
SOURCE=.\bvxm_voxel_storage_disk_cached.txx
# End Source File
# Begin Source File
SOURCE=.\bvxm_voxel_storage_mem.h
# End Source File
# Begin Source File
SOURCE=.\bvxm_voxel_storage_disk_cached.h
# End Source File
# Begin Source File
SOURCE=.\bvxm_memory_chunk.h
# End Source File
# Begin Source File
SOURCE=.\bvxm_voxel_slab_iterator.txx
# End Source File
# Begin Source File
SOURCE=.\bvxm_voxel_slab.txx
# End Source File
# Begin Source File
SOURCE=.\bvxm_voxel_grid_basic_ops.h
# End Source File
# Begin Source File
SOURCE=.\bvxm_opinion.h
# End Source File
# Begin Source File
SOURCE=.\bvxm_voxel_grid_opinion_basic_ops.h
# End Source File
# End Group
# End Target
# End Project
