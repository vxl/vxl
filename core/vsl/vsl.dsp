# Microsoft Developer Studio Project File - Name="vsl" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# ITK DSP Header file
# This file is read by the build system of itk, and is used as the top part of
# a microsoft project dsp header file
# IF this is in a dsp file, then it is not the header, but has
# already been used, so do not edit here...

# variables to REPLACE
# 
# /I "c:/awf/src/vxl/vcl" /I "c:/awf/src/vxl/vxl" /I "c:/awf/src/vxl/vcl" /I "c:/awf/src/vxl/vcl/config.win32-VC60" /I "c:/awf/src/vxl/vxl"  == include path
#   -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE == compiler defines
#  == override in output directory
# vsl  == name of output library

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=vsl - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "vsl.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "vsl.mak" CFG="vsl - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "vsl - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "vsl - Win32 MinSizeRel" (based on "Win32 (x86) Static Library")
!MESSAGE "vsl - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "vsl - Win32 RelWithDebInfo" (based on "Win32 (x86) Static Library")
# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "vsl - Win32 Release"

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
# ADD CPP /I "c:/awf/src/vxl/vcl" /I "c:/awf/src/vxl/vxl" /I "c:/awf/src/vxl/vcl" /I "c:/awf/src/vxl/vcl/config.win32-VC60" /I "c:/awf/src/vxl/vxl"    -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE /D "vsl_EXPORTS"
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

!ELSEIF  "$(CFG)" == "vsl - Win32 Debug"

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
# ADD CPP /I "c:/awf/src/vxl/vcl" /I "c:/awf/src/vxl/vxl" /I "c:/awf/src/vxl/vcl" /I "c:/awf/src/vxl/vcl/config.win32-VC60" /I "c:/awf/src/vxl/vxl"    -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE /D "vsl_EXPORTS"
# ADD CPP /W3 /Zm1000 /GX /GR
# ADD CPP /MDd /Zi /Od /GZ
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
!ELSEIF  "$(CFG)" == "vsl - Win32 MinSizeRel"

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
# ADD CPP /I "c:/awf/src/vxl/vcl" /I "c:/awf/src/vxl/vxl" /I "c:/awf/src/vxl/vcl" /I "c:/awf/src/vxl/vcl/config.win32-VC60" /I "c:/awf/src/vxl/vxl"    -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE /D "vsl_EXPORTS"
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

!ELSEIF  "$(CFG)" == "vsl - Win32 RelWithDebInfo"

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
# ADD CPP /I "c:/awf/src/vxl/vcl" /I "c:/awf/src/vxl/vxl" /I "c:/awf/src/vxl/vcl" /I "c:/awf/src/vxl/vcl/config.win32-VC60" /I "c:/awf/src/vxl/vxl"    -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE /D "vsl_EXPORTS"
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

# Name "vsl - Win32 Release"
# Name "vsl - Win32 Debug"
# Name "vsl - Win32 MinSizeRel"
# Name "vsl - Win32 RelWithDebInfo"


# Begin Source File

SOURCE="c:/awf/src/vxl/vxl/vsl/CMakeLists.txt"

!IF  "$(CFG)" == "vsl - Win32 Release"
USERDEP__HACK=\
	"c:/awf/src/vxl/CMakeLists.txt"\
	"c:/awf/src/vxl/vxl/CMakeLists.txt"\
	"c:/awf/src/vxl/vxl/vsl/CMakeLists.txt"
# Begin Custom Build

"vsl.dsp" :  "$(SOURCE)" "$(INTDIR)" "$(OUTDIR)"
	"c:/awf/src/CMake/Source/cmake.exe" "c:/awf/src/vxl/vxl/vsl/CMakeLists.txt" -DSP -H"c:/awf/src/vxl" -S"c:/awf/src/vxl/vxl/vsl" -O"c:/awf/src/vxl/vxl/vsl" -B"c:/awf/src/vxl"

# End Custom Build

!ELSEIF  "$(CFG)" == "vsl - Win32 Debug"
USERDEP__HACK=\
	"c:/awf/src/vxl/CMakeLists.txt"\
	"c:/awf/src/vxl/vxl/CMakeLists.txt"\
	"c:/awf/src/vxl/vxl/vsl/CMakeLists.txt"
