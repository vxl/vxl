# Microsoft Developer Studio Project File - Name="bvpl_octree" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# variables to REPLACE
# 
# /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/core" /I "$(VXLROOT)/vcl/config.win32" /I "$(VXLROOT)/contrib/brl/bseg" == include path
#   -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE == compiler defines
#  == override in output directory
# bvpl_octree  == name of output library

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=bvpl_octree - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "bvpl_octree.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "bvpl_octree.mak" CFG="bmrf - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "bvpl_octree - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "bvpl_octree - Win32 Release" (based on "Win32 (x86) Static Library")
# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "bvpl_octree - Win32 Release"

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
# ADD CPP /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/core" /I "$(VXLROOT)/vcl/config.win32" /I "$(VXLROOT)/contrib/brl/bseg" -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE /D "bvpl_octree_EXPORTS"
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

!ELSEIF  "$(CFG)" == "bvpl_octree - Win32 Debug"

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
# ADD CPP /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/core" /I "$(VXLROOT)/vcl/config.win32" /I "$(VXLROOT)/contrib/brl/bseg" -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE /D "bvpl_octree_EXPORTS"
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

# Name "bvpl_octree - Win32 Release"
# Name "bvpl_octree - Win32 Debug"

# Begin Group "Source Files"
# PROP Default_Filter "cxx"
# Begin Source File
SOURCE=.\bvpl_scene_neighborhood_operator.cxx
# End Source File
# Begin Source File
SOURCE=.\bvpl_scene_statistics.cxx
# End Source File
# Begin Source File
SOURCE=.\bvpl_discover_pca_kernels.cxx
# End Source File
# Begin Source File
SOURCE=.\bvpl_taylor_basis.cxx
# End Source File
# Begin Source File
SOURCE=.\bvpl_pca_error_scenes.cxx
# End Source File
# Begin Source File
SOURCE=.\bvpl_taylor_scenes_map.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/boxm_block+boct_tree+short.bvpl_octree_sample+bsta_num_obs+bsta_gauss_f1----.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/boxm_block+boct_tree+short.bvpl_octree_sample+float---.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/boct_tree+short.bvpl_octree_sample+float--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/boct_tree+short.bvpl_octree_sample+bsta_num_obs+bsta_gauss_f1---.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/boxm_block+boct_tree+short.vnl_vector_fixed+float.3---.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/boct_tree+short.vnl_vector_fixed+float.3--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vbl_smart_ptr+bvpl_pca_error_scenes-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vbl_smart_ptr+bvpl_taylor_scenes_map-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vbl_smart_ptr+bvpl_discover_pca_kernels-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/boct_tree+short.bvpl_taylor_basis2_sample-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/boxm_block+boct_tree+short.bvpl_taylor_basis2_sample--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/boct_tree+short.bvpl_pca_basis_sample+10--.cxx
# End Source File
# End Group
# Begin Group "Header Files"
# PROP Default_Filter "h;txx"
# Begin Source File
SOURCE=.\bvpl_octree_vector_operator.h
# End Source File
# Begin Source File
SOURCE=.\bvpl_octree_kernel_operator.h
# End Source File
# Begin Source File
SOURCE=.\bvpl_scene_kernel_operator.h
# End Source File
# Begin Source File
SOURCE=.\bvpl_scene_vector_operator.h
# End Source File
# Begin Source File
SOURCE=.\bvpl_octree_vector_operator_impl.h
# End Source File
# Begin Source File
SOURCE=.\bvpl_octree_vrml_util.h
# End Source File
# Begin Source File
SOURCE=.\bvpl_cell_data_traits.h
# End Source File
# Begin Source File
SOURCE=.\bvpl_octree_neighbors.h
# End Source File
# Begin Source File
SOURCE=.\bvpl_scene_neighborhood_operator.h
# End Source File
# Begin Source File
SOURCE=.\bvpl_scene_statistics.h
# End Source File
# Begin Source File
SOURCE=.\bvpl_discover_pca_kernels.h
# End Source File
# Begin Source File
SOURCE=.\bvpl_taylor_basis.h
# End Source File
# Begin Source File
SOURCE=.\bvpl_block_kernel_operator.h
# End Source File
# Begin Source File
SOURCE=.\bvpl_pca_error_scenes.h
# End Source File
# Begin Source File
SOURCE=.\bvpl_taylor_scenes_map.h
# End Source File
# End Group
# End Target
# End Project
