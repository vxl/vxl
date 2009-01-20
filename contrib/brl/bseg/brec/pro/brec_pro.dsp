# Microsoft Developer Studio Project File - Name="brec_pro" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# variables to REPLACE
# 
# /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/core" /I "$(VXLROOT)/vcl/config.win32" /I "$(VXLROOT)/contrib/mul" == include path
#   -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE == compiler defines
#  == override in output directory
# brec_pro  == name of output library

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=brec_pro - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "brec_pro.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "brec_pro.mak" CFG="vil2 - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "brec_pro - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "brec_pro - Win32 Release" (based on "Win32 (x86) Static Library")
# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "brec_pro - Win32 Release"

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
# ADD CPP /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/core" /I "$(VXLROOT)/vcl/config.win32" /I "$(VXLROOT)/contrib/mul" -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE /D "brec_pro_EXPORTS"
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

!ELSEIF  "$(CFG)" == "brec_pro - Win32 Debug"

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
# ADD CPP /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/core" /I "$(VXLROOT)/vcl/config.win32" /I "$(VXLROOT)/contrib/mul" -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE /D "brec_pro_EXPORTS"
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

# Name "brec_pro - Win32 Release"
# Name "brec_pro - Win32 Debug"

# Begin Group "Source Files"
# PROP Default_Filter "cxx"
# Begin Source File
SOURCE=.\brec_prob_map_threshold_process.cxx
# End Source File
# Begin Source File
SOURCE=.\brec_change_area_process.cxx
# End Source File
# Begin Source File
SOURCE=.\brec_prob_map_roc_compute_process.cxx
# End Source File
# Begin Source File
SOURCE=.\brec_prob_map_supress_process.cxx
# End Source File
# Begin Source File
SOURCE=.\brec_glitch_overlay_process.cxx
# End Source File
# Begin Source File
SOURCE=.\brec_density_to_prob_map_process.cxx
# End Source File
# Begin Source File
SOURCE=.\brec_recognize_structure_process.cxx
# End Source File
# Begin Source File
SOURCE=.\brec_glitch_process.cxx
# End Source File
# Begin Source File
SOURCE=.\brec_bayesian_update_process.cxx
# End Source File
# Begin Source File
SOURCE=.\brec_prob_map_area_process.cxx
# End Source File
# Begin Source File
SOURCE=.\brec_update_changes_process.cxx
# End Source File
# Begin Source File
SOURCE=.\brec_save_mog_image_process.cxx
# End Source File
# Begin Source File
SOURCE=.\brec_create_mog_image_process.cxx
# End Source File
# Begin Source File
SOURCE=.\brec_register.cxx
# End Source File
# End Group
# Begin Group "Header Files"
# PROP Default_Filter "h;txx"
# Begin Source File
SOURCE=.\brec_glitch_process.h
# End Source File
# Begin Source File
SOURCE=.\brec_bayesian_update_process.h
# End Source File
# Begin Source File
SOURCE=.\brec_change_area_process.h
# End Source File
# Begin Source File
SOURCE=.\brec_prob_map_supress_process.h
# End Source File
# Begin Source File
SOURCE=.\brec_prob_map_threshold_process.h
# End Source File
# Begin Source File
SOURCE=.\brec_update_changes_process.h
# End Source File
# Begin Source File
SOURCE=.\brec_density_to_prob_map_process.h
# End Source File
# Begin Source File
SOURCE=.\brec_recognize_structure_process.h
# End Source File
# Begin Source File
SOURCE=.\brec_prob_map_roc_compute_process.h
# End Source File
# Begin Source File
SOURCE=.\brec_save_mog_image_process.h
# End Source File
# Begin Source File
SOURCE=.\brec_glitch_overlay_process.h
# End Source File
# Begin Source File
SOURCE=.\brec_prob_map_area_process.h
# End Source File
# Begin Source File
SOURCE=.\brec_create_mog_image_process.h
# End Source File
# Begin Source File
SOURCE=.\brec_register.h
# End Source File
# End Group
# End Target
# End Project
