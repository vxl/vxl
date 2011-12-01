# Microsoft Developer Studio Project File - Name="boxm" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# variables to REPLACE
# 
# /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/core" /I "$(VXLROOT)/vcl/config.win32" /I "$(VXLROOT)/contrib/brl/bseg" == include path
#   -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE == compiler defines
#  == override in output directory
# boxm  == name of output library

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=boxm - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "boxm.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "boxm.mak" CFG="bmrf - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "boxm - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "boxm - Win32 Release" (based on "Win32 (x86) Static Library")
# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "boxm - Win32 Release"

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
# ADD CPP /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/core" /I "$(VXLROOT)/vcl/config.win32" /I "$(VXLROOT)/contrib/brl/bseg" -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE /D "boxm_EXPORTS"
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

!ELSEIF  "$(CFG)" == "boxm - Win32 Debug"

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
# ADD CPP /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/core" /I "$(VXLROOT)/vcl/config.win32" /I "$(VXLROOT)/contrib/brl/bseg" -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE /D "boxm_EXPORTS"
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

# Name "boxm - Win32 Release"
# Name "boxm - Win32 Debug"

# Begin Project
# Begin Target
# Begin Group "Source Files"
# PROP Default_Filter "cxx"
# Begin Source File
SOURCE=.\boxm_io_scene_base.cxx
# End Source File
# Begin Source File
SOURCE=.\boxm_scene_parser.cxx
# End Source File
# Begin Source File
SOURCE=.\boxm_scene_base.cxx
# End Source File
# Begin Source File
SOURCE=.\boxm_apm_traits.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/boxm_scene+boct_tree+short.int--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/boxm_block+boct_tree+short.bsta_num_obs+bsta_gauss_sf1---.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/boct_tree_cell+short.bsta_num_obs+bsta_gauss_sf1--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vbl_io_smart_ptr+boxm_scene_base-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/boct_tree+short.bsta_num_obs+bsta_gauss_sf1--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vbl_array_3d+boxm_block+boct_tree+short.float--~-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/boxm_block+boct_tree+short.float--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/boxm_scene+boct_tree+short.float--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vbl_array_3d+vsl_b_ifstream~-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vbl_array_3d+boxm_block+boct_tree+short.int--~-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vbl_smart_ptr+boxm_scene_base-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vbl_array_3d+boxm_block+boct_tree+short.bsta_num_obs+bsta_gauss_sf1---~-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/boxm_scene+boct_tree+short.bsta_num_obs+bsta_gauss_sf1---.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/boxm_block+boct_tree+short.vgl_point_3d+double---.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/boxm_block+boct_tree+short.int--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/brdb_value_t+boxm_scene_base_sptr-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/boxm_scene+boct_tree+short.vgl_point_3d+double---.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vbl_array_3d+boxm_block+boct_tree+short.vgl_point_3d+double---~-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/boxm_block_iterator+boct_tree+short.bsta_num_obs+bsta_gauss_sf1---.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/boxm_block_iterator+boct_tree+short.vgl_point_3d+double---.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/boxm_block_iterator+boct_tree+short.int--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/boxm_block_iterator+boct_tree+short.float--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/boxm_scene+boct_tree+short.bool--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vbl_array_3d+boxm_block+boct_tree+short.bool--~-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/boxm_block_iterator+boct_tree+short.bool--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/boxm_block+boct_tree+short.bool--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/boxm_block+boct_tree+short.char--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/boxm_block_iterator+boct_tree+short.char--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/boxm_scene+boct_tree+short.char--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vbl_array_3d+boxm_block+boct_tree+short.char--~-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/boct_tree+short.vnl_vector_fixed+float.10--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/boct_tree+short.vnl_vector_fixed+float.3--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/boxm_block+boct_tree+short.vnl_vector_fixed+float.10---.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/boxm_block+boct_tree+short.vnl_vector_fixed+float.3---.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/boct_tree+short.vnl_vector_fixed+double.10--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/boxm_block+boct_tree+short.vnl_vector_fixed+double.10---.cxx
# End Source File
# End Group
# Begin Group "Header Files"
# PROP Default_Filter "h;txx"
# Begin Source File
SOURCE=.\boxm_aux_scene.h
# End Source File
# Begin Source File
SOURCE=.\boxm_block.h
# End Source File
# Begin Source File
SOURCE=.\boxm_scene.txx
# End Source File
# Begin Source File
SOURCE=.\boxm_scene_base.h
# End Source File
# Begin Source File
SOURCE=.\boxm_block.txx
# End Source File
# Begin Source File
SOURCE=.\boxm_apm_traits.h
# End Source File
# Begin Source File
SOURCE=.\boxm_io_scene_base.h
# End Source File
# Begin Source File
SOURCE=.\boxm_aux_traits.h
# End Source File
# Begin Source File
SOURCE=.\boxm_scene.h
# End Source File
# Begin Source File
SOURCE=.\boxm_scene_parser.h
# End Source File
# Begin Source File
SOURCE=.\boxm_aux_scene.txx
# End Source File
# End Group
# End Target
# End Project
