# Microsoft Developer Studio Project File - Name="vipl" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# ITK DSP Header file
# This file is read by the build system of itk, and is used as the top part of
# a microsoft project dsp header file
# IF this is in a dsp file, then it is not the header, but has
# already been used, so do not edit here...

# variables to REPLACE
# 
# /I "$(IUEROOT)/vcl" /I "$(IUEROOT)/vxl" /I "$(IUEROOT)/vcl" /I "$(IUEROOT)/vcl/config.win32-VC60" /I "$(IUEROOT)/vxl" /I "$(IUEROOT)/tbl"  == include path
#   -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE == compiler defines
#  == override in output directory
# vipl  == name of output library

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=vipl - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "vipl.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "vipl.mak" CFG="vipl - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "vipl - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "vipl - Win32 MinSizeRel" (based on "Win32 (x86) Static Library")
!MESSAGE "vipl - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "vipl - Win32 RelWithDebInfo" (based on "Win32 (x86) Static Library")
# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "vipl - Win32 Release"

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
# ADD CPP /I "$(IUEROOT)/vcl" /I "$(IUEROOT)/vxl" /I "$(IUEROOT)/vcl" /I "$(IUEROOT)/vcl/config.win32-VC60" /I "$(IUEROOT)/vxl" /I "$(IUEROOT)/tbl"    -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE /D "vipl_EXPORTS"
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

!ELSEIF  "$(CFG)" == "vipl - Win32 Debug"

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
# ADD CPP /I "$(IUEROOT)/vcl" /I "$(IUEROOT)/vxl" /I "$(IUEROOT)/vcl" /I "$(IUEROOT)/vcl/config.win32-VC60" /I "$(IUEROOT)/vxl" /I "$(IUEROOT)/tbl"    -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE /D "vipl_EXPORTS"
# ADD CPP /W3 /Zm1000 /GX /GR
# ADD CPP /MDd /Zi /Od /GZ
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
!ELSEIF  "$(CFG)" == "vipl - Win32 MinSizeRel"

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
# ADD CPP /I "$(IUEROOT)/vcl" /I "$(IUEROOT)/vxl" /I "$(IUEROOT)/vcl" /I "$(IUEROOT)/vcl/config.win32-VC60" /I "$(IUEROOT)/vxl" /I "$(IUEROOT)/tbl"    -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE /D "vipl_EXPORTS"
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

!ELSEIF  "$(CFG)" == "vipl - Win32 RelWithDebInfo"

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
# ADD CPP /I "$(IUEROOT)/vcl" /I "$(IUEROOT)/vxl" /I "$(IUEROOT)/vcl" /I "$(IUEROOT)/vcl/config.win32-VC60" /I "$(IUEROOT)/vxl" /I "$(IUEROOT)/tbl"    -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE /D "vipl_EXPORTS"
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

# Name "vipl - Win32 Release"
# Name "vipl - Win32 Debug"
# Name "vipl - Win32 MinSizeRel"
# Name "vipl - Win32 RelWithDebInfo"


# Begin Source File

SOURCE=.\CMakeLists.txt

!IF  "$(CFG)" == "vipl - Win32 Release"
USERDEP__HACK=\
	"$(IUEROOT)/CMakeLists.txt"\
	"$(IUEROOT)/tbl/CMakeLists.txt"\
	"$(IUEROOT)/tbl/vipl/CMakeLists.txt"
# Begin Custom Build

"vipl.dsp" :  "$(SOURCE)" "$(INTDIR)" "$(OUTDIR)"
	"$(CMAKE_ROOT)/Source/cmake.exe" "$(IUEROOT)/tbl/vipl/CMakeLists.txt" -DSP -H"$(IUEROOT)" -S"$(IUEROOT)/tbl/vipl" -O"$(IUEROOT)/tbl/vipl" -B"$(IUEROOT)"

# End Custom Build

!ELSEIF  "$(CFG)" == "vipl - Win32 Debug"
USERDEP__HACK=\
	"$(IUEROOT)/CMakeLists.txt"\
	"$(IUEROOT)/tbl/CMakeLists.txt"\
	"$(IUEROOT)/tbl/vipl/CMakeLists.txt"
# Begin Custom Build

"vipl.dsp" :  "$(SOURCE)" "$(INTDIR)" "$(OUTDIR)"
	"$(CMAKE_ROOT)/Source/cmake.exe" "$(IUEROOT)/tbl/vipl/CMakeLists.txt" -DSP -H"$(IUEROOT)" -S"$(IUEROOT)/tbl/vipl" -O"$(IUEROOT)/tbl/vipl" -B"$(IUEROOT)"

