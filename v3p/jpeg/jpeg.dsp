# Microsoft Developer Studio Project File - Name="jpeg" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# ITK DSP Header file
# This file is read by the build system of itk, and is used as the top part of
# a microsoft project dsp header file
# IF this is in a dsp file, then it is not the header, but has
# already been used, so do not edit here...

# variables to REPLACE
# 
# /I "c:/awf/src/vxl/vcl" /I "c:/awf/src/vxl/vxl" /I "c:/awf/src/vxl/vcl" /I "c:/awf/src/vxl/vcl/config.win32-VC60"  == include path
#   -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE == compiler defines
#  == override in output directory
# jpeg  == name of output library

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=jpeg - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "jpeg.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "jpeg.mak" CFG="jpeg - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "jpeg - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "jpeg - Win32 MinSizeRel" (based on "Win32 (x86) Static Library")
!MESSAGE "jpeg - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "jpeg - Win32 RelWithDebInfo" (based on "Win32 (x86) Static Library")
# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "jpeg - Win32 Release"

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
# ADD CPP /I "c:/awf/src/vxl/vcl" /I "c:/awf/src/vxl/vxl" /I "c:/awf/src/vxl/vcl" /I "c:/awf/src/vxl/vcl/config.win32-VC60"    -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE /D "jpeg_EXPORTS"
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

!ELSEIF  "$(CFG)" == "jpeg - Win32 Debug"

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
# ADD CPP /I "c:/awf/src/vxl/vcl" /I "c:/awf/src/vxl/vxl" /I "c:/awf/src/vxl/vcl" /I "c:/awf/src/vxl/vcl/config.win32-VC60"    -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE /D "jpeg_EXPORTS"
# ADD CPP /W3 /Zm1000 /GX /GR
# ADD CPP /MDd /Zi /Od /GZ
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
!ELSEIF  "$(CFG)" == "jpeg - Win32 MinSizeRel"

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
# ADD CPP /I "c:/awf/src/vxl/vcl" /I "c:/awf/src/vxl/vxl" /I "c:/awf/src/vxl/vcl" /I "c:/awf/src/vxl/vcl/config.win32-VC60"    -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE /D "jpeg_EXPORTS"
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

!ELSEIF  "$(CFG)" == "jpeg - Win32 RelWithDebInfo"

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
# ADD CPP /I "c:/awf/src/vxl/vcl" /I "c:/awf/src/vxl/vxl" /I "c:/awf/src/vxl/vcl" /I "c:/awf/src/vxl/vcl/config.win32-VC60"    -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE /D "jpeg_EXPORTS"
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

# Name "jpeg - Win32 Release"
# Name "jpeg - Win32 Debug"
# Name "jpeg - Win32 MinSizeRel"
# Name "jpeg - Win32 RelWithDebInfo"


# Begin Source File

SOURCE="c:/awf/src/vxl/v3p/jpeg/CMakeLists.txt"

!IF  "$(CFG)" == "jpeg - Win32 Release"
USERDEP__HACK=\
	"c:/awf/src/vxl/CMakeLists.txt"\
	"c:/awf/src/vxl/config.cmake/Modules/FindNativeJPEG.cmake"\
	"c:/awf/src/vxl/config.cmake/Modules/FindNativePNG.cmake"\
	"c:/awf/src/vxl/config.cmake/Modules/FindNativeTIFF.cmake"\
	"c:/awf/src/vxl/config.cmake/Modules/FindNativeZLIB.cmake"\
	"c:/awf/src/vxl/v3p/CMakeLists.txt"\
	"c:/awf/src/vxl/v3p/jpeg/CMakeLists.txt"
# Begin Custom Build

"jpeg.dsp" :  "$(SOURCE)" "$(INTDIR)" "$(OUTDIR)"
	"c:/awf/src/CMake/Source/cmake.exe" "c:/awf/src/vxl/v3p/jpeg/CMakeLists.txt" -DSP -H"c:/awf/src/vxl" -S"c:/awf/src/vxl/v3p/jpeg" -O"c:/awf/src/vxl/v3p/jpeg" -B"c:/awf/src/vxl"

# End Custom Build

!ELSEIF  "$(CFG)" == "jpeg - Win32 Debug"
USERDEP__HACK=\
	"c:/awf/src/vxl/CMakeLists.txt"\
	"c:/awf/src/vxl/config.cmake/Modules/FindNativeJPEG.cmake"\
	"c:/awf/src/vxl/config.cmake/Modules/FindNativePNG.cmake"\
	"c:/awf/src/vxl/config.cmake/Modules/FindNativeTIFF.cmake"\
	"c:/awf/src/vxl/config.cmake/Modules/FindNativeZLIB.cmake"\
	"c:/awf/src/vxl/v3p/CMakeLists.txt"\
	"c:/awf/src/vxl/v3p/jpeg/CMakeLists.txt"
