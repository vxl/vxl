# Microsoft Developer Studio Project File - Name="vepl" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# ITK DSP Header file
# This file is read by the build system of itk, and is used as the top part of
# a microsoft project dsp header file
# IF this is in a dsp file, then it is not the header, but has
# already been used, so do not edit here...

# variables to REPLACE
# 
# /I "c:/awf/src/vxl/vcl" /I "c:/awf/src/vxl/vxl" /I "c:/awf/src/vxl/vcl" /I "c:/awf/src/vxl/vcl/config.win32-VC60" /I "c:/awf/src/vxl/vxl" /I "c:/awf/src/vxl/tbl"  == include path
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
!MESSAGE "vepl - Win32 MinSizeRel" (based on "Win32 (x86) Static Library")
!MESSAGE "vepl - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "vepl - Win32 RelWithDebInfo" (based on "Win32 (x86) Static Library")
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
# ADD BASE CPP /nologo /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB"  /FD /c
# ADD CPP /nologo /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB"  /FD /c
# ADD CPP /I "c:/awf/src/vxl/vcl" /I "c:/awf/src/vxl/vxl" /I "c:/awf/src/vxl/vcl" /I "c:/awf/src/vxl/vcl/config.win32-VC60" /I "c:/awf/src/vxl/vxl" /I "c:/awf/src/vxl/tbl"    -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE /D "vepl_EXPORTS"
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
# ADD BASE CPP /nologo /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB"  /FD /c
# ADD CPP /nologo /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "_LIB"  /FD /GZ /c
# ADD CPP /I "c:/awf/src/vxl/vcl" /I "c:/awf/src/vxl/vxl" /I "c:/awf/src/vxl/vcl" /I "c:/awf/src/vxl/vcl/config.win32-VC60" /I "c:/awf/src/vxl/vxl" /I "c:/awf/src/vxl/tbl"    -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE /D "vepl_EXPORTS"
# ADD CPP /W3 /Zm1000 /GX /GR
# ADD CPP /MDd /Zi /Od /GZ
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
!ELSEIF  "$(CFG)" == "vepl - Win32 MinSizeRel"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "MinSizeRel"
# PROP BASE Intermediate_Dir "MinSizeRel"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "MinSizeRel"
# PROP Intermediate_Dir "MinSizeRel"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "_ATL_DLL" /FD /c
# ADD CPP /nologo /D "NDEBUG" /D "_MBCS" /D "_ATL_DLL"  /D "WIN32" /D "_WINDOWS" /D "_USRDLL" /FD /c
# ADD CPP /I "c:/awf/src/vxl/vcl" /I "c:/awf/src/vxl/vxl" /I "c:/awf/src/vxl/vcl" /I "c:/awf/src/vxl/vcl/config.win32-VC60" /I "c:/awf/src/vxl/vxl" /I "c:/awf/src/vxl/tbl"    -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE /D "vepl_EXPORTS"
# ADD CPP /W3 /Zm1000 /GX /GR
# ADD CPP /MD /O1
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "vepl - Win32 RelWithDebInfo"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "RelWithDebInfo"
# PROP BASE Intermediate_Dir "RelWithDebInfo"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "RelWithDebInfo"
# PROP Intermediate_Dir "RelWithDebInfo"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB"  /FD /c
# ADD CPP /nologo /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB"  /FD /c
# ADD CPP /I "c:/awf/src/vxl/vcl" /I "c:/awf/src/vxl/vxl" /I "c:/awf/src/vxl/vcl" /I "c:/awf/src/vxl/vcl/config.win32-VC60" /I "c:/awf/src/vxl/vxl" /I "c:/awf/src/vxl/tbl"    -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE /D "vepl_EXPORTS"
# ADD CPP /W3 /Zm1000 /GX /GR
# ADD CPP /MD /Zi /O2
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "vepl - Win32 Release"
# Name "vepl - Win32 Debug"
# Name "vepl - Win32 MinSizeRel"
# Name "vepl - Win32 RelWithDebInfo"