# Begin Custom Build

"vsl.dsp" :  "$(SOURCE)" "$(INTDIR)" "$(OUTDIR)"
	"c:/awf/src/CMake/Source/cmake.exe" "c:/awf/src/vxl/vxl/vsl/CMakeLists.txt" -DSP -H"c:/awf/src/vxl" -S"c:/awf/src/vxl/vxl/vsl" -O"c:/awf/src/vxl/vxl/vsl" -B"c:/awf/src/vxl"

# End Custom Build

!ELSEIF  "$(CFG)" == "vsl - Win32 MinSizeRel"
USERDEP__HACK=\
	"c:/awf/src/vxl/CMakeLists.txt"\
	"c:/awf/src/vxl/vxl/CMakeLists.txt"\
	"c:/awf/src/vxl/vxl/vsl/CMakeLists.txt"
# Begin Custom Build

"vsl.dsp" :  "$(SOURCE)" "$(INTDIR)" "$(OUTDIR)"
	"c:/awf/src/CMake/Source/cmake.exe" "c:/awf/src/vxl/vxl/vsl/CMakeLists.txt" -DSP -H"c:/awf/src/vxl" -S"c:/awf/src/vxl/vxl/vsl" -O"c:/awf/src/vxl/vxl/vsl" -B"c:/awf/src/vxl"

# End Custom Build

!ELSEIF  "$(CFG)" == "vsl - Win32 RelWithDebInfo"
USERDEP__HACK=\
	"c:/awf/src/vxl/CMakeLists.txt"\
	"c:/awf/src/vxl/vxl/CMakeLists.txt"\
	"c:/awf/src/vxl/vxl/vsl/CMakeLists.txt"
# Begin Custom Build

"vsl.dsp" :  "$(SOURCE)" "$(INTDIR)" "$(OUTDIR)"
	"c:/awf/src/CMake/Source/cmake.exe" "c:/awf/src/vxl/vxl/vsl/CMakeLists.txt" -DSP -H"c:/awf/src/vxl" -S"c:/awf/src/vxl/vxl/vsl" -O"c:/awf/src/vxl/vxl/vsl" -B"c:/awf/src/vxl"

# End Custom Build

!ENDIF

# End Source File
# Begin Group "Source Files"
# PROP Default_Filter ""
# Begin Source File

SOURCE=c:/awf/src/vxl/vxl/vsl/Templates/vcl_vector+vsl_binary_loader_base~-.cxx

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/vxl/vsl/Templates/vsl_complex+double-.cxx

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/vxl/vsl/Templates/vsl_complex+float-.cxx

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/vxl/vsl/Templates/vsl_deque+int-.cxx

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/vxl/vsl/Templates/vsl_list+float-.cxx

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/vxl/vsl/Templates/vsl_list+int-.cxx

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/vxl/vsl/Templates/vsl_map_io+int.int-.cxx

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/vxl/vsl/Templates/vsl_map_io+int.vcl_string-.cxx

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/vxl/vsl/Templates/vsl_map_io+vcl_string.int-.cxx

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/vxl/vsl/Templates/vsl_pair+uint.uint-.cxx

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/vxl/vsl/Templates/vsl_pair+vcl_pair+uint.uint-.double-.cxx

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/vxl/vsl/Templates/vsl_pair+vcl_pair+uint.uint-.int-.cxx

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/vxl/vsl/Templates/vsl_set+int-.cxx

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/vxl/vsl/Templates/vsl_set+vcl_string-.cxx

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/vxl/vsl/Templates/vsl_stack+int-.cxx

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/vxl/vsl/Templates/vsl_vector_io+double-.cxx

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/vxl/vsl/Templates/vsl_vector_io+float-.cxx

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/vxl/vsl/Templates/vsl_vector_io+int-.cxx

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/vxl/vsl/Templates/vsl_vector_io+uint-.cxx

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/vxl/vsl/Templates/vsl_vector_io+vcl_string-.cxx

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/vxl/vsl/vsl_binary_io.cxx

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/vxl/vsl/vsl_binary_loader_base.cxx

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/vxl/vsl/vsl_indent.cxx

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/vxl/vsl/vsl_test.cxx

# End Source File
# End Group
# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project

