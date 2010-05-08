# Microsoft Developer Studio Project File - Name="boxm2_sample" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# variables to REPLACE
# 
# /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/core" /I "$(VXLROOT)/vcl/config.win32" /I "$(VXLROOT)/contrib/brl/bseg" == include path
#   -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE == compiler defines
#  == override in output directory
# boxm2_sample  == name of output library

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=boxm2_sample - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "boxm2_sample.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "boxm2_sample.mak" CFG="bmrf - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "boxm2_sample - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "boxm2_sample - Win32 Release" (based on "Win32 (x86) Static Library")
# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "boxm2_sample - Win32 Release"

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
# ADD CPP /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/core" /I "$(VXLROOT)/vcl/config.win32" /I "$(VXLROOT)/contrib/brl/bseg" -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE /D "boxm2_sample_EXPORTS"
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

!ELSEIF  "$(CFG)" == "boxm2_sample - Win32 Debug"

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
# ADD CPP /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/core" /I "$(VXLROOT)/vcl/config.win32" /I "$(VXLROOT)/contrib/brl/bseg" -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE /D "boxm2_sample_EXPORTS"
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

# Name "boxm2_sample - Win32 Release"
# Name "boxm2_sample - Win32 Debug"

# Begin Project
# Begin Target
# Begin Group "Source Files"
# PROP Default_Filter "cxx"
# Begin Source File
SOURCE=.\dummy.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vbl_array_3d+boxm_block+boct_tree+short.boxm_rt_sample+float---~-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/boct_tree_cell+short.boxm_sample_multi_bin+BOXM_APM_MOG_GREY--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/boxm_sample+BOXM_APM_MOB_GREY-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/boxm_block+boct_tree+short.boxm_sample_multi_bin+BOXM_APM_MOG_GREY---.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/boxm_aux_scene+short.boxm_sample+BOXM_APM_MOB_GREY-.boxm_rt_sample+float--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/boxm_block+boct_tree+short.boxm_sample+BOXM_APM_MOB_GREY---.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/boxm_block+boct_tree+short.boxm_opt2_sample+float---.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/boxm_edge_sample+float-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/boxm_aux_scene+short.boxm_edge_sample+float-.boxm_aux_edge_sample+float--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/boct_tree_cell+short.boxm_rt_sample+float--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/boct_tree+short.boxm_opt2_sample+float--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/boxm_aux_scene+short.boxm_sample+BOXM_APM_SIMPLE_GREY-.boxm_rt_sample+float--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/boct_tree_cell+short.boxm_opt3_sample+BOXM_APM_MOG_GREY--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/boxm_block_vis_graph_iterator+boct_tree+short.boxm_edge_tangent_sample+float---.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/boxm_block_iterator+boct_tree+short.boxm_edge_tangent_sample+float---.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/boct_tree_cell+short.boxm_edge_sample+float--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vbl_array_3d+boxm_block+boct_tree+short.boxm_scalar_sample+float---~-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/boxm_aux_scene+short.boxm_sample+BOXM_APM_MOG_GREY-.boxm_rt_sample+float--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/boxm_block+boct_tree+short.boxm_scalar_sample+float---.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/boxm_cell_vis_graph_iterator+short.boxm_edge_tangent_sample+float--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/boxm_sample+BOXM_APM_MOG_GREY-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/boxm_inf_line_sample+float-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/boct_tree_cell_reader+short.boxm_sample+BOXM_APM_MOB_GREY--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/boxm_aux_scene+short.boxm_sample+BOXM_APM_SIMPLE_GREY-.boxm_scalar_sample+float--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/boxm_sample+BOXM_APM_SIMPLE_GREY-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/boxm_plane_obs+float-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/boxm_block+boct_tree+short.boxm_rt_sample+float---.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vbl_array_3d+boxm_block+boct_tree+short.boxm_opt2_sample+float---~-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/boct_tree+short.boxm_edge_sample+float--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vbl_array_3d+boxm_block+boct_tree+short.boxm_sample+BOXM_APM_MOG_GREY---~-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/boct_tree_cell_reader+short.boxm_scalar_sample+float--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/boct_tree+short.boxm_sample+BOXM_APM_MOG_GREY--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/boct_tree+short.boxm_edge_tangent_sample+float--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/boxm_block+boct_tree+short.boxm_sample+BOXM_APM_MOG_GREY---.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/boxm_scene+boct_tree+short.boxm_opt3_sample+BOXM_APM_MOG_GREY---.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/boct_tree_cell_reader+short.boxm_inf_line_sample+float--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/boxm_cell_vis_graph_iterator+short.boxm_inf_line_sample+float--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/boct_tree_cell_reader+short.boxm_opt2_sample+float--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/boct_tree_cell+short.boxm_sample+BOXM_APM_SIMPLE_GREY--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/boxm_block_iterator+boct_tree+short.boxm_edge_sample+float---.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/boxm_aux_scene+short.boxm_sample+BOXM_APM_MOG_GREY-.boxm_opt2_sample+float--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/boct_tree_cell_reader+short.boxm_edge_tangent_sample+float--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/boct_tree+short.boxm_sample+BOXM_APM_MOB_GREY--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/boxm_aux_scene+short.boxm_sample+BOXM_APM_SIMPLE_GREY-.boxm_opt2_sample+float--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vbl_array_3d+boxm_block+boct_tree+short.boxm_sample+BOXM_APM_SIMPLE_GREY---~-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/boxm_edge_tangent_sample+float-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/boct_tree_cell+short.boxm_edge_tangent_sample+float--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/boxm_scene+boct_tree+short.boxm_rt_sample+float---.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vbl_array_3d+boxm_block+boct_tree+short.boxm_sample_multi_bin+BOXM_APM_MOG_GREY---~-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/boxm_block+boct_tree+short.boxm_edge_sample+float---.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/boct_tree_cell+short.boxm_opt2_sample+float--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/boxm_block+boct_tree+short.boxm_edge_tangent_sample+float---.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vil_image_view+boxm_inf_line_sample+float--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/boxm_opt3_sample+BOXM_APM_MOG_GREY-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/boxm_block+boct_tree+short.boxm_opt3_sample+BOXM_APM_MOG_GREY---.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/boxm_scene+boct_tree+short.boxm_opt2_sample+float---.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/boct_tree_cell+short.boxm_sample+BOXM_APM_MOG_GREY--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/boct_tree_cell_reader+short.boxm_edge_sample+float--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/boct_tree+short.boxm_sample+BOXM_APM_SIMPLE_GREY--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/boxm_aux_scene+short.boxm_sample+BOXM_APM_MOG_GREY-.boxm_opt3_sample+BOXM_APM_MOG_GREY--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/boxm_aux_scene+short.boxm_sample+BOXM_APM_SIMPLE_GREY-.float-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/boct_tree+short.boxm_rt_sample+float--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/boxm_opt2_sample+float-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/boct_tree_cell_reader+short.boxm_sample+BOXM_APM_MOG_GREY--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vbl_array_3d+boxm_block+boct_tree+short.boxm_opt3_sample+BOXM_APM_MOG_GREY---~-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vbl_array_3d+boxm_block+boct_tree+short.boxm_sample+BOXM_APM_MOB_GREY---~-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/boxm_rt_sample+float-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/boxm_sample_multi_bin+BOXM_APM_MOG_GREY-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/boct_tree_cell_reader+short.boxm_rt_sample+float--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/boxm_aux_scene+short.float.boxm_edge_tangent_sample+float--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/boxm_aux_scene+short.boxm_sample+BOXM_APM_SIMPLE_GREY-.boxm_opt3_sample+BOXM_APM_MOG_GREY--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/boct_tree_cell+short.boxm_scalar_sample+float--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/boxm_scene+boct_tree+short.boxm_edge_tangent_sample+float---.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/boxm_aux_scene+short.boxm_sample+BOXM_APM_MOG_GREY-.float-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vbl_array_3d+boxm_block+boct_tree+short.boxm_edge_tangent_sample+float---~-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/boxm_cell_vis_graph_iterator+short.boxm_edge_sample+float--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/boxm_scalar_sample+float-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/boxm_block_vis_graph_iterator+boct_tree+short.boxm_edge_sample+float---.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/boct_tree+short.boxm_sample_multi_bin+BOXM_APM_MOG_GREY--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/boct_tree_cell_reader+short.boxm_opt3_sample+BOXM_APM_MOG_GREY--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/boct_tree+short.boxm_opt3_sample+BOXM_APM_MOG_GREY--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/boct_tree_cell+short.boxm_sample+BOXM_APM_MOB_GREY--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vbl_array_3d+boxm_block+boct_tree+short.boxm_edge_sample+float---~-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/boxm_aux_scene+short.boxm_inf_line_sample+float-.boxm_edge_tangent_sample+float--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/boct_tree+short.boxm_scalar_sample+float--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/boxm_block+boct_tree+short.boxm_sample+BOXM_APM_SIMPLE_GREY---.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/boxm_scene+boct_tree+short.boxm_edge_sample+float---.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/boxm_scene+boct_tree+short.int--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/boxm_scene+boct_tree+short.boxm_scalar_sample+float---.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/boxm_scene+boct_tree+short.boxm_sample+BOXM_APM_MOB_GREY---.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/boxm_scene+boct_tree+short.boxm_sample_multi_bin+BOXM_APM_MOG_GREY---.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/boxm_scene+boct_tree+short.float--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/boxm_scene+boct_tree+short.boxm_sample+BOXM_APM_MOG_GREY---.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/boxm_scene+boct_tree+short.boxm_sample+BOXM_APM_SIMPLE_GREY---.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/boxm_scene+boct_tree+short.bsta_num_obs+bsta_gauss_f1---.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/boxm_scene+boct_tree+short.vgl_point_3d+double---.cxx
# End Source File
# End Group
# Begin Group "Header Files"
# PROP Default_Filter "h;txx"
# Begin Source File
SOURCE=.\boxm_plane_obs.txx
# End Source File
# Begin Source File
SOURCE=.\boxm_plane_obs.h
# End Source File
# Begin Source File
SOURCE=.\boxm_edge_sample.h
# End Source File
# Begin Source File
SOURCE=.\boxm_rt_sample.txx
# End Source File
# Begin Source File
SOURCE=.\boxm_opt3_sample.h
# End Source File
# Begin Source File
SOURCE=.\boxm_sample_multi_bin.txx
# End Source File
# Begin Source File
SOURCE=.\boxm_opt3_sample.txx
# End Source File
# Begin Source File
SOURCE=.\boxm_opt2_sample.h
# End Source File
# Begin Source File
SOURCE=.\boxm_sample.h
# End Source File
# Begin Source File
SOURCE=.\boxm_rt_sample.h
# End Source File
# Begin Source File
SOURCE=.\boxm_opt2_sample.txx
# End Source File
# Begin Source File
SOURCE=.\boxm_sample.txx
# End Source File
# Begin Source File
SOURCE=.\boxm_inf_line_sample.h
# End Source File
# Begin Source File
SOURCE=.\boxm_vis_implicit_sample.h
# End Source File
# Begin Source File
SOURCE=.\boxm_edge_tangent_sample.txx
# End Source File
# Begin Source File
SOURCE=.\boxm_edge_sample.txx
# End Source File
# Begin Source File
SOURCE=.\boxm_scalar_sample.txx
# End Source File
# Begin Source File
SOURCE=.\boxm_inf_line_sample.txx
# End Source File
# Begin Source File
SOURCE=.\boxm_edge_tangent_sample.h
# End Source File
# Begin Source File
SOURCE=.\boxm_sample_multi_bin.h
# End Source File
# Begin Source File
SOURCE=.\boxm_scalar_sample.h
# End Source File
# Begin Source File
SOURCE=.\boxm_update_sample.h
# End Source File
# End Group
# End Target
# End Project
