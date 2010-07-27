# Microsoft Developer Studio Project File - Name="boxm_ocl" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# variables to REPLACE
# 
# /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/core" /I "$(VXLROOT)/vcl/config.win32" /I "$(VXLROOT)/contrib/brl/bseg" == include path
#   -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE == compiler defines
#  == override in output directory
# boxm_ocl  == name of output library

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=boxm_ocl - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "boxm_ocl.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "boxm_ocl.mak" CFG="bmrf - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "boxm_ocl - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "boxm_ocl - Win32 Release" (based on "Win32 (x86) Static Library")
# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "boxm_ocl - Win32 Release"

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
# ADD CPP /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/core" /I "$(VXLROOT)/vcl/config.win32" /I "$(VXLROOT)/contrib/brl/bseg" -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE /D "boxm_ocl_EXPORTS"
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

!ELSEIF  "$(CFG)" == "boxm_ocl - Win32 Debug"

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
# ADD CPP /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/core" /I "$(VXLROOT)/vcl/config.win32" /I "$(VXLROOT)/contrib/brl/bseg" -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE /D "boxm_ocl_EXPORTS"
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

# Name "boxm_ocl - Win32 Release"
# Name "boxm_ocl - Win32 Debug"

# Begin Project
# Begin Target
# Begin Group "Source Files"
# PROP Default_Filter "cxx"
# Begin Source File
SOURCE=.\boxm_ocl_utils.cxx
# End Source File
# Begin Source File
SOURCE=.\octree_library_functions.cl
# End Source File
# Begin Source File
SOURCE=.\expected_functor.cl
# End Source File
# Begin Source File
SOURCE=.\ray_trace_main.cl
# End Source File
# Begin Source File
SOURCE=.\ray_stack_trace_main.cl
# End Source File
# Begin Source File
SOURCE=.\refine_main.cl
# End Source File
# Begin Source File
SOURCE=.\ray_bundle_library_functions.cl
# End Source File
# Begin Source File
SOURCE=.\update_main.cl
# End Source File
# Begin Source File
SOURCE=.\boxm_render_single_block_manager.cxx
# End Source File
# Begin Source File
SOURCE=.\boxm_online_update_manager.cxx
# End Source File
# Begin Source File
SOURCE=.\boxm_refine_scene_manager.cxx
# End Source File
# Begin Source File
SOURCE=.\boxm_ocl_scene.cxx
# End Source File
# Begin Source File
SOURCE=.\boxm_render_ocl_scene_manager.cxx
# End Source File
# Begin Source File
SOURCE=.\boxm_update_ocl_scene_manager.cxx
# End Source File
# Begin Source File
SOURCE=.\boxm_ocl_render_expected.cxx
# End Source File
# Begin Source File
SOURCE=.\boxm_change_detection_ocl_scene_manager.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bocl_manager+boxm_stack_ray_trace_manager+boxm_sample+BOXM_APM_SIMPLE_GREY---.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bocl_manager+boxm_stack_ray_trace_manager+boxm_sample+BOXM_APM_MOG_GREY---.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bocl_manager+boxm_ray_trace_manager+boxm_sample+BOXM_APM_SIMPLE_GREY---.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/boxm_ray_trace_manager+boxm_sample+BOXM_APM_MOG_GREY--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bocl_manager+boxm_ray_trace_manager+boxm_sample+BOXM_APM_MOG_GREY---.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vbl_io_array_2d+vnl_vector_fixed+float.3--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vnl_vector_fixed+float.16-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/boxm_ray_trace_manager+boxm_sample+BOXM_APM_SIMPLE_GREY--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/boxm_ray_trace_manager+float-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vbl_array_2d+vnl_vector_fixed+float.3--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bocl_manager+boxm_ray_trace_manager+float--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/boxm_stack_ray_trace_manager+boxm_sample+BOXM_APM_SIMPLE_GREY--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/boxm_stack_ray_trace_manager+boxm_sample+BOXM_APM_MOG_GREY--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/boxm_ocl_utils+boxm_sample+BOXM_APM_MOG_GREY--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/boxm_ocl_utils+float-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/boxm_stat_manager+boxm_sample+BOXM_APM_MOG_GREY--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/boxm_stat_manager+float-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bocl_manager+boxm_stat_manager+boxm_sample+BOXM_APM_MOG_GREY---.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/boxm_ocl_utils+boxm_sample+BOXM_APM_SIMPLE_GREY--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bocl_manager+boxm_stat_manager+float--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/boxm_refine_manager+boxm_sample+BOXM_APM_SIMPLE_GREY--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bocl_manager+boxm_ray_bundle_trace_manager+boxm_sample+BOXM_APM_MOG_GREY---.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/boxm_ray_bundle_trace_manager+boxm_sample+BOXM_APM_MOG_GREY--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bocl_manager+boxm_refine_manager+boxm_sample+BOXM_APM_MOG_GREY---.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/boxm_ray_bundle_trace_manager+boxm_sample+BOXM_APM_SIMPLE_GREY--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bocl_manager+boxm_refine_manager+boxm_sample+BOXM_APM_SIMPLE_GREY---.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/boxm_refine_manager+boxm_sample+BOXM_APM_MOG_GREY--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/boxm_refine_manager+float-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bocl_manager+boxm_ray_bundle_trace_manager+boxm_sample+BOXM_APM_SIMPLE_GREY---.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bocl_manager+boxm_refine_manager+float--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/boxm_render_image_manager+boxm_sample+BOXM_APM_MOG_GREY--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bocl_manager+boxm_render_image_manager+float--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/boxm_render_image_manager+float-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/boxm_render_image_manager+boxm_sample+BOXM_APM_SIMPLE_GREY--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bocl_manager+boxm_render_image_manager+boxm_sample+BOXM_APM_MOG_GREY---.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bocl_manager+boxm_render_image_manager+boxm_sample+BOXM_APM_SIMPLE_GREY---.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bocl_manager+boxm_online_update_manager-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bocl_manager+boxm_render_single_block_manager-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bocl_manager+boxm_refine_scene_manager-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vbl_array_1d+vnl_vector_fixed+int.2--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vbl_array_3d+vnl_vector_fixed+int.4--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vbl_io_array_3d+vnl_vector_fixed+int.4--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vnl_io_vector_fixed+int.4-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vbl_array_2d+vnl_vector_fixed+int.4--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vbl_array_2d+vnl_vector_fixed+float.16--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vbl_io_array_2d+vnl_vector_fixed+float.16--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vnl_io_vector_fixed+float.16-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vnl_io_vector_fixed+int.2-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vbl_io_array_1d+vnl_vector_fixed+int.2--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vbl_io_array_2d+vnl_vector_fixed+int.4--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bocl_manager+boxm_update_ocl_scene_manager-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bocl_manager+boxm_render_ocl_scene_manager-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bocl_manager+boxm_change_detection_ocl_scene_manager-.cxx
# End Source File
# End Group
# Begin Group "Header Files"
# PROP Default_Filter "h;txx"
# Begin Source File
SOURCE=.\boxm_ray_trace_manager.txx
# End Source File
# Begin Source File
SOURCE=.\boxm_ocl_render_expected.h
# End Source File
# Begin Source File
SOURCE=.\boxm_ray_trace_manager.h
# End Source File
# Begin Source File
SOURCE=.\boxm_ocl_utils.h
# End Source File
# Begin Source File
SOURCE=.\boxm_stack_ray_trace_manager.h
# End Source File
# Begin Source File
SOURCE=.\boxm_stack_ray_trace_manager.txx
# End Source File
# Begin Source File
SOURCE=.\boxm_stat_manager.txx
# End Source File
# Begin Source File
SOURCE=.\boxm_stat_manager.h
# End Source File
# Begin Source File
SOURCE=.\boxm_ocl_utils.txx
# End Source File
# Begin Source File
SOURCE=.\boxm_ocl_refine_scene.h
# End Source File
# Begin Source File
SOURCE=.\boxm_ray_bundle_trace_manager.h
# End Source File
# Begin Source File
SOURCE=.\boxm_refine_manager.h
# End Source File
# Begin Source File
SOURCE=.\boxm_ray_bundle_trace_manager.txx
# End Source File
# Begin Source File
SOURCE=.\boxm_refine_manager.txx
# End Source File
# Begin Source File
SOURCE=.\boxm_render_image_manager.h
# End Source File
# Begin Source File
SOURCE=.\boxm_render_image_manager.txx
# End Source File
# Begin Source File
SOURCE=.\boxm_online_update_manager.h
# End Source File
# Begin Source File
SOURCE=.\boxm_render_single_block_manager.h
# End Source File
# Begin Source File
SOURCE=.\boxm_refine_scene_manager.h
# End Source File
# Begin Source File
SOURCE=.\boxm_ocl_scene.h
# End Source File
# Begin Source File
SOURCE=.\boxm_update_ocl_scene_manager.h
# End Source File
# Begin Source File
SOURCE=.\boxm_render_ocl_scene_manager.h
# End Source File
# Begin Source File
SOURCE=.\boxm_change_detection_ocl_scene_manager.h
# End Source File
# End Group
# End Target
# End Project