# End Custom Build

!ELSEIF  "$(CFG)" == "vipl - Win32 MinSizeRel"
USERDEP__HACK=\
	"$(IUEROOT)/CMakeLists.txt"\
	"$(IUEROOT)/tbl/CMakeLists.txt"\
	"$(IUEROOT)/tbl/vipl/CMakeLists.txt"
# Begin Custom Build

"vipl.dsp" :  "$(SOURCE)" "$(INTDIR)" "$(OUTDIR)"
	"$(CMAKE_ROOT)/Source/cmake.exe" "$(IUEROOT)/tbl/vipl/CMakeLists.txt" -DSP -H"$(IUEROOT)" -S"$(IUEROOT)/tbl/vipl" -O"$(IUEROOT)/tbl/vipl" -B"$(IUEROOT)"

# End Custom Build

!ELSEIF  "$(CFG)" == "vipl - Win32 RelWithDebInfo"
USERDEP__HACK=\
	"$(IUEROOT)/CMakeLists.txt"\
	"$(IUEROOT)/tbl/CMakeLists.txt"\
	"$(IUEROOT)/tbl/vipl/CMakeLists.txt"
# Begin Custom Build

"vipl.dsp" :  "$(SOURCE)" "$(INTDIR)" "$(OUTDIR)"
	"$(CMAKE_ROOT)/Source/cmake.exe" "$(IUEROOT)/tbl/vipl/CMakeLists.txt" -DSP -H"$(IUEROOT)" -S"$(IUEROOT)/tbl/vipl" -O"$(IUEROOT)/tbl/vipl" -B"$(IUEROOT)"

# End Custom Build

!ENDIF

# End Source File
# Begin Group "Source Files"
# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Templates/vipl_accessors_vil_image+double-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vipl_accessors_vil_image+float-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vipl_accessors_vil_image+int-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vipl_accessors_vil_image+short-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vipl_accessors_vil_image+uchar-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vipl_accessors_vil_image+ushort-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vipl_add_random_noise+vil_image.vil_image.uchar.uchar-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vipl_convert+vil_image.vil_image.double.uchar-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vipl_convert+vil_image.vil_image.float.uchar-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vipl_convert+vil_image.vil_image.int.uchar-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vipl_convert+vil_image.vil_image.short.uchar-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vipl_convert+vil_image.vil_image.uchar.uchar-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vipl_convert+vil_image.vil_image.vil_rgb+uchar-.uchar-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vipl_dilate_disk+vil_image.vil_image.uchar.uchar-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vipl_dyadic+vil_image.vil_image.float.float-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vipl_erode_disk+vil_image.vil_image.float.float-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vipl_erode_disk+vil_image.vil_image.uchar.uchar-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vipl_erode_disk+vil_image.vil_image.ushort.ushort-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vipl_filter_2d+vil_image.vil_image.double.uchar-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vipl_filter_2d+vil_image.vil_image.float.float-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vipl_filter_2d+vil_image.vil_image.float.uchar-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vipl_filter_2d+vil_image.vil_image.int.uchar-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vipl_filter_2d+vil_image.vil_image.short.short-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vipl_filter_2d+vil_image.vil_image.short.uchar-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vipl_filter_2d+vil_image.vil_image.uchar.float-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vipl_filter_2d+vil_image.vil_image.uchar.uchar-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vipl_filter_2d+vil_image.vil_image.ushort.ushort-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vipl_filter_2d+vil_image.vil_image.vil_rgb+uchar-.uchar-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vipl_gradient_mag+vil_image.vil_image.uchar.uchar-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vipl_median+vil_image.vil_image.uchar.uchar-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vipl_moment+vil_image.vil_image.uchar.float-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vipl_monadic+vil_image.vil_image.float.float-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vipl_section_container+double-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vipl_section_container+float-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vipl_section_container+int-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vipl_section_container+short-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vipl_section_container+uchar-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vipl_section_container+ushort-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vipl_section_container+vil_rgb+uchar--.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vipl_section_descriptor+double-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vipl_section_descriptor+float-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vipl_section_descriptor+int-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vipl_section_descriptor+short-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vipl_section_descriptor+uchar-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vipl_section_descriptor+ushort-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vipl_section_descriptor+vil_rgb+uchar--.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vipl_section_iterator+double-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vipl_section_iterator+float-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vipl_section_iterator+int-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vipl_section_iterator+short-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vipl_section_iterator+uchar-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vipl_section_iterator+ushort-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vipl_section_iterator+vil_rgb+uchar--.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vipl_sobel+vil_image.vil_image.uchar.uchar-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vipl_threshold+vil_image.vil_image.float.float-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vipl_threshold+vil_image.vil_image.uchar.uchar-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vipl_threshold+vil_image.vil_image.ushort.ushort-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vipl_x_gradient+vil_image.vil_image.uchar.uchar-.cxx

