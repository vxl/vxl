# Microsoft Developer Studio Project File - Name="rsdl" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# ITK DSP Header file
# This file is read by the build system of itk, and is used as the top part of
# a microsoft project dsp header file
# IF this is in a dsp file, then it is not the header, but has
# already been used, so do not edit here...

# variables to REPLACE
# 
# /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/core" /I "$(VXLROOT)/vcl/config.win32" /I "$(VXLROOT)/contrib/rpl" == include path
#   -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE == compiler defines
#  == override in output directory
# rsdl  == name of output library

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=rsdl - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "rsdl.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "rsdl.mak" CFG="rsdl - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "rsdl - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "rsdl - Win32 Release" (based on "Win32 (x86) Static Library")
# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "rsdl - Win32 Release"

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
# ADD CPP /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/core" /I "$(VXLROOT)/vcl/config.win32" /I "$(VXLROOT)/contrib/rpl" -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE /D "rsdl_EXPORTS"
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

!ELSEIF  "$(CFG)" == "rsdl - Win32 Debug"

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
# ADD CPP /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/core" /I "$(VXLROOT)/vcl/config.win32" /I "$(VXLROOT)/contrib/rpl" -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE /D "rsdl_EXPORTS"
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

# Name "rsdl - Win32 Release"
# Name "rsdl - Win32 Debug"

# Begin Group "Source Files"
# PROP Default_Filter ""
# Begin Source File
SOURCE=.\rsdl_bounding_box.cxx
# End Source File
# Begin Source File
SOURCE=.\rsdl_dist.cxx
# End Source File
# Begin Source File
SOURCE=.\rsdl_kd_tree.cxx
# End Source File
# Begin Source File
SOURCE=.\rsdl_point.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/rsdl_bins_2d+double.int-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/rsdl_borgefors+vnl_vector_fixed+double.2--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vbl_smart_ptr+rsdl_kd_tree-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/rsdl_bins+2.double.int-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/rsdl_bins+3.double.int-.cxx
# End Source File
# End Group
# Begin Group "Header Files"
# PROP Default_Filter ""
# Begin Source File
SOURCE=.\rsdl_bins_2d.h
# End Source File
# Begin Source File
SOURCE=.\rsdl_bins_2d.txx
# End Source File
# Begin Source File
SOURCE=.\rsdl_borgefors.h
# End Source File
# Begin Source File
SOURCE=.\rsdl_borgefors.txx
# End Source File
# Begin Source File
SOURCE=.\rsdl_bounding_box.h
# End Source File
# Begin Source File
SOURCE=.\rsdl_dist.h
# End Source File
# Begin Source File
SOURCE=.\rsdl_kd_tree.h
# End Source File
# Begin Source File
SOURCE=.\rsdl_point.h
# End Source File
# Begin Source File
SOURCE=.\rsdl_fwd.h
# End Source File
# Begin Source File
SOURCE=.\rsdl_bins.h
# End Source File
# Begin Source File
SOURCE=.\rsdl_bins.txx
# End Source File
# End Group
# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
