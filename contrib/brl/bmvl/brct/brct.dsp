# Microsoft Developer Studio Project File - Name="brct" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# ITK DSP Header file
# This file is read by the build system of itk, and is used as the top part of
# a microsoft project dsp header file
# IF this is in a dsp file, then it is not the header, but has
# already been used, so do not edit here...

# variables to REPLACE
# 
# /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/core" /I "$(VXLROOT)/vcl/config.win32" /I "$(VXLROOT)/contrib/mul" == include path
#   -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE == compiler defines
#  == override in output directory
# brct  == name of output library

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=brct - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "brct.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "brct.mak" CFG="vil2 - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "brct - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "brct - Win32 Release" (based on "Win32 (x86) Static Library")
# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "brct - Win32 Release"

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
# ADD CPP /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/core" /I "$(VXLROOT)/vcl/config.win32" /I "$(VXLROOT)/contrib/mul" -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE /D "brct_EXPORTS"
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

!ELSEIF  "$(CFG)" == "brct - Win32 Debug"

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
# ADD CPP /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/core" /I "$(VXLROOT)/vcl/config.win32" /I "$(VXLROOT)/contrib/mul" -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE /D "brct_EXPORTS"
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

# Name "brct - Win32 Release"
# Name "brct - Win32 Debug"

# Begin Group "Source Files"
# PROP Default_Filter "c;cxx;cpp"
# Begin Source File
SOURCE=.\brct_algos.cpp
# End Source File
# Begin Source File
SOURCE=.\brct_structure_estimator.cxx
# End Source File
# Begin Source File
SOURCE=.\brct_epi_reconstructor.cxx
# End Source File
# Begin Source File
SOURCE=.\brct_corr.cxx
# End Source File
# Begin Source File
SOURCE=.\brct_plane_calibrator.cxx
# End Source File
# Begin Source File
SOURCE=.\brct_plane_sweeper.cxx
# End Source File
# Begin Source File
SOURCE=.\brct_plane_sweeper_params.cxx
# End Source File
# Begin Source File
SOURCE=.\brct_volume_processor.cxx
# End Source File
# Begin Source File
SOURCE=.\brct_volume_processor_params.cxx
# End Source File
# Begin Source File
SOURCE=.\brct_dense_reconstructor.cxx
# End Source File
# Begin Source File
SOURCE=.\brct_plane_corr.cxx
# End Source File
# Begin Source File
SOURCE=.\kalman_filter.cpp
# End Source File
# Begin Source File
SOURCE=.\Templates/vbl_smart_ptr+brct_plane_corr-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vcl_vector+brct_plane_corr_sptr-.cxx
# End Source File
# End Group
# Begin Group "Header Files"
# PROP Default_Filter "h;txx"
# Begin Source File
SOURCE=.\brct_algos.h
# End Source File
# Begin Source File
SOURCE=.\brct_structure_estimator.h
# End Source File
# Begin Source File
SOURCE=.\brct_epi_reconstructor.h
# End Source File
# Begin Source File
SOURCE=.\kalman_filter.h
# End Source File
# Begin Source File
SOURCE=.\brct_corr.h
# End Source File
# Begin Source File
SOURCE=.\brct_plane_calibrator.h
# End Source File
# Begin Source File
SOURCE=.\brct_plane_sweeper.h
# End Source File
# Begin Source File
SOURCE=.\brct_plane_sweeper_params.h
# End Source File
# Begin Source File
SOURCE=.\brct_volume_processor.h
# End Source File
# Begin Source File
SOURCE=.\brct_volume_processor_params.h
# End Source File
# Begin Source File
SOURCE=.\brct_dense_reconstructor.h
# End Source File
# Begin Source File
SOURCE=.\brct_plane_corr.h
# End Source File
# Begin Source File
SOURCE=.\brct_plane_corr_sptr.h
# End Source File
# End Group
# End Target
# End Project