# Begin Source File

SOURCE="c:/awf/src/vxl/tbl/vepl/CMakeLists.txt"

!IF  "$(CFG)" == "vepl - Win32 Release"
USERDEP__HACK=\
	"c:/awf/src/vxl/CMakeLists.txt"\
	"c:/awf/src/vxl/tbl/CMakeLists.txt"\
	"c:/awf/src/vxl/tbl/vepl/CMakeLists.txt"
# Begin Custom Build

"vepl.dsp" :  "$(SOURCE)" "$(INTDIR)" "$(OUTDIR)"
	"c:/awf/src/CMake/Source/cmake.exe" "c:/awf/src/vxl/tbl/vepl/CMakeLists.txt" -DSP -H"c:/awf/src/vxl" -S"c:/awf/src/vxl/tbl/vepl" -O"c:/awf/src/vxl/tbl/vepl" -B"c:/awf/src/vxl"

# End Custom Build

!ELSEIF  "$(CFG)" == "vepl - Win32 Debug"
USERDEP__HACK=\
	"c:/awf/src/vxl/CMakeLists.txt"\
	"c:/awf/src/vxl/tbl/CMakeLists.txt"\
	"c:/awf/src/vxl/tbl/vepl/CMakeLists.txt"
# Begin Custom Build

"vepl.dsp" :  "$(SOURCE)" "$(INTDIR)" "$(OUTDIR)"
	"c:/awf/src/CMake/Source/cmake.exe" "c:/awf/src/vxl/tbl/vepl/CMakeLists.txt" -DSP -H"c:/awf/src/vxl" -S"c:/awf/src/vxl/tbl/vepl" -O"c:/awf/src/vxl/tbl/vepl" -B"c:/awf/src/vxl"

# End Custom Build

!ELSEIF  "$(CFG)" == "vepl - Win32 MinSizeRel"
USERDEP__HACK=\
	"c:/awf/src/vxl/CMakeLists.txt"\
	"c:/awf/src/vxl/tbl/CMakeLists.txt"\
	"c:/awf/src/vxl/tbl/vepl/CMakeLists.txt"
# Begin Custom Build

"vepl.dsp" :  "$(SOURCE)" "$(INTDIR)" "$(OUTDIR)"
	"c:/awf/src/CMake/Source/cmake.exe" "c:/awf/src/vxl/tbl/vepl/CMakeLists.txt" -DSP -H"c:/awf/src/vxl" -S"c:/awf/src/vxl/tbl/vepl" -O"c:/awf/src/vxl/tbl/vepl" -B"c:/awf/src/vxl"

# End Custom Build

!ELSEIF  "$(CFG)" == "vepl - Win32 RelWithDebInfo"
USERDEP__HACK=\
	"c:/awf/src/vxl/CMakeLists.txt"\
	"c:/awf/src/vxl/tbl/CMakeLists.txt"\
	"c:/awf/src/vxl/tbl/vepl/CMakeLists.txt"
# Begin Custom Build

"vepl.dsp" :  "$(SOURCE)" "$(INTDIR)" "$(OUTDIR)"
	"c:/awf/src/CMake/Source/cmake.exe" "c:/awf/src/vxl/tbl/vepl/CMakeLists.txt" -DSP -H"c:/awf/src/vxl" -S"c:/awf/src/vxl/tbl/vepl" -O"c:/awf/src/vxl/tbl/vepl" -B"c:/awf/src/vxl"

# End Custom Build

!ENDIF

# End Source File
# Begin Group "Source Files"
# PROP Default_Filter ""
# Begin Source File

