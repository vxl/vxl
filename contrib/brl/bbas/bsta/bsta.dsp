# Microsoft Developer Studio Project File - Name="bsta" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# variables to REPLACE
# 
# /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/core" /I "$(VXLROOT)/vcl/config.win32" /I "$(VXLROOT)/contrib/mul" == include path
#   -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE == compiler defines
#  == override in output directory
# bsta  == name of output library

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=bsta - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "bsta.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "bsta.mak" CFG="bsta - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "bsta - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "bsta - Win32 Release" (based on "Win32 (x86) Static Library")
# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "bsta - Win32 Release"

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
# ADD CPP /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/core" /I "$(VXLROOT)/vcl/config.win32" /I "$(VXLROOT)/contrib/mul" -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE /D "bsta_EXPORTS"
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

!ELSEIF  "$(CFG)" == "bsta - Win32 Debug"

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
# ADD CPP /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/core" /I "$(VXLROOT)/vcl/config.win32" /I "$(VXLROOT)/contrib/mul" -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE /D "bsta_EXPORTS"
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

# Name "bsta - Win32 Release"
# Name "bsta - Win32 Debug"

# Begin Group "Source Files"
# PROP Default_Filter "cxx"
# Begin Source File
SOURCE=.\bsta_gauss.cxx
# End Source File
# Begin Source File
SOURCE=.\bsta_k_medoid.cxx
# End Source File
# Begin Source File
SOURCE=.\bsta_int_histogram_1d.cxx
# End Source File
# Begin Source File
SOURCE=.\bsta_int_histogram_2d.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bsta_histogram+double-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bsta_histogram+float-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bsta_joint_histogram+double-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bsta_joint_histogram+float-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bsta_otsu_threshold+double-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bsta_otsu_threshold+float-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bsta_gaussian_full+double.3-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bsta_gaussian_full+float.3-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bsta_gaussian_full+float.6-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bsta_gaussian_indep+double.3-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bsta_gaussian_indep+float.2-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bsta_gaussian_indep+float.3-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bsta_gaussian_sphere+double.1-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bsta_gaussian_sphere+double.3-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bsta_gaussian_sphere+float.1-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bsta_gaussian_sphere+float.2-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bsta_gaussian_sphere+float.3-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vbl_smart_ptr+bsta_histogram_base-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vbl_smart_ptr+bsta_joint_histogram_base-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bsta_gaussian_indep+float.4-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bsta_weibull+double-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bsta_weibull+float-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bsta_sampler+uint-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bsta_parzen_sphere+float.1-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bsta_parzen_sphere+float.3-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bsta_gaussian_full+float.2-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bsta_beta+double-.cxx
# End Source File
# End Group
# Begin Group "Header Files"
# PROP Default_Filter "h;txx"
# Begin Source File
SOURCE=.\bsta_gauss.h
# End Source File
# Begin Source File
SOURCE=.\bsta_histogram.h
# End Source File
# Begin Source File
SOURCE=.\bsta_histogram.txx
# End Source File
# Begin Source File
SOURCE=.\bsta_joint_histogram.h
# End Source File
# Begin Source File
SOURCE=.\bsta_joint_histogram.txx
# End Source File
# Begin Source File
SOURCE=.\bsta_k_medoid.h
# End Source File
# Begin Source File
SOURCE=.\bsta_otsu_threshold.h
# End Source File
# Begin Source File
SOURCE=.\bsta_otsu_threshold.txx
# End Source File
# Begin Source File
SOURCE=.\bsta_int_histogram_1d.h
# End Source File
# Begin Source File
SOURCE=.\bsta_int_histogram_2d.h
# End Source File
# Begin Source File
SOURCE=.\bsta_attributes.h
# End Source File
# Begin Source File
SOURCE=.\bsta_basic_functors.h
# End Source File
# Begin Source File
SOURCE=.\bsta_detector_gaussian.h
# End Source File
# Begin Source File
SOURCE=.\bsta_detector_mixture.h
# End Source File
# Begin Source File
SOURCE=.\bsta_distribution.h
# End Source File
# Begin Source File
SOURCE=.\bsta_gauss_f1.h
# End Source File
# Begin Source File
SOURCE=.\bsta_gauss_fd3.h
# End Source File
# Begin Source File
SOURCE=.\bsta_gauss_ff3.h
# End Source File
# Begin Source File
SOURCE=.\bsta_gauss_id3.h
# End Source File
# Begin Source File
SOURCE=.\bsta_gauss_if3.h
# End Source File
# Begin Source File
SOURCE=.\bsta_gauss_sd3.h
# End Source File
# Begin Source File
SOURCE=.\bsta_gauss_sf3.h
# End Source File
# Begin Source File
SOURCE=.\bsta_gaussian.h
# End Source File
# Begin Source File
SOURCE=.\bsta_gaussian_full.h
# End Source File
# Begin Source File
SOURCE=.\bsta_gaussian_full.txx
# End Source File
# Begin Source File
SOURCE=.\bsta_gaussian_indep.h
# End Source File
# Begin Source File
SOURCE=.\bsta_gaussian_indep.txx
# End Source File
# Begin Source File
SOURCE=.\bsta_gaussian_sphere.h
# End Source File
# Begin Source File
SOURCE=.\bsta_gaussian_sphere.txx
# End Source File
# Begin Source File
SOURCE=.\bsta_mixture.h
# End Source File
# Begin Source File
SOURCE=.\bsta_mixture_fixed.h
# End Source File
# Begin Source File
SOURCE=.\bsta_histogram_base.h
# End Source File
# Begin Source File
SOURCE=.\bsta_histogram_sptr.h
# End Source File
# Begin Source File
SOURCE=.\bsta_joint_histogram_base.h
# End Source File
# Begin Source File
SOURCE=.\bsta_joint_histogram_sptr.h
# End Source File
# Begin Source File
SOURCE=.\bsta_gauss_f2.h
# End Source File
# Begin Source File
SOURCE=.\bsta_weibull.h
# End Source File
# Begin Source File
SOURCE=.\bsta_weibull.txx
# End Source File
# Begin Source File
SOURCE=.\bsta_sampler.h
# End Source File
# Begin Source File
SOURCE=.\bsta_sampler.txx
# End Source File
# Begin Source File
SOURCE=.\bsta_parzen_sphere.txx
# End Source File
# Begin Source File
SOURCE=.\bsta_parzen_sphere.h
# End Source File
# Begin Source File
SOURCE=.\bsta_parzen.h
# End Source File
# Begin Source File
SOURCE=.\bsta_parzen_sf1.h
# End Source File
# Begin Source File
SOURCE=.\bsta_parzen_sf3.h
# End Source File
# Begin Source File
SOURCE=.\bsta_beta.h
# End Source File
# Begin Source File
SOURCE=.\bsta_beta.txx
# End Source File
# End Group
# End Target
# End Project
