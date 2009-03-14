# Microsoft Developer Studio Project File - Name="vpdl" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# variables to REPLACE
#
# /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/core" /I "$(VXLROOT)/vcl/config.win32" == include path
#   -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE == compiler defines
#  == override in output directory
# vpdl  == name of output library

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=vpdl - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE
!MESSAGE NMAKE /f "vpdl.mak".
!MESSAGE
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE
!MESSAGE NMAKE /f "vpdl.mak" CFG="vpdl - Win32 Debug"
!MESSAGE
!MESSAGE Possible choices for configuration are:
!MESSAGE
!MESSAGE "vpdl - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "vpdl - Win32 Release" (based on "Win32 (x86) Static Library")
# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "vpdl - Win32 Release"

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
# ADD CPP /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/core" /I "$(VXLROOT)/vcl/config.win32" -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE /D "vpdl_EXPORTS"
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

!ELSEIF  "$(CFG)" == "vpdl - Win32 Debug"

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
# ADD CPP /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/core" /I "$(VXLROOT)/vcl/config.win32" -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE /D "vpdl_EXPORTS"
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

# Name "vpdl - Win32 Release"
# Name "vpdl - Win32 Debug"

# Begin Group "Source Files"
# PROP Default_Filter "cxx"
# Begin Source File
SOURCE=.\Templates/vpdl_distribution+float.2-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vpdl_distribution+double.2-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vpdl_distribution+float.3-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vpdl_distribution+double.0-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vpdl_distribution+float.1-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vpdl_distribution+double.3-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vpdl_distribution+double.1-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vpdl_distribution+float.0-.cxx
# End Source File
# End Group
# Begin Group "Header Files"
# PROP Default_Filter "h;txx"
# Begin Source File
SOURCE=.\vpdl_distribution.txx
# End Source File
# Begin Source File
SOURCE=.\vpdl_gaussian_indep.h
# End Source File
# Begin Source File
SOURCE=.\vpdl_distribution.h
# End Source File
# Begin Source File
SOURCE=.\vpdl_gaussian_base.h
# End Source File
# Begin Source File
SOURCE=.\vpdl_gaussian.h
# End Source File
# Begin Source File
SOURCE=.\vpdl_gaussian_sphere.h
# End Source File
# Begin Source File
SOURCE=.\vpdl_mixture.h
# End Source File
# Begin Source File
SOURCE=.\vpdl_multi_cmp_dist.h
# End Source File
# Begin Source File
SOURCE=.\vpdl_kernel_base.h
# End Source File
# Begin Source File
SOURCE=.\vpdl_kernel_gaussian_sfbw.h
# End Source File
# Begin Source File
SOURCE=.\vpdl_mixture_of.h
# End Source File
# Begin Source File
SOURCE=.\vpdt/vpdt_dist_traits.h
# End Source File
# Begin Source File
SOURCE=.\vpdt/vpdt_field_default.h
# End Source File
# Begin Source File
SOURCE=.\vpdt/vpdt_field_traits.h
# End Source File
# Begin Source File
SOURCE=.\vpdt/vpdt_access.h
# End Source File
# Begin Source File
SOURCE=.\vpdt/vpdt_eigen_sym_matrix.h
# End Source File
# Begin Source File
SOURCE=.\vpdt/vpdt_gaussian.h
# End Source File
# Begin Source File
SOURCE=.\vpdt/vpdt_norm_metric.h
# End Source File
# Begin Source File
SOURCE=.\vpdt/vpdt_update_gaussian.h
# End Source File
# Begin Source File
SOURCE=.\vpdt/vpdt_mixture_of.h
# End Source File
# Begin Source File
SOURCE=.\vpdt/vpdt_log_probability.h
# End Source File
# Begin Source File
SOURCE=.\vpdt/vpdt_probability.h
# End Source File
# Begin Source File
SOURCE=.\vpdt/vpdt_mog_fitness.h
# End Source File
# Begin Source File
SOURCE=.\vpdt/vpdt_update_mog.h
# End Source File
# Begin Source File
SOURCE=.\vpdt/vpdt_gaussian_detector.h
# End Source File
# Begin Source File
SOURCE=.\vpdt/vpdt_mixture_detector.h
# End Source File
# End Group
# End Target
# End Project