SOURCE=c:/awf/src/vxl/tbl/vepl/Templates/vepl_convert+double-.cxx

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/tbl/vepl/Templates/vepl_convert+float-.cxx

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/tbl/vepl/Templates/vepl_convert+int-.cxx

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/tbl/vepl/Templates/vepl_convert+short-.cxx

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/tbl/vepl/Templates/vepl_convert+uchar-.cxx

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/tbl/vepl/Templates/vepl_convert+vil_rgb+uchar--.cxx

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/tbl/vepl/Templates/vipl_add_random_noise+vil_image.vil_image.double.double-.cxx

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/tbl/vepl/Templates/vipl_add_random_noise+vil_image.vil_image.float.float-.cxx

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/tbl/vepl/Templates/vipl_add_random_noise+vil_image.vil_image.vil_rgb+uchar-.vil_rgb+uchar--.cxx

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/tbl/vepl/Templates/vipl_filter+vil_image.vil_image.double.double-.cxx

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/tbl/vepl/Templates/vipl_filter+vil_image.vil_image.double.float-.cxx

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/tbl/vepl/Templates/vipl_filter+vil_image.vil_image.double.int-.cxx

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/tbl/vepl/Templates/vipl_filter+vil_image.vil_image.double.short-.cxx

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/tbl/vepl/Templates/vipl_filter+vil_image.vil_image.double.vil_rgb+uchar--.cxx

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/tbl/vepl/Templates/vipl_filter+vil_image.vil_image.float.double-.cxx

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/tbl/vepl/Templates/vipl_filter+vil_image.vil_image.float.float-.cxx

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/tbl/vepl/Templates/vipl_filter+vil_image.vil_image.float.int-.cxx

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/tbl/vepl/Templates/vipl_filter+vil_image.vil_image.float.short-.cxx

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/tbl/vepl/Templates/vipl_filter+vil_image.vil_image.float.vil_rgb+uchar--.cxx

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/tbl/vepl/Templates/vipl_filter+vil_image.vil_image.short.short-.cxx

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/tbl/vepl/Templates/vipl_filter+vil_image.vil_image.uchar.double-.cxx

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/tbl/vepl/Templates/vipl_filter+vil_image.vil_image.uchar.int-.cxx

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/tbl/vepl/Templates/vipl_filter+vil_image.vil_image.uchar.short-.cxx

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/tbl/vepl/Templates/vipl_filter+vil_image.vil_image.uchar.uchar-.cxx

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/tbl/vepl/Templates/vipl_filter+vil_image.vil_image.uchar.vil_rgb+uchar--.cxx

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/tbl/vepl/Templates/vipl_filter+vil_image.vil_image.vil_rgb+uchar-.double-.cxx

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/tbl/vepl/Templates/vipl_filter+vil_image.vil_image.vil_rgb+uchar-.float-.cxx

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/tbl/vepl/Templates/vipl_filter+vil_image.vil_image.vil_rgb+uchar-.int-.cxx

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/tbl/vepl/Templates/vipl_filter+vil_image.vil_image.vil_rgb+uchar-.short-.cxx

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/tbl/vepl/Templates/vipl_filter+vil_image.vil_image.vil_rgb+uchar-.uchar-.cxx

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/tbl/vepl/Templates/vipl_filter+vil_image.vil_image.vil_rgb+uchar-.vil_rgb+uchar--.cxx

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/tbl/vepl/vepl_add_random_noise.cxx

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/tbl/vepl/vepl_dilate_disk.cxx

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/tbl/vepl/vepl_dyadic.cxx

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/tbl/vepl/vepl_erode_disk.cxx

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/tbl/vepl/vepl_gaussian_convolution.cxx

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/tbl/vepl/vepl_gradient_dir.cxx

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/tbl/vepl/vepl_gradient_mag.cxx

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/tbl/vepl/vepl_histogram.cxx

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/tbl/vepl/vepl_median.cxx

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/tbl/vepl/vepl_moment.cxx

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/tbl/vepl/vepl_monadic.cxx

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/tbl/vepl/vepl_sobel.cxx

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/tbl/vepl/vepl_threshold.cxx

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/tbl/vepl/vepl_x_gradient.cxx

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/tbl/vepl/vepl_y_gradient.cxx

# End Source File
# End Group
# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project

