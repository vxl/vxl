# Microsoft Developer Studio Project File - Name="bsta_algo" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# variables to REPLACE
# 
# /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/core" /I "$(VXLROOT)/vcl/config.win32" == include path
#   -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE == compiler defines
#  == override in output directory
# bsta_algo  == name of output library

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=bsta_algo - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "bsta_algo.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "bsta_algo.mak" CFG="bsta_algo - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "bsta_algo - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "bsta_algo - Win32 Release" (based on "Win32 (x86) Static Library")
# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "bsta_algo - Win32 Release"

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
# ADD CPP /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/core" /I "$(VXLROOT)/vcl/config.win32" -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE /D "bsta_algo_EXPORTS"
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

!ELSEIF  "$(CFG)" == "bsta_algo - Win32 Debug"

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
# ADD CPP /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/core" /I "$(VXLROOT)/vcl/config.win32" -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE /D "bsta_algo_EXPORTS"
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
# Name "bsta_algo - Win32 Release"
# Name "bsta_algo - Win32 Debug"
# Begin Group "Source Files"
# PROP Default_Filter "cxx"
# Begin Source File
SOURCE=.\bsta_fit_weibull.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bsta_adaptive_updater+bsta_mixture+bsta_num_obs+bsta_gauss_f1---.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bsta_adaptive_updater+bsta_mixture+bsta_num_obs+bsta_gauss_fd3---.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bsta_adaptive_updater+bsta_mixture+bsta_num_obs+bsta_gauss_ff3---.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bsta_adaptive_updater+bsta_mixture+bsta_num_obs+bsta_gauss_id3---.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bsta_adaptive_updater+bsta_mixture+bsta_num_obs+bsta_gauss_if3---.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bsta_adaptive_updater+bsta_mixture+bsta_num_obs+bsta_gauss_sd3---.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bsta_adaptive_updater+bsta_mixture+bsta_num_obs+bsta_gauss_sf3---.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bsta_adaptive_updater+bsta_mixture_fixed+bsta_num_obs+bsta_gauss_f1-.3--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bsta_adaptive_updater+bsta_mixture_fixed+bsta_num_obs+bsta_gauss_if3-.3--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bsta_adaptive_updater+bsta_mixture_fixed+bsta_num_obs+bsta_gaussian_indep+float.2--.3--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bsta_adaptive_updater+bsta_mixture_fixed+bsta_num_obs+bsta_gaussian_indep+float.4--.3--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bsta_parzen_updater+bsta_parzen_sphere+float.3--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bsta_parzen_updater+bsta_parzen_sphere+float.1--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bsta_mean_shift+float.1-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bsta_mean_shift+float.2-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bsta_mean_shift+float.3-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bsta_mean_shift+double.1-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bsta_sample_set+float.1-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bsta_sample_set+float.2-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bsta_sample_set+double.1-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bsta_sample_set+float.3-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bsta_sample_set+double.2-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bsta_mix_beta_updater+bsta_mixture_fixed+bsta_num_obs+bsta_beta+float--.3--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bsta_von_mises_updater+bsta_von_mises+float.3--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bsta_von_mises_updater+bsta_von_mises+float.2--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bsta_von_mises_updater+bsta_von_mises+double.3--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bsta_von_mises_updater+bsta_von_mises+double.2--.cxx
# End Source File
# End Group
# Begin Group "Header Files"
# PROP Default_Filter "h;txx"
# Begin Source File
SOURCE=.\bsta_adaptive_updater.h
# End Source File
# Begin Source File
SOURCE=.\bsta_adaptive_updater.txx
# End Source File
# Begin Source File
SOURCE=.\bsta_bayes_functor.h
# End Source File
# Begin Source File
SOURCE=.\bsta_gaussian_stats.h
# End Source File
# Begin Source File
SOURCE=.\bsta_gaussian_updater.h
# End Source File
# Begin Source File
SOURCE=.\bsta_mixture_functors.h
# End Source File
# Begin Source File
SOURCE=.\bsta_truth_updater.h
# End Source File
# Begin Source File
SOURCE=.\bsta_truth_updater.txx
# End Source File
# Begin Source File
SOURCE=.\bsta_fit_weibull.h
# End Source File
# Begin Source File
SOURCE=.\bsta_parzen_updater.txx
# End Source File
# Begin Source File
SOURCE=.\bsta_parzen_updater.h
# End Source File
# Begin Source File
SOURCE=.\bsta_mean_shift.h
# End Source File
# Begin Source File
SOURCE=.\bsta_mean_shift.txx
# End Source File
# Begin Source File
SOURCE=.\bsta_sample_set.h
# End Source File
# Begin Source File
SOURCE=.\bsta_sample_set.txx
# End Source File
# Begin Source File
SOURCE=.\bsta_beta_updater.h
# End Source File
# Begin Source File
SOURCE=.\bsta_beta_updater.txx
# End Source File
# Begin Source File
SOURCE=.\bsta_von_mises_updater.h
# End Source File
# Begin Source File
SOURCE=.\bsta_von_mises_updater.txx
# End Source File
# End Group
# End Target
# End Project
