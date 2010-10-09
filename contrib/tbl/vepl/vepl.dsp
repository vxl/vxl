# Microsoft Developer Studio Project File - Name="vepl" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# variables to REPLACE
# 
# /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/core" /I "$(VXLROOT)/vcl/config.win32" /I "$(VXLROOT)/contrib/tbl" == include path
#   -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE == compiler defines
#  == override in output directory
# vepl  == name of output library

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=vepl - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "vepl.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "vepl.mak" CFG="vepl - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "vepl - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "vepl - Win32 Release" (based on "Win32 (x86) Static Library")
# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "vepl - Win32 Release"

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
# ADD CPP /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/core" /I "$(VXLROOT)/vcl/config.win32" /I "$(VXLROOT)/contrib/tbl" -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE /D "vepl_EXPORTS"
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

!ELSEIF  "$(CFG)" == "vepl - Win32 Debug"

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
# ADD CPP /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/core" /I "$(VXLROOT)/vcl/config.win32" /I "$(VXLROOT)/contrib/tbl" -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE /D "vepl_EXPORTS"
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

# Name "vepl - Win32 Release"
# Name "vepl - Win32 Debug"

# Begin Group "Source Files"
# PROP Default_Filter "cxx"
# Begin Source File
SOURCE=.\Templates/vepl2_convert+double-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vepl2_convert+float-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vepl2_convert+int-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vepl2_convert+short-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vepl2_convert+uchar-.cxx
# End Source File
# Begin Source File
SOURCE=.\vepl2_add_random_noise.cxx
# End Source File
# Begin Source File
SOURCE=.\vepl2_dilate_disk.cxx
# End Source File
# Begin Source File
SOURCE=.\vepl2_dyadic.cxx
# End Source File
# Begin Source File
SOURCE=.\vepl2_erode_disk.cxx
# End Source File
# Begin Source File
SOURCE=.\vepl2_gaussian_convolution.cxx
# End Source File
# Begin Source File
SOURCE=.\vepl2_gradient_dir.cxx
# End Source File
# Begin Source File
SOURCE=.\vepl2_gradient_mag.cxx
# End Source File
# Begin Source File
SOURCE=.\vepl2_histogram.cxx
# End Source File
# Begin Source File
SOURCE=.\vepl2_median.cxx
# End Source File
# Begin Source File
SOURCE=.\vepl2_moment.cxx
# End Source File
# Begin Source File
SOURCE=.\vepl2_monadic.cxx
# End Source File
# Begin Source File
SOURCE=.\vepl2_sobel.cxx
# End Source File
# Begin Source File
SOURCE=.\vepl2_threshold.cxx
# End Source File
# Begin Source File
SOURCE=.\vepl2_x_gradient.cxx
# End Source File
# Begin Source File
SOURCE=.\vepl2_y_gradient.cxx
# End Source File
# End Group
# Begin Group "Header Files"
# PROP Default_Filter "h;txx"
# Begin Source File
SOURCE=.\vepl2_add_random_noise.h
# End Source File
# Begin Source File
SOURCE=.\vepl2_convert.h
# End Source File
# Begin Source File
SOURCE=.\vepl2_convert.txx
# End Source File
# Begin Source File
SOURCE=.\vepl2_dilate_disk.h
# End Source File
# Begin Source File
SOURCE=.\vepl2_dyadic.h
# End Source File
# Begin Source File
SOURCE=.\vepl2_erode_disk.h
# End Source File
# Begin Source File
SOURCE=.\vepl2_gaussian_convolution.h
# End Source File
# Begin Source File
SOURCE=.\vepl2_gradient_dir.h
# End Source File
# Begin Source File
SOURCE=.\vepl2_gradient_mag.h
# End Source File
# Begin Source File
SOURCE=.\vepl2_histogram.h
# End Source File
# Begin Source File
SOURCE=.\vepl2_median.h
# End Source File
# Begin Source File
SOURCE=.\vepl2_moment.h
# End Source File
# Begin Source File
SOURCE=.\vepl2_monadic.h
# End Source File
# Begin Source File
SOURCE=.\vepl2_sobel.h
# End Source File
# Begin Source File
SOURCE=.\vepl2_threshold.h
# End Source File
# Begin Source File
SOURCE=.\vepl2_x_gradient.h
# End Source File
# Begin Source File
SOURCE=.\vepl2_y_gradient.h
# End Source File
# Begin Source File
SOURCE=.\accessors/vipl_accessors_vil_image_view_base.h
# End Source File
# Begin Source File
SOURCE=.\accessors/vipl_accessors_vil_image_view_base.txx
# End Source File
# Begin Source File
SOURCE=.\section/vipl_filterable_section_container_generator_vil_image_view_base.txx
# End Source File
# End Group
# End Target
# End Project