# End Source File
# Begin Source File

SOURCE=.\filter/vipl_filter_abs.cxx

# End Source File
# Begin Source File

SOURCE=.\vipl_with_vbl_array_2d/Templates/vipl_accessors_vbl_array_2d+uchar-.cxx

# End Source File
# Begin Source File

SOURCE=.\vipl_with_vbl_array_2d/Templates/vipl_filter_2d+vbl_array_2d+uchar-.vbl_array_2d+uchar-.uchar.uchar-.cxx

# End Source File
# Begin Source File

SOURCE=.\vipl_with_vbl_array_2d/Templates/vipl_gradient_mag+vbl_array_2d+uchar-.vbl_array_2d+uchar-.uchar.uchar-.cxx

# End Source File
# Begin Source File

SOURCE=.\vipl_with_vbl_array_2d/Templates/vipl_sobel+vbl_array_2d+uchar-.vbl_array_2d+uchar-.uchar.uchar-.cxx

# End Source File
# Begin Source File

SOURCE=.\vipl_with_vnl_matrix/Templates/vipl_accessors_vnl_matrix+uchar-.cxx

# End Source File
# Begin Source File

SOURCE=.\vipl_with_vnl_matrix/Templates/vipl_filter_2d+vnl_matrix+uchar-.vnl_matrix+uchar-.uchar.uchar-.cxx

# End Source File
# Begin Source File

SOURCE=.\vipl_with_vnl_matrix/Templates/vipl_median+vnl_matrix+uchar-.vnl_matrix+uchar-.uchar.uchar-.cxx

# End Source File
# Begin Source File

SOURCE=.\vipl_with_vnl_matrix/Templates/vipl_threshold+vnl_matrix+uchar-.vnl_matrix+uchar-.uchar.uchar-.cxx

# End Source File
# Begin Source File

SOURCE=.\vipl_with_section/Templates/vipl_x_gradient+section+vil_rgb+uchar-.2-.section+vil_rgb+uchar-.2-.vil_rgb+uchar-.vil_rgb+uchar--.cxx

# End Source File
# Begin Source File

SOURCE=.\vipl_with_section/Templates/vipl_threshold+section+uchar.2-.section+uchar.2-.uchar.uchar-.cxx

# End Source File
# Begin Source File

SOURCE=.\vipl_with_section/Templates/vipl_histogram+section+uchar.2-.section+int.2-.uchar.int-.cxx

# End Source File
# Begin Source File

SOURCE=.\vipl_with_section/Templates/vipl_filter_2d+section+vil_rgb+uchar-.2-.section+vil_rgb+uchar-.2-.vil_rgb+uchar-.vil_rgb+uchar--.cxx

# End Source File
# Begin Source File

SOURCE=.\vipl_with_section/Templates/vipl_filter_2d+section+uchar.2-.section+uchar.2-.uchar.uchar-.cxx

# End Source File
# Begin Source File

SOURCE=.\vipl_with_section/Templates/vipl_filter_2d+section+uchar.2-.section+int.2-.uchar.int-.cxx

# End Source File
# Begin Source File

SOURCE=.\vipl_with_section/Templates/vipl_erode_disk+section+uchar.2-.section+uchar.2-.uchar.uchar-.cxx

# End Source File
# Begin Source File

SOURCE=.\vipl_with_section/Templates/vipl_dilate_disk+section+vil_rgb+uchar-.2-.section+vil_rgb+uchar-.2-.vil_rgb+uchar-.vil_rgb+uchar--.cxx

# End Source File
# Begin Source File

SOURCE=.\vipl_with_section/Templates/vipl_accessors_section+vil_rgb+uchar-.2-.cxx

# End Source File
# Begin Source File

SOURCE=.\vipl_with_section/Templates/vipl_accessors_section+uchar.2-.cxx

# End Source File
# Begin Source File

SOURCE=.\vipl_with_section/Templates/vipl_accessors_section+int.2-.cxx

# End Source File
# End Group
# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project

