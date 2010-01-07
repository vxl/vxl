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

# Begin Group "Source Files"
# PROP Default_Filter "cxx"
# Begin Source File
SOURCE=.\boxm_utils.cxx
# End Source File
# Begin Source File
SOURCE=.\boxm_mog_grey_processor.cxx
# End Source File
# Begin Source File
SOURCE=.\boxm_apm_traits.cxx
# End Source File
# Begin Source File
SOURCE=.\boxm_quad_scan_iterator.cxx
# End Source File
# Begin Source File
SOURCE=.\boxm_scene_base.cxx
# End Source File
# Begin Source File
SOURCE=.\boxm_raytrace_operations.cxx
# End Source File
# Begin Source File
SOURCE=.\boxm_rational_camera_utils.cxx
# End Source File
# Begin Source File
SOURCE=.\boxm_triangle_scan_iterator.cxx
# End Source File
# Begin Source File
SOURCE=.\boxm_simple_grey_processor.cxx
# End Source File
# Begin Source File
SOURCE=.\boxm_scene_parser.cxx
# End Source File
# Begin Source File
SOURCE=.\boxm_mob_grey_processor.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/boxm_cell_vis_graph_iterator+short.boxm_sample+BOXM_APM_MOG_GREY--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/boxm_block_vis_graph_iterator+boct_tree+short.vgl_point_3d+double---.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/boxm_block_vis_graph_iterator+boct_tree+short.boxm_sample+BOXM_APM_MOG_GREY---.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vbl_array_3d+boxm_block+boct_tree+short.boxm_sample+BOXM_APM_MOG_GREY---~-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/boct_tree+short.boxm_sample+BOXM_APM_MOG_GREY--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/boxm_block+boct_tree+short.boxm_sample+BOXM_APM_MOG_GREY---.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/boxm_cell_vis_graph_iterator+short.vgl_point_3d+double--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/boxm_scene+boct_tree+short.boxm_sample+BOXM_APM_MOG_GREY---.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/boxm_block_vis_graph_node+boct_tree+short.vgl_point_3d+double---.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/boct_tree_cell+short.boxm_sample+BOXM_APM_MOG_GREY--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vbl_smart_ptr+boxm_scene_base-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/boxm_block+boct_tree+short.vgl_point_3d+double---.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/boxm_scene+boct_tree+short.vgl_point_3d+double---.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vbl_array_3d+boxm_block+boct_tree+short.vgl_point_3d+double---~-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/boxm_sample+BOXM_APM_MOG_GREY-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/boxm_block_iterator+boct_tree+short.boxm_sample+BOXM_APM_MOG_GREY---.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/boxm_block_vis_graph_iterator+boct_tree+short.boxm_sample_multi_bin+BOXM_APM_MOG_GREY---.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/boct_tree_cell+short.boxm_sample_multi_bin+BOXM_APM_MOG_GREY--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/boxm_block_iterator+boct_tree+short.vgl_point_3d+double---.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/boxm_block+boct_tree+short.boxm_sample_multi_bin+BOXM_APM_MOG_GREY---.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/boxm_sample_multi_bin+BOXM_APM_MOG_GREY-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/boxm_scene+boct_tree+short.boxm_sample_multi_bin+BOXM_APM_MOG_GREY---.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/boct_tree+short.boxm_sample_multi_bin+BOXM_APM_MOG_GREY--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/boxm_block_iterator+boct_tree+short.boxm_sample_multi_bin+BOXM_APM_MOG_GREY---.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/boxm_cell_vis_graph_iterator+short.boxm_sample_multi_bin+BOXM_APM_MOG_GREY--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vbl_array_3d+boxm_block+boct_tree+short.boxm_sample_multi_bin+BOXM_APM_MOG_GREY---~-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/boxm_triangle_interpolation_iterator+float-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/boxm_block_iterator+boct_tree+short.boxm_sample+BOXM_APM_SIMPLE_GREY---.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/boct_tree+short.boxm_sample+BOXM_APM_SIMPLE_GREY--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/boct_tree_cell+short.boxm_sample+BOXM_APM_SIMPLE_GREY--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/boxm_scene+boct_tree+short.boxm_sample+BOXM_APM_SIMPLE_GREY---.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/boxm_sample+BOXM_APM_SIMPLE_GREY-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/boxm_block+boct_tree+short.boxm_sample+BOXM_APM_SIMPLE_GREY---.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/boxm_cell_vis_graph_iterator+short.boxm_sample+BOXM_APM_SIMPLE_GREY--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vbl_array_3d+boxm_block+boct_tree+short.boxm_sample+BOXM_APM_SIMPLE_GREY---~-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vbl_array_3d+vsl_b_ifstream~-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/boxm_block_vis_graph_iterator+boct_tree+short.boxm_sample+BOXM_APM_SIMPLE_GREY---.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/boxm_block_iterator+boct_tree+short.float--.cxx
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
SOURCE=.\Templates/boxm_sample+BOXM_APM_MOB_GREY-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/boxm_scene+boct_tree+short.int--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/boxm_block+boct_tree+short.boxm_sample+BOXM_APM_MOB_GREY---.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/boxm_scene+boct_tree+short.boxm_sample+BOXM_APM_MOB_GREY---.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/boxm_block_iterator+boct_tree+short.boxm_sample+BOXM_APM_MOB_GREY---.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/boxm_cell_vis_graph_iterator+short.boxm_sample+BOXM_APM_MOB_GREY--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/boct_tree+short.boxm_sample+BOXM_APM_MOB_GREY--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vbl_array_3d+boxm_block+boct_tree+short.int--~-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/boxm_block_vis_graph_iterator+boct_tree+short.boxm_sample+BOXM_APM_MOB_GREY---.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vbl_array_3d+boxm_block+boct_tree+short.boxm_sample+BOXM_APM_MOB_GREY---~-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/boxm_block_iterator+boct_tree+short.int--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/boxm_block+boct_tree+short.int--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/boct_tree_cell+short.boxm_sample+BOXM_APM_MOB_GREY--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vbl_array_3d+boxm_block+boct_tree+short.bsta_num_obs+bsta_gauss_f1---~-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/boxm_block+boct_tree+short.bsta_num_obs+bsta_gauss_f1---.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/boxm_block_iterator+boct_tree+short.bsta_num_obs+bsta_gauss_f1---.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/boct_tree_cell+short.bsta_num_obs+bsta_gauss_f1--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/boxm_scene+boct_tree+short.bsta_num_obs+bsta_gauss_f1---.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/boct_tree+short.bsta_num_obs+bsta_gauss_f1--.cxx
# End Source File
# End Group
# Begin Group "Header Files"
# PROP Default_Filter "h;txx"
# Begin Source File
SOURCE=.\boxm_cell_vis_graph_node.h
# End Source File
# Begin Source File
SOURCE=.\boxm_block_vis_graph_iterator.txx
# End Source File
# Begin Source File
SOURCE=.\boxm_block_vis_graph_node.h
# End Source File
# Begin Source File
SOURCE=.\boxm_block.h
# End Source File
# Begin Source File
SOURCE=.\boxm_neighb_graph_node.h
# End Source File
# Begin Source File
SOURCE=.\boxm_utils.h
# End Source File
# Begin Source File
SOURCE=.\boxm_scene.txx
# End Source File
# Begin Source File
SOURCE=.\boxm_sample.h
# End Source File
# Begin Source File
SOURCE=.\boxm_block_vis_graph_iterator.h
# End Source File
# Begin Source File
SOURCE=.\boxm_cell_vis_graph_iterator.h
# End Source File
# Begin Source File
SOURCE=.\boxm_block.txx
# End Source File
# Begin Source File
SOURCE=.\boxm_sample.txx
# End Source File
# Begin Source File
SOURCE=.\boxm_apm_traits.h
# End Source File
# Begin Source File
SOURCE=.\boxm_cell_vis_graph_iterator.txx
# End Source File
# Begin Source File
SOURCE=.\boxm_scene.h
# End Source File
# Begin Source File
SOURCE=.\boxm_mog_grey_processor.h
# End Source File
# Begin Source File
SOURCE=.\boxm_block_vis_graph_node.txx
# End Source File
# Begin Source File
SOURCE=.\boxm_scene_base.h
# End Source File
# Begin Source File
SOURCE=.\boxm_render_image.h
# End Source File
# Begin Source File
SOURCE=.\boxm_save_block_raw.h
# End Source File
# Begin Source File
SOURCE=.\boxm_update.h
# End Source File
# Begin Source File
SOURCE=.\boxm_refine.h
# End Source File
# Begin Source File
SOURCE=.\boxm_quad_scan_iterator.h
# End Source File
# Begin Source File
SOURCE=.\boxm_compute_volume_visibility.h
# End Source File
# Begin Source File
SOURCE=.\boxm_sample_multi_bin.txx
# End Source File
# Begin Source File
SOURCE=.\boxm_sample_multi_bin.h
# End Source File
# Begin Source File
SOURCE=.\boxm_triangle_interpolation_iterator.txx
# End Source File
# Begin Source File
SOURCE=.\boxm_update_triangle.h
# End Source File
# Begin Source File
SOURCE=.\boxm_triangle_interpolation_iterator.h
# End Source File
# Begin Source File
SOURCE=.\boxm_render_image_triangle.h
# End Source File
# Begin Source File
SOURCE=.\boxm_triangle_scan_iterator.h
# End Source File
# Begin Source File
SOURCE=.\boxm_rational_camera_utils.h
# End Source File
# Begin Source File
SOURCE=.\boxm_raytrace_operations.h
# End Source File
# Begin Source File
SOURCE=.\boxm_upload_mesh.h
# End Source File
# Begin Source File
SOURCE=.\boxm_fill_in_mesh.h
# End Source File
# Begin Source File
SOURCE=.\boxm_simple_grey_processor.h
# End Source File
# Begin Source File
SOURCE=.\boxm_update_sample.h
# End Source File
# Begin Source File
SOURCE=.\boxm_vis_implicit_sample.h
# End Source File
# Begin Source File
SOURCE=.\boxm_depth_image_triangle.h
# End Source File
# Begin Source File
SOURCE=.\boxm_save_scene_raw.h
# End Source File
# Begin Source File
SOURCE=.\boxm_scene_parser.h
# End Source File
# Begin Source File
SOURCE=.\boxm_mob_grey_processor.h
# End Source File
# Begin Source File
SOURCE=.\boxm_cell_data_traits.h
# End Source File
# End Group
# End Target
# End Project