# Begin Custom Build

"jpeg.dsp" :  "$(SOURCE)" "$(INTDIR)" "$(OUTDIR)"
	"c:/awf/src/CMake/Source/cmake.exe" "c:/awf/src/vxl/v3p/jpeg/CMakeLists.txt" -DSP -H"c:/awf/src/vxl" -S"c:/awf/src/vxl/v3p/jpeg" -O"c:/awf/src/vxl/v3p/jpeg" -B"c:/awf/src/vxl"

# End Custom Build

!ELSEIF  "$(CFG)" == "jpeg - Win32 MinSizeRel"
USERDEP__HACK=\
	"c:/awf/src/vxl/CMakeLists.txt"\
	"c:/awf/src/vxl/config.cmake/Modules/FindNativeJPEG.cmake"\
	"c:/awf/src/vxl/config.cmake/Modules/FindNativePNG.cmake"\
	"c:/awf/src/vxl/config.cmake/Modules/FindNativeTIFF.cmake"\
	"c:/awf/src/vxl/config.cmake/Modules/FindNativeZLIB.cmake"\
	"c:/awf/src/vxl/v3p/CMakeLists.txt"\
	"c:/awf/src/vxl/v3p/jpeg/CMakeLists.txt"
# Begin Custom Build

"jpeg.dsp" :  "$(SOURCE)" "$(INTDIR)" "$(OUTDIR)"
	"c:/awf/src/CMake/Source/cmake.exe" "c:/awf/src/vxl/v3p/jpeg/CMakeLists.txt" -DSP -H"c:/awf/src/vxl" -S"c:/awf/src/vxl/v3p/jpeg" -O"c:/awf/src/vxl/v3p/jpeg" -B"c:/awf/src/vxl"

# End Custom Build

!ELSEIF  "$(CFG)" == "jpeg - Win32 RelWithDebInfo"
USERDEP__HACK=\
	"c:/awf/src/vxl/CMakeLists.txt"\
	"c:/awf/src/vxl/config.cmake/Modules/FindNativeJPEG.cmake"\
	"c:/awf/src/vxl/config.cmake/Modules/FindNativePNG.cmake"\
	"c:/awf/src/vxl/config.cmake/Modules/FindNativeTIFF.cmake"\
	"c:/awf/src/vxl/config.cmake/Modules/FindNativeZLIB.cmake"\
	"c:/awf/src/vxl/v3p/CMakeLists.txt"\
	"c:/awf/src/vxl/v3p/jpeg/CMakeLists.txt"
# Begin Custom Build

"jpeg.dsp" :  "$(SOURCE)" "$(INTDIR)" "$(OUTDIR)"
	"c:/awf/src/CMake/Source/cmake.exe" "c:/awf/src/vxl/v3p/jpeg/CMakeLists.txt" -DSP -H"c:/awf/src/vxl" -S"c:/awf/src/vxl/v3p/jpeg" -O"c:/awf/src/vxl/v3p/jpeg" -B"c:/awf/src/vxl"

# End Custom Build

!ENDIF

# End Source File
# Begin Group "Source Files"
# PROP Default_Filter ""
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/jpeg/jcapimin.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/jpeg/jcapistd.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/jpeg/jccoefct.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/jpeg/jccolor.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/jpeg/jcdctmgr.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/jpeg/jchuff.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/jpeg/jcinit.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/jpeg/jcmainct.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/jpeg/jcmarker.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/jpeg/jcmaster.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/jpeg/jcomapi.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/jpeg/jcparam.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/jpeg/jcphuff.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/jpeg/jcprepct.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/jpeg/jcsample.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/jpeg/jctrans.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/jpeg/jdapimin.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/jpeg/jdapistd.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/jpeg/jdatadst.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/jpeg/jdatasrc.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/jpeg/jdcoefct.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/jpeg/jdcolor.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/jpeg/jddctmgr.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/jpeg/jdhuff.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/jpeg/jdinput.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/jpeg/jdmainct.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/jpeg/jdmarker.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/jpeg/jdmaster.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/jpeg/jdmerge.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/jpeg/jdphuff.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/jpeg/jdpostct.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/jpeg/jdsample.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/jpeg/jdtrans.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/jpeg/jerror.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/jpeg/jfdctflt.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/jpeg/jfdctfst.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/jpeg/jfdctint.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/jpeg/jidctflt.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/jpeg/jidctfst.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/jpeg/jidctint.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/jpeg/jidctred.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/jpeg/jmemansi.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/jpeg/jmemmgr.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/jpeg/jquant1.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/jpeg/jquant2.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/jpeg/jutils.c

# End Source File
# End Group
# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project

