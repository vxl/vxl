# Microsoft Developer Studio Project File - Name="boxm_algo_sp" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# variables to REPLACE
# 
# /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/core" /I "$(VXLROOT)/vcl/config.win32" /I "$(VXLROOT)/contrib/brl/bseg" == include path
#   -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE == compiler defines
#  == override in output directory
# boxm_algo_sp  == name of output library

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=boxm_algo_sp - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "boxm_algo_sp.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "boxm_algo_sp.mak" CFG="bmrf - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "boxm_algo_sp - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "boxm_algo_sp - Win32 Release" (based on "Win32 (x86) Static Library")
# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "boxm_algo_sp - Win32 Release"

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
# ADD CPP /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/core" /I "$(VXLROOT)/vcl/config.win32" /I "$(VXLROOT)/contrib/brl/bseg" -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE /D "boxm_algo_sp_EXPORTS"
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

!ELSEIF  "$(CFG)" == "boxm_algo_sp - Win32 Debug"

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
# ADD CPP /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/core" /I "$(VXLROOT)/vcl/config.win32" /I "$(VXLROOT)/contrib/brl/bseg" -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE /D "boxm_algo_sp_EXPORTS"
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

# Name "boxm_algo_sp - Win32 Release"
# Name "boxm_algo_sp - Win32 Debug"

# Begin Group "Source Files"
# PROP Default_Filter "cxx"
# Begin Source File
SOURCE=.\dummy.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/boxm_opt_rt_bayesian_optimizer+short.BOXM_APM_MOG_GREY.BOXM_AUX_OPT_RT_GREY-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/boxm_edge_tangent_updater+short.float.float-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/boxm_edge_updater+short.float.float-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/boxm_opt3_optimizer+short.BOXM_APM_MOG_GREY.BOXM_APM_MOG_GREY-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/boxm_opt2_optimizer+short.BOXM_APM_SIMPLE_GREY.BOXM_AUX_OPT2_GREY-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/boxm_opt_rt_bayesian_optimizer+short.BOXM_APM_SIMPLE_GREY.BOXM_AUX_OPT_RT_GREY-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/boxm_opt2_optimizer+short.BOXM_APM_MOG_GREY.BOXM_AUX_OPT2_GREY-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/boxm_opt3_optimizer+short.BOXM_APM_SIMPLE_GREY.BOXM_APM_MOG_GREY-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bvpgl_camera_estimator+boxm_expected_edge_functor+short.boxm_inf_line_sample+float---.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/boxm_shadow_bayes_optimizer+short.BOXM_APM_SIMPLE_GREY.BOXM_AUX_OPT_RT_GREY-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/boxm_shadow_app_initializer+short.BOXM_APM_SIMPLE_GREY.BOXM_AUX_OPT_RT_GREY-.cxx
# End Source File
# End Group
# Begin Group "Header Files"
# PROP Default_Filter "h;txx"
# Begin Source File
SOURCE=.\boxm_render_expected_shadow_image_functor.h
# End Source File
# Begin Source File
SOURCE=.\boxm_generate_edge_tangent_sample_functor.h
# End Source File
# Begin Source File
SOURCE=.\boxm_generate_opt2_sample_functor.h
# End Source File
# Begin Source File
SOURCE=.\boxm_change_probability_functor.h
# End Source File
# Begin Source File
SOURCE=.\boxm_opt3_optimizer.txx
# End Source File
# Begin Source File
SOURCE=.\boxm_opt_appearance_estimator.h
# End Source File
# Begin Source File
SOURCE=.\boxm_update_image_functor.h
# End Source File
# Begin Source File
SOURCE=.\boxm_rpc_registration.h
# End Source File
# Begin Source File
SOURCE=.\boxm_edge_tangent_updater.h
# End Source File
# Begin Source File
SOURCE=.\boxm_edge_updater.h
# End Source File
# Begin Source File
SOURCE=.\boxm_opt_rt_bayesian_optimizer.h
# End Source File
# Begin Source File
SOURCE=.\boxm_sun_vis_prob_functor.h
# End Source File
# Begin Source File
SOURCE=.\boxm_opt2_optimizer.h
# End Source File
# Begin Source File
SOURCE=.\boxm_generate_edge_sample_functor.h
# End Source File
# Begin Source File
SOURCE=.\boxm_render_expected_image_functor.h
# End Source File
# Begin Source File
SOURCE=.\boxm_generate_opt_sample_functor.h
# End Source File
# Begin Source File
SOURCE=.\boxm_pre_infinity_functor.h
# End Source File
# Begin Source File
SOURCE=.\boxm_render_expected_edge_tangent_image_functor.h
# End Source File
# Begin Source File
SOURCE=.\boxm_render_expected_edge_image_functor.h
# End Source File
# Begin Source File
SOURCE=.\boxm_render_expected_depth_functor.h
# End Source File
# Begin Source File
SOURCE=.\boxm_edge_tangent_updater.txx
# End Source File
# Begin Source File
SOURCE=.\boxm_opt_rt_bayesian_optimizer.txx
# End Source File
# Begin Source File
SOURCE=.\boxm_opt3_optimizer.h
# End Source File
# Begin Source File
SOURCE=.\boxm_opt2_optimizer.txx
# End Source File
# Begin Source File
SOURCE=.\boxm_compute_num_rays_statistics.h
# End Source File
# Begin Source File
SOURCE=.\boxm_edge_updater.txx
# End Source File
# Begin Source File
SOURCE=.\boxm_generate_opt3_sample_functor.h
# End Source File
# Begin Source File
SOURCE=.\boxm_change_3d_functor.h
# End Source File
# Begin Source File
SOURCE=.\boxm_expected_edge_functor.h
# End Source File
# Begin Source File
SOURCE=.\boxm_shadow_app_initializer.h
# End Source File
# Begin Source File
SOURCE=.\boxm_shadow_bayes_optimizer.txx
# End Source File
# Begin Source File
SOURCE=.\boxm_shadow_appearance_estimator.h
# End Source File
# Begin Source File
SOURCE=.\boxm_shadow_bayes_optimizer.h
# End Source File
# Begin Source File
SOURCE=.\boxm_shadow_app_initializer.txx
# End Source File
# Begin Source File
SOURCE=.\boxm_generate_shadow_sample_functor.h
# End Source File
# End Group
# End Target
# End Project
