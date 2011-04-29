# Microsoft Developer Studio Project File - Name="msm" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# variables to REPLACE
# 
# /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/core" /I "$(VXLROOT)/vcl/config.win32" == include path
#   -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE == compiler defines
#  == override in output directory
# msm  == name of output library

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=msm - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "msm.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "msm.mak" CFG="msm - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "msm - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "msm - Win32 Release" (based on "Win32 (x86) Static Library")
# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "msm - Win32 Release"

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
# ADD CPP /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/core" /I "$(VXLROOT)/vcl/config.win32" -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE /D "msm_EXPORTS"
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

!ELSEIF  "$(CFG)" == "msm - Win32 Debug"

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
# ADD CPP /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/core" /I "$(VXLROOT)/vcl/config.win32" -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE /D "msm_EXPORTS"
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
# Name "msm - Win32 Release"
# Name "msm - Win32 Debug"
# Begin Group "Source Files"
# PROP Default_Filter "cxx"
# Begin Source File
SOURCE=.\msm_points.cxx
# End Source File
# Begin Source File
SOURCE=.\msm_similarity_aligner.cxx
# End Source File
# Begin Source File
SOURCE=.\msm_add_all_loaders.cxx
# End Source File
# Begin Source File
SOURCE=.\msm_ref_shape_instance.cxx
# End Source File
# Begin Source File
SOURCE=.\msm_no_limiter.cxx
# End Source File
# Begin Source File
SOURCE=.\msm_wt_mat_2d.cxx
# End Source File
# Begin Source File
SOURCE=.\msm_translation_aligner.cxx
# End Source File
# Begin Source File
SOURCE=.\msm_shape_model_builder.cxx
# End Source File
# Begin Source File
SOURCE=.\msm_param_limiter.cxx
# End Source File
# Begin Source File
SOURCE=.\msm_shape_instance.cxx
# End Source File
# Begin Source File
SOURCE=.\msm_aligner.cxx
# End Source File
# Begin Source File
SOURCE=.\msm_curve.cxx
# End Source File
# Begin Source File
SOURCE=.\msm_shape_model.cxx
# End Source File
# Begin Source File
SOURCE=.\msm_box_limiter.cxx
# End Source File
# Begin Source File
SOURCE=.\msm_zoom_aligner.cxx
# End Source File
# Begin Source File
SOURCE=.\msm_ellipsoid_limiter.cxx
# End Source File
# Begin Source File
SOURCE=.\msm_ref_shape_model.cxx
# End Source File
# Begin Source File
SOURCE=.\msm_shape_perturber.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/mbl_cloneables_factory+msm_param_limiter-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vsl_binary_loader+msm_param_limiter-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vsl_binary_loader+msm_aligner-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/mbl_cloneables_factory+msm_aligner-.cxx
# End Source File
# End Group
# Begin Group "Header Files"
# PROP Default_Filter "h;txx"
# Begin Source File
SOURCE=.\msm_points.h
# End Source File
# Begin Source File
SOURCE=.\msm_aligner.h
# End Source File
# Begin Source File
SOURCE=.\msm_curve.h
# End Source File
# Begin Source File
SOURCE=.\msm_shape_instance.h
# End Source File
# Begin Source File
SOURCE=.\msm_box_limiter.h
# End Source File
# Begin Source File
SOURCE=.\msm_shape_model_builder.h
# End Source File
# Begin Source File
SOURCE=.\msm_no_limiter.h
# End Source File
# Begin Source File
SOURCE=.\msm_add_all_loaders.h
# End Source File
# Begin Source File
SOURCE=.\msm_similarity_aligner.h
# End Source File
# Begin Source File
SOURCE=.\msm_ref_shape_instance.h
# End Source File
# Begin Source File
SOURCE=.\msm_param_limiter.h
# End Source File
# Begin Source File
SOURCE=.\msm_ellipsoid_limiter.h
# End Source File
# Begin Source File
SOURCE=.\msm_wt_mat_2d.h
# End Source File
# Begin Source File
SOURCE=.\msm_translation_aligner.h
# End Source File
# Begin Source File
SOURCE=.\msm_shape_model.h
# End Source File
# Begin Source File
SOURCE=.\msm_zoom_aligner.h
# End Source File
# Begin Source File
SOURCE=.\msm_ref_shape_model.h
# End Source File
# Begin Source File
SOURCE=.\msm_shape_perturber.h
# End Source File
# End Group
# End Target
# End Project
