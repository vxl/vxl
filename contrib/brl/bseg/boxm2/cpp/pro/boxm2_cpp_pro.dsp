# Microsoft Developer Studio Project File - Name="boxm2_cpp_pro" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# variables to REPLACE
#
# /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/core" /I "$(VXLROOT)/vcl/config.win32" /I "$(VXLROOT)/contrib/mul" == include path
#   -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE == compiler defines
#  == override in output directory
# boxm2_cpp_pro  == name of output library

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=boxm2_cpp_pro - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE
!MESSAGE NMAKE /f "boxm2_cpp_pro.mak".
!MESSAGE
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE
!MESSAGE NMAKE /f "boxm2_cpp_pro.mak" CFG="vil2 - Win32 Debug"
!MESSAGE
!MESSAGE Possible choices for configuration are:
!MESSAGE
!MESSAGE "boxm2_cpp_pro - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "boxm2_cpp_pro - Win32 Release" (based on "Win32 (x86) Static Library")
# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "boxm2_cpp_pro - Win32 Release"

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
# ADD CPP /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/core" /I "$(VXLROOT)/vcl/config.win32" /I "$(VXLROOT)/contrib/mul" -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE /D "boxm2_cpp_pro_EXPORTS"
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

!ELSEIF  "$(CFG)" == "boxm2_cpp_pro - Win32 Debug"

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
# ADD CPP /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/core" /I "$(VXLROOT)/vcl/config.win32" /I "$(VXLROOT)/contrib/mul" -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE /D "boxm2_cpp_pro_EXPORTS"
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

# Name "boxm2_cpp_pro - Win32 Release"
# Name "boxm2_cpp_pro - Win32 Debug"

# Begin Group "Source Files"
# PROP Default_Filter "cxx"
# Begin Source File
SOURCE=.\boxm2_cpp_register.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/boxm2_cpp_refine_process2.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/boxm2_cpp_render_expected_image_process.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/boxm2_cpp_update_image_process.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/boxm2_cpp_change_detection_process2.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/boxm2_cpp_render_cone_expected_image_process.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/boxm2_cpp_render_expected_depth_process.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/boxm2_cpp_cast_intensities_process.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/boxm2_cpp_mean_intensities_batch_process.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/boxm2_cpp_query_cell_data_process.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/boxm2_cpp_filter_process.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/boxm2_cpp_ray_app_density_process.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/boxm2_cpp_ray_probe_process.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/boxm2_cpp_cone_update_image_process.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/boxm2_cpp_batch_update_processes.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/boxm2_cpp_vis_of_point_process.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/boxm2_cpp_batch_update_opt2_processes.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/boxm2_cpp_merge_process.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/boxm2_cpp_image_density_process.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/boxm2_cpp_update_with_shadow_process.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/boxm2_cpp_batch_compute_shadow_model_process.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/boxm2_cpp_render_z_images_process.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/boxm2_cpp_batch_compute_phong_model_process.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/boxm2_cpp_batch_probe_intensities_process.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/boxm2_cpp_batch_update_nonsurface_model_process.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/boxm2_cpp_batch_update_opt2_phongs_processes.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/boxm2_cpp_query_cell_brdf_process.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/boxm2_cpp_update_using_quality_process.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/boxm2_cpp_batch_compute_synoptic_function_1d_process.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/boxm2_cpp_compute_derivative_process.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/boxm2_cpp_filter_response_process.cxx
# End Source File
# End Group
# Begin Group "Header Files"
# PROP Default_Filter "h;txx"
# Begin Source File
SOURCE=.\boxm2_cpp_register.h
# End Source File
# Begin Source File
SOURCE=.\boxm2_cpp_processes.h
# End Source File
# End Group
# End Target
# End Project
