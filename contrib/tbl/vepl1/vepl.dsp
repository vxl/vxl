# Microsoft Developer Studio Project File - Name="vepl1" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# variables to REPLACE
# 
# /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/core" /I "$(VXLROOT)/vcl/config.win32" /I "$(VXLROOT)/contrib/tbl" == include path
#   -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE == compiler defines
#  == override in output directory
# vepl1  == name of output library

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=vepl1 - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "vepl1.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "vepl1.mak" CFG="vepl1 - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "vepl1 - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "vepl1 - Win32 Release" (based on "Win32 (x86) Static Library")
# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "vepl1 - Win32 Release"

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
# ADD CPP /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/core" /I "$(VXLROOT)/vcl/config.win32" /I "$(VXLROOT)/contrib/tbl" -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE /D "vepl1_EXPORTS"
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

!ELSEIF  "$(CFG)" == "vepl1 - Win32 Debug"

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
# ADD CPP /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/core" /I "$(VXLROOT)/vcl/config.win32" /I "$(VXLROOT)/contrib/tbl" -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE /D "vepl1_EXPORTS"
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

# Name "vepl1 - Win32 Release"
# Name "vepl1 - Win32 Debug"

# Begin Group "Source Files"
# PROP Default_Filter "cxx"
# Begin Source File
SOURCE=.\Templates/vepl1_convert+double-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vepl1_convert+float-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vepl1_convert+int-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vepl1_convert+short-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vepl1_convert+uchar-.cxx
# End Source File
# Begin Source File
SOURCE=.\vepl1_add_random_noise.cxx
# End Source File
# Begin Source File
SOURCE=.\vepl1_dilate_disk.cxx
# End Source File
# Begin Source File
SOURCE=.\vepl1_dyadic.cxx
# End Source File
# Begin Source File
SOURCE=.\vepl1_erode_disk.cxx
# End Source File
# Begin Source File
SOURCE=.\vepl1_gaussian_convolution.cxx
# End Source File
# Begin Source File
SOURCE=.\vepl1_gradient_dir.cxx
# End Source File
# Begin Source File
SOURCE=.\vepl1_gradient_mag.cxx
# End Source File
# Begin Source File
SOURCE=.\vepl1_histogram.cxx
# End Source File
# Begin Source File
SOURCE=.\vepl1_median.cxx
# End Source File
# Begin Source File
SOURCE=.\vepl1_moment.cxx
# End Source File
# Begin Source File
SOURCE=.\vepl1_monadic.cxx
# End Source File
# Begin Source File
SOURCE=.\vepl1_sobel.cxx
# End Source File
# Begin Source File
SOURCE=.\vepl1_threshold.cxx
# End Source File
# Begin Source File
SOURCE=.\vepl1_x_gradient.cxx
# End Source File
# Begin Source File
SOURCE=.\vepl1_y_gradient.cxx
# End Source File
# End Group
# Begin Group "Header Files"
# PROP Default_Filter "h;txx"
# Begin Source File
SOURCE=.\vepl1_add_random_noise.h
# End Source File
# Begin Source File
SOURCE=.\vepl1_convert.h
# End Source File
# Begin Source File
SOURCE=.\vepl1_convert.txx
# End Source File
# Begin Source File
SOURCE=.\vepl1_dilate_disk.h
# End Source File
# Begin Source File
SOURCE=.\vepl1_dyadic.h
# End Source File
# Begin Source File
SOURCE=.\vepl1_erode_disk.h
# End Source File
# Begin Source File
SOURCE=.\vepl1_gaussian_convolution.h
# End Source File
# Begin Source File
SOURCE=.\vepl1_gradient_dir.h
# End Source File
# Begin Source File
SOURCE=.\vepl1_gradient_mag.h
# End Source File
# Begin Source File
SOURCE=.\vepl1_histogram.h
# End Source File
# Begin Source File
SOURCE=.\vepl1_median.h
# End Source File
# Begin Source File
SOURCE=.\vepl1_moment.h
# End Source File
# Begin Source File
SOURCE=.\vepl1_monadic.h
# End Source File
# Begin Source File
SOURCE=.\vepl1_sobel.h
# End Source File
# Begin Source File
SOURCE=.\vepl1_threshold.h
# End Source File
# Begin Source File
SOURCE=.\vepl1_x_gradient.h
# End Source File
# Begin Source File
SOURCE=.\vepl1_y_gradient.h
# End Source File
# End Group
# End Target
# End Project
