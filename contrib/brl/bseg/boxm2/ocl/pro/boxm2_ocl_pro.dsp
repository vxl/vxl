# Microsoft Developer Studio Project File - Name="boxm2_ocl_pro" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# variables to REPLACE
#
# /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/core" /I "$(VXLROOT)/vcl/config.win32" /I "$(VXLROOT)/contrib/mul" == include path
#   -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE == compiler defines
#  == override in output directory
# boxm2_ocl_pro  == name of output library

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=boxm2_ocl_pro - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE
!MESSAGE NMAKE /f "boxm2_ocl_pro.mak".
!MESSAGE
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE
!MESSAGE NMAKE /f "boxm2_ocl_pro.mak" CFG="vil2 - Win32 Debug"
!MESSAGE
!MESSAGE Possible choices for configuration are:
!MESSAGE
!MESSAGE "boxm2_ocl_pro - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "boxm2_ocl_pro - Win32 Release" (based on "Win32 (x86) Static Library")
# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "boxm2_ocl_pro - Win32 Release"

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
# ADD CPP /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/core" /I "$(VXLROOT)/vcl/config.win32" /I "$(VXLROOT)/contrib/mul" -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE /D "boxm2_ocl_pro_EXPORTS"
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

!ELSEIF  "$(CFG)" == "boxm2_ocl_pro - Win32 Debug"

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
# ADD CPP /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/core" /I "$(VXLROOT)/vcl/config.win32" /I "$(VXLROOT)/contrib/mul" -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE /D "boxm2_ocl_pro_EXPORTS"
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

# Name "boxm2_ocl_pro - Win32 Release"
# Name "boxm2_ocl_pro - Win32 Debug"

# Begin Group "Source Files"
# PROP Default_Filter "cxx"
# Begin Source File
SOURCE=.\boxm2_ocl_register.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/boxm2_ocl_render_expected_image_process.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/boxm2_create_opencl_cache_process.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/boxm2_ocl_query_hist_data_process.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/boxm2_ocl_update_histogram_process.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/boxm2_ocl_batch_probability_process.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/boxm2_ocl_query_cell_data_process.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/boxm2_ocl_render_expected_depth_process.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/boxm2_ocl_update_process.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/boxm2_ocl_refine_process.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/boxm2_ocl_render_gl_expected_image_process.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/boxm2_ocl_render_expected_height_map_process.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/boxm2_ocl_update_color_process.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/boxm2_ocl_render_expected_color_process.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/boxm2_ocl_render_gl_expected_color_process.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/boxm2_ocl_change_detection_process.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/boxm2_ocl_convert_float_image_to_rgba_process.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/boxm2_ocl_probability_of_image_process.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/boxm2_ocl_probability_of_image_gl_process.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/boxm2_ocl_render_cone_expected_image_process.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/boxm2_ocl_filter_process.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/boxm2_clear_opencl_cache_process.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/boxm2_ocl_cone_update_process.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/boxm2_ocl_update_aux_per_view_process.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/boxm2_ocl_adaptive_cone_update_process.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/boxm2_ocl_adaptive_cone_render_expected_process.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/boxm2_ocl_merge_process.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/boxm2_ocl_render_expected_shadow_map_process.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/boxm2_ocl_update_sun_visibilities_process.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/boxm2_ocl_aux_update_view_direction_process.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/boxm2_ocl_create_aux_process.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/boxm2_ocl_create_norm_intensities_process.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/boxm2_ocl_ingest_dem_process.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/boxm2_ocl_visualize_change_process.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/boxm2_ocl_paint_batch_process.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/boxm2_ocl_batch_synoptic_function_process.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/boxm2_ocl_flip_normals_using_vis_process.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/boxm2_ocl_paint_online_process.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/boxm2_ocl_render_expected_albedo_normal_process.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/boxm2_ocl_ingest_buckeye_process.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/boxm2_ocl_update_alpha_naa_process.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/boxm2_ocl_render_expected_z_image_process.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/boxm2_ocl_render_expected_image_naa_process.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/boxm2_ocl_update_alpha_wcubic_process.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/boxm2_ocl_batch_uncertainty_process.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/boxm2_ocl_batch_compute_normal_albedo_array_process.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/boxm2_ocl_update_aux_per_view_naa_process.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/boxm2_ocl_update_with_alt_process.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/boxm2_ocl_uncertainty_per_image_process.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/boxm2_ocl_aggregate_normal_from_filter_response_process.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/boxm2_ocl_synoptic_update_alpha_process.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/boxm2_ocl_render_scene_uncertainty_map_process.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/boxm2_ocl_kernel_filter_process.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/boxm2_ocl_aggregate_normal_from_filter_vector_process.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/boxm2_ocl_kernel_vector_filter_process.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/boxm2_ocl_batch_synoptic_phongs_process.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/boxm2_ocl_update_alpha_wphongs_process.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/boxm2_ocl_compute_visibility_process.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/boxm2_ocl_probability_of_image_wcubic_process.cxx
# End Source File
# End Group
# Begin Group "Header Files"
# PROP Default_Filter "h;txx"
# Begin Source File
SOURCE=.\boxm2_ocl_register.h
# End Source File
# Begin Source File
SOURCE=.\boxm2_ocl_processes.h
# End Source File
# Begin Source File
SOURCE=.\processes/boxm2_ocl_filter_process.h
# End Source File
# Begin Source File
SOURCE=.\processes/boxm2_ocl_kernel_vector_filter_process.h
# End Source File
# Begin Source File
SOURCE=.\processes/boxm2_ocl_kernel_filter_process.h
# End Source File
# Begin Source File
SOURCE=.\processes/boxm2_ocl_change_detection_process2.cxx
# End Source File
# End Group
# End Target
# End Project
