# Microsoft Developer Studio Project File - Name="rrel" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# ITK DSP Header file
# This file is read by the build system of itk, and is used as the top part of
# a microsoft project dsp header file
# IF this is in a dsp file, then it is not the header, but has
# already been used, so do not edit here...

# variables to REPLACE
# 
# /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/vxl" /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/vcl/config.win32-VC60" /I "$(VXLROOT)/vxl" /I "$(VXLROOT)/rpl"  == include path
#   -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE == compiler defines
#  == override in output directory
# rrel  == name of output library

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=rrel - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "rrel.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "rrel.mak" CFG="rrel - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "rrel - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "rrel - Win32 Release" (based on "Win32 (x86) Static Library")
# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "rrel - Win32 Release"

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
# ADD BASE CPP /nologo /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB"  /FD /c
# ADD CPP /nologo /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB"  /FD /c
# ADD CPP /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/vxl" /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/vcl/config.win32-VC60" /I "$(VXLROOT)/vxl" /I "$(VXLROOT)/rpl"    -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE /D "rrel_EXPORTS"
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

!ELSEIF  "$(CFG)" == "rrel - Win32 Debug"

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
# ADD BASE CPP /nologo /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB"  /FD /c
# ADD CPP /nologo /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "_LIB"  /FD /GZ /c
# ADD CPP /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/vxl" /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/vcl/config.win32-VC60" /I "$(VXLROOT)/vxl" /I "$(VXLROOT)/rpl"    -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE /D "rrel_EXPORTS"
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

# Name "rrel - Win32 Release"
# Name "rrel - Win32 Debug"

# Begin Group "Source Files"
# PROP Default_Filter ""
# Begin Source File

SOURCE=.\rrel_cauchy_obj.cxx

# End Source File
# Begin Source File

SOURCE=.\rrel_estimation_problem.cxx

# End Source File
# Begin Source File

SOURCE=.\rrel_homography2d_est.cxx

# End Source File
# Begin Source File

SOURCE=.\rrel_irls.cxx

# End Source File
# Begin Source File

SOURCE=.\rrel_linear_regression.cxx

# End Source File
# Begin Source File

SOURCE=.\rrel_lms_obj.cxx

# End Source File
# Begin Source File

SOURCE=.\rrel_lts_obj.cxx

# End Source File
# Begin Source File

SOURCE=.\rrel_m_est_obj.cxx

# End Source File
# Begin Source File

SOURCE=.\rrel_misc.cxx

# End Source File
# Begin Source File

SOURCE=.\rrel_mlesac_obj.cxx

# End Source File
# Begin Source File

SOURCE=.\rrel_muse_table.cxx

# End Source File
# Begin Source File

SOURCE=.\rrel_muset_obj.cxx

# End Source File
# Begin Source File

SOURCE=.\rrel_orthogonal_regression.cxx

# End Source File
# Begin Source File

SOURCE=.\rrel_ran_sam_search.cxx

# End Source File
# Begin Source File

SOURCE=.\rrel_ransac_obj.cxx

# End Source File
# Begin Source File

SOURCE=.\rrel_trunc_quad_obj.cxx

# End Source File
# Begin Source File

SOURCE=.\rrel_tukey_obj.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/rrel_util_double~.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/rrel_util_list+double-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/rrel_util_vector+double-.cxx

# End Source File
# Begin Source File

SOURCE=.\rrel_kernel_density_obj.cxx

# End Source File
# End Group
# Begin Group "Header Files"
# PROP Default_Filter ""
# Begin Source File

SOURCE=.\rrel_cauchy_obj.h

# End Source File
# Begin Source File

SOURCE=.\rrel_estimation_problem.h

# End Source File
# Begin Source File

SOURCE=.\rrel_homography2d_est.h

# End Source File
# Begin Source File

SOURCE=.\rrel_irls.h

# End Source File
# Begin Source File

SOURCE=.\rrel_linear_regression.h

# End Source File
# Begin Source File

SOURCE=.\rrel_lms_obj.h

# End Source File
# Begin Source File

SOURCE=.\rrel_lts_obj.h

# End Source File
# Begin Source File

SOURCE=.\rrel_m_est_obj.h

# End Source File
# Begin Source File

SOURCE=.\rrel_misc.h

# End Source File
# Begin Source File

SOURCE=.\rrel_mlesac_obj.h

# End Source File
# Begin Source File

SOURCE=.\rrel_muse_table.h

# End Source File
# Begin Source File

SOURCE=.\rrel_muset_obj.h

# End Source File
# Begin Source File

SOURCE=.\rrel_orthogonal_regression.h

# End Source File
# Begin Source File

SOURCE=.\rrel_ran_sam_search.h

# End Source File
# Begin Source File

SOURCE=.\rrel_ransac_obj.h

# End Source File
# Begin Source File

SOURCE=.\rrel_trunc_quad_obj.h

# End Source File
# Begin Source File

SOURCE=.\rrel_tukey_obj.h

# End Source File
# Begin Source File

SOURCE=.\rrel_objective.h

# End Source File
# Begin Source File

SOURCE=.\rrel_util.h

# End Source File
# Begin Source File

SOURCE=.\rrel_util.txx

# End Source File
# Begin Source File

SOURCE=.\rrel_wls_obj.h

# End Source File
# Begin Source File

SOURCE=.\rrel_kernel_density_obj.h

# End Source File
# End Group
# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
