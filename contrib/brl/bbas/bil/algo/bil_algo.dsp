# Microsoft Developer Studio Project File - Name="bil_algo" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# variables to REPLACE
# 
# /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/core" /I "$(VXLROOT)/vcl/config.win32" /I "$(VXLROOT)/contrib/mul" == include path
#   -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE == compiler defines
#  == override in output directory
# bil_algo  == name of output library

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=bil_algo - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "bil_algo.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "bil_algo.mak" CFG="vil2 - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "bil_algo - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "bil_algo - Win32 Release" (based on "Win32 (x86) Static Library")
# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "bil_algo - Win32 Release"

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
# ADD CPP /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/core" /I "$(VXLROOT)/vcl/config.win32" /I "$(VXLROOT)/contrib/mul" -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE /D "bil_algo_EXPORTS"
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

!ELSEIF  "$(CFG)" == "bil_algo - Win32 Debug"

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
# ADD CPP /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/core" /I "$(VXLROOT)/vcl/config.win32" /I "$(VXLROOT)/contrib/mul" -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE /D "bil_algo_EXPORTS"
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

# Name "bil_algo - Win32 Release"
# Name "bil_algo - Win32 Debug"

# Begin Group "Source Files"
# PROP Default_Filter "cxx"
# Begin Source File
SOURCE=.\bil_cedt.cxx
# End Source File
# Begin Source File
SOURCE=.\bil_color_conversions.cxx
# End Source File
# Begin Source File
SOURCE=.\bil_detect_ridges.cxx
# End Source File
# Begin Source File
SOURCE=.\bil_edt.cxx
# End Source File
# Begin Source File
SOURCE=.\bil_wshed2d.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bil_detect_ridges+float-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bil_detect_ridges+uint16-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bil_finite_differences+double-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bil_finite_differences+float-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bil_finite_second_differences+double-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bil_finite_second_differences+float-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bil_harr_wavelet_transform+float-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bil_perform_tensor_decomposition+float-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bil_scale_image+float-.cxx
# End Source File
# End Group
# Begin Group "Header Files"
# PROP Default_Filter "h;txx"
# Begin Source File
SOURCE=.\bil_cedt.h
# End Source File
# Begin Source File
SOURCE=.\bil_color_conversions.h
# End Source File
# Begin Source File
SOURCE=.\bil_detect_ridges.h
# End Source File
# Begin Source File
SOURCE=.\bil_detect_ridges.txx
# End Source File
# Begin Source File
SOURCE=.\bil_edge_indicator.h
# End Source File
# Begin Source File
SOURCE=.\bil_edt.h
# End Source File
# Begin Source File
SOURCE=.\bil_equalize.h
# End Source File
# Begin Source File
SOURCE=.\bil_finite_differences.h
# End Source File
# Begin Source File
SOURCE=.\bil_finite_differences.txx
# End Source File
# Begin Source File
SOURCE=.\bil_finite_second_differences.h
# End Source File
# Begin Source File
SOURCE=.\bil_finite_second_differences.txx
# End Source File
# Begin Source File
SOURCE=.\bil_harr_wavelet_transform.h
# End Source File
# Begin Source File
SOURCE=.\bil_harr_wavelet_transform.txx
# End Source File
# Begin Source File
SOURCE=.\bil_perform_tensor_decomposition.h
# End Source File
# Begin Source File
SOURCE=.\bil_perform_tensor_decomposition.txx
# End Source File
# Begin Source File
SOURCE=.\bil_roi_mask.h
# End Source File
# Begin Source File
SOURCE=.\bil_wshed2d.h
# End Source File
# Begin Source File
SOURCE=.\bil_scale_image.h
# End Source File
# Begin Source File
SOURCE=.\bil_scale_image.txx
# End Source File
# End Group
# End Target
# End Project
