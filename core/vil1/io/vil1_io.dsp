# Microsoft Developer Studio Project File - Name="vil_io" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# ITK DSP Header file
# This file is read by the build system of itk, and is used as the top part of
# a microsoft project dsp header file
# IF this is in a dsp file, then it is not the header, but has
# already been used, so do not edit here...

# variables to REPLACE
# 
# /I "$(IUEROOT)/vcl" /I "$(IUEROOT)/vxl" /I "$(IUEROOT)/vcl" /I "$(IUEROOT)/vcl/config.win32-VC60" /I "$(IUEROOT)/vxl" /I "$(IUEROOT)/v3p/jpeg" /I "$(IUEROOT)/v3p/png" /I "$(IUEROOT)/v3p/zlib" /I "$(IUEROOT)/v3p/tiff"  == include path
#   -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE -DHAS_JPEG -DHAS_PNG -DHAS_ZLIB -DHAS_TIFF == compiler defines
#  == override in output directory
# vil_io  == name of output library

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=vil_io - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "vil_io.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "vil_io.mak" CFG="vil_io - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "vil_io - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "vil_io - Win32 MinSizeRel" (based on "Win32 (x86) Static Library")
!MESSAGE "vil_io - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "vil_io - Win32 RelWithDebInfo" (based on "Win32 (x86) Static Library")
# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "vil_io - Win32 Release"

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
# ADD CPP /I "$(IUEROOT)/vcl" /I "$(IUEROOT)/vxl" /I "$(IUEROOT)/vcl" /I "$(IUEROOT)/vcl/config.win32-VC60" /I "$(IUEROOT)/vxl" /I "$(IUEROOT)/v3p/jpeg" /I "$(IUEROOT)/v3p/png" /I "$(IUEROOT)/v3p/zlib" /I "$(IUEROOT)/v3p/tiff"    -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE -DHAS_JPEG -DHAS_PNG -DHAS_ZLIB -DHAS_TIFF /D "vil_io_EXPORTS"
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

!ELSEIF  "$(CFG)" == "vil_io - Win32 Debug"

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
# ADD CPP /I "$(IUEROOT)/vcl" /I "$(IUEROOT)/vxl" /I "$(IUEROOT)/vcl" /I "$(IUEROOT)/vcl/config.win32-VC60" /I "$(IUEROOT)/vxl" /I "$(IUEROOT)/v3p/jpeg" /I "$(IUEROOT)/v3p/png" /I "$(IUEROOT)/v3p/zlib" /I "$(IUEROOT)/v3p/tiff"    -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE -DHAS_JPEG -DHAS_PNG -DHAS_ZLIB -DHAS_TIFF /D "vil_io_EXPORTS"
# ADD CPP /W3 /Zm1000 /GX /GR
# ADD CPP /MDd /Zi /Od /GZ
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
!ELSEIF  "$(CFG)" == "vil_io - Win32 MinSizeRel"

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
# ADD CPP /I "$(IUEROOT)/vcl" /I "$(IUEROOT)/vxl" /I "$(IUEROOT)/vcl" /I "$(IUEROOT)/vcl/config.win32-VC60" /I "$(IUEROOT)/vxl" /I "$(IUEROOT)/v3p/jpeg" /I "$(IUEROOT)/v3p/png" /I "$(IUEROOT)/v3p/zlib" /I "$(IUEROOT)/v3p/tiff"    -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE -DHAS_JPEG -DHAS_PNG -DHAS_ZLIB -DHAS_TIFF /D "vil_io_EXPORTS"
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

!ELSEIF  "$(CFG)" == "vil_io - Win32 RelWithDebInfo"

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
# ADD CPP /I "$(IUEROOT)/vcl" /I "$(IUEROOT)/vxl" /I "$(IUEROOT)/vcl" /I "$(IUEROOT)/vcl/config.win32-VC60" /I "$(IUEROOT)/vxl" /I "$(IUEROOT)/v3p/jpeg" /I "$(IUEROOT)/v3p/png" /I "$(IUEROOT)/v3p/zlib" /I "$(IUEROOT)/v3p/tiff"    -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE -DHAS_JPEG -DHAS_PNG -DHAS_ZLIB -DHAS_TIFF /D "vil_io_EXPORTS"
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

# Name "vil_io - Win32 Release"
# Name "vil_io - Win32 Debug"
# Name "vil_io - Win32 MinSizeRel"
# Name "vil_io - Win32 RelWithDebInfo"


# Begin Source File

SOURCE=.\CMakeLists.txt

!IF  "$(CFG)" == "vil_io - Win32 Release"
USERDEP__HACK=\
	"$(IUEROOT)/CMakeLists.txt"\
	"$(IUEROOT)/config.cmake/Modules/FindJPEG.cmake"\
	"$(IUEROOT)/config.cmake/Modules/FindNativeJPEG.cmake"\
	"$(IUEROOT)/config.cmake/Modules/FindNativePNG.cmake"\
	"$(IUEROOT)/config.cmake/Modules/FindNativeTIFF.cmake"\
	"$(IUEROOT)/config.cmake/Modules/FindNativeZLIB.cmake"\
	"$(IUEROOT)/config.cmake/Modules/FindPNG.cmake"\
	"$(IUEROOT)/config.cmake/Modules/FindTIFF.cmake"\
	"$(IUEROOT)/config.cmake/Modules/FindZLIB.cmake"\
	"$(IUEROOT)/v3p/jpeg/CMakeListsLink.txt"\
	"$(IUEROOT)/v3p/png/CMakeListsLink.txt"\
	"$(IUEROOT)/v3p/tiff/CMakeListsLink.txt"\
	"$(IUEROOT)/v3p/zlib/CMakeListsLink.txt"\
	"$(IUEROOT)/vxl/CMakeLists.txt"\
	"$(IUEROOT)/vxl/vil/CMakeLists.txt"\
	"$(IUEROOT)/vxl/vil/io/CMakeLists.txt"
# Begin Custom Build

"vil_io.dsp" :  "$(SOURCE)" "$(INTDIR)" "$(OUTDIR)"
	"$(CMAKE_ROOT)/Source/cmake.exe" "$(IUEROOT)/vxl/vil/io/CMakeLists.txt" -DSP -H"$(IUEROOT)" -S"$(IUEROOT)/vxl/vil/io" -O"$(IUEROOT)/vxl/vil/io" -B"$(IUEROOT)"

# End Custom Build

!ELSEIF  "$(CFG)" == "vil_io - Win32 Debug"
USERDEP__HACK=\
	"$(IUEROOT)/CMakeLists.txt"\
	"$(IUEROOT)/config.cmake/Modules/FindJPEG.cmake"\
	"$(IUEROOT)/config.cmake/Modules/FindNativeJPEG.cmake"\
	"$(IUEROOT)/config.cmake/Modules/FindNativePNG.cmake"\
	"$(IUEROOT)/config.cmake/Modules/FindNativeTIFF.cmake"\
	"$(IUEROOT)/config.cmake/Modules/FindNativeZLIB.cmake"\
	"$(IUEROOT)/config.cmake/Modules/FindPNG.cmake"\
	"$(IUEROOT)/config.cmake/Modules/FindTIFF.cmake"\
	"$(IUEROOT)/config.cmake/Modules/FindZLIB.cmake"\
	"$(IUEROOT)/v3p/jpeg/CMakeListsLink.txt"\
	"$(IUEROOT)/v3p/png/CMakeListsLink.txt"\
	"$(IUEROOT)/v3p/tiff/CMakeListsLink.txt"\
	"$(IUEROOT)/v3p/zlib/CMakeListsLink.txt"\
	"$(IUEROOT)/vxl/CMakeLists.txt"\
	"$(IUEROOT)/vxl/vil/CMakeLists.txt"\
	"$(IUEROOT)/vxl/vil/io/CMakeLists.txt"
# Begin Custom Build

"vil_io.dsp" :  "$(SOURCE)" "$(INTDIR)" "$(OUTDIR)"
	"$(CMAKE_ROOT)/Source/cmake.exe" "$(IUEROOT)/vxl/vil/io/CMakeLists.txt" -DSP -H"$(IUEROOT)" -S"$(IUEROOT)/vxl/vil/io" -O"$(IUEROOT)/vxl/vil/io" -B"$(IUEROOT)"

# End Custom Build

!ELSEIF  "$(CFG)" == "vil_io - Win32 MinSizeRel"
USERDEP__HACK=\
	"$(IUEROOT)/CMakeLists.txt"\
	"$(IUEROOT)/config.cmake/Modules/FindJPEG.cmake"\
	"$(IUEROOT)/config.cmake/Modules/FindNativeJPEG.cmake"\
	"$(IUEROOT)/config.cmake/Modules/FindNativePNG.cmake"\
	"$(IUEROOT)/config.cmake/Modules/FindNativeTIFF.cmake"\
	"$(IUEROOT)/config.cmake/Modules/FindNativeZLIB.cmake"\
	"$(IUEROOT)/config.cmake/Modules/FindPNG.cmake"\
	"$(IUEROOT)/config.cmake/Modules/FindTIFF.cmake"\
	"$(IUEROOT)/config.cmake/Modules/FindZLIB.cmake"\
	"$(IUEROOT)/v3p/jpeg/CMakeListsLink.txt"\
	"$(IUEROOT)/v3p/png/CMakeListsLink.txt"\
	"$(IUEROOT)/v3p/tiff/CMakeListsLink.txt"\
	"$(IUEROOT)/v3p/zlib/CMakeListsLink.txt"\
	"$(IUEROOT)/vxl/CMakeLists.txt"\
	"$(IUEROOT)/vxl/vil/CMakeLists.txt"\
	"$(IUEROOT)/vxl/vil/io/CMakeLists.txt"
# Begin Custom Build

"vil_io.dsp" :  "$(SOURCE)" "$(INTDIR)" "$(OUTDIR)"
	"$(CMAKE_ROOT)/Source/cmake.exe" "$(IUEROOT)/vxl/vil/io/CMakeLists.txt" -DSP -H"$(IUEROOT)" -S"$(IUEROOT)/vxl/vil/io" -O"$(IUEROOT)/vxl/vil/io" -B"$(IUEROOT)"

# End Custom Build

!ELSEIF  "$(CFG)" == "vil_io - Win32 RelWithDebInfo"
USERDEP__HACK=\
	"$(IUEROOT)/CMakeLists.txt"\
	"$(IUEROOT)/config.cmake/Modules/FindJPEG.cmake"\
	"$(IUEROOT)/config.cmake/Modules/FindNativeJPEG.cmake"\
	"$(IUEROOT)/config.cmake/Modules/FindNativePNG.cmake"\
	"$(IUEROOT)/config.cmake/Modules/FindNativeTIFF.cmake"\
	"$(IUEROOT)/config.cmake/Modules/FindNativeZLIB.cmake"\
	"$(IUEROOT)/config.cmake/Modules/FindPNG.cmake"\
	"$(IUEROOT)/config.cmake/Modules/FindTIFF.cmake"\
	"$(IUEROOT)/config.cmake/Modules/FindZLIB.cmake"\
	"$(IUEROOT)/v3p/jpeg/CMakeListsLink.txt"\
	"$(IUEROOT)/v3p/png/CMakeListsLink.txt"\
	"$(IUEROOT)/v3p/tiff/CMakeListsLink.txt"\
	"$(IUEROOT)/v3p/zlib/CMakeListsLink.txt"\
	"$(IUEROOT)/vxl/CMakeLists.txt"\
	"$(IUEROOT)/vxl/vil/CMakeLists.txt"\
	"$(IUEROOT)/vxl/vil/io/CMakeLists.txt"
# Begin Custom Build

"vil_io.dsp" :  "$(SOURCE)" "$(INTDIR)" "$(OUTDIR)"
	"$(CMAKE_ROOT)/Source/cmake.exe" "$(IUEROOT)/vxl/vil/io/CMakeLists.txt" -DSP -H"$(IUEROOT)" -S"$(IUEROOT)/vxl/vil/io" -O"$(IUEROOT)/vxl/vil/io" -B"$(IUEROOT)"

# End Custom Build

!ENDIF

# End Source File
# Begin Group "Source Files"
# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Templates/vil_io_memory_image_of+bool-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vil_io_memory_image_of+double-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vil_io_memory_image_of+float-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vil_io_memory_image_of+signed_char-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vil_io_memory_image_of+signed_int-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vil_io_memory_image_of+signed_short-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vil_io_memory_image_of+unsigned_char-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vil_io_memory_image_of+unsigned_int-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vil_io_memory_image_of+unsigned_short-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vil_io_memory_image_of+vcl_complex+double--.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vil_io_memory_image_of+vcl_complex+float--.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vil_io_memory_image_of+vil_rgb+double--.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vil_io_memory_image_of+vil_rgb+float--.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vil_io_memory_image_of+vil_rgb+uchar--.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vil_io_rgb+double-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vil_io_rgb+float-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vil_io_rgba+double-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vil_io_rgba+float-.cxx

# End Source File
# Begin Source File

SOURCE=.\vil_io_image.cxx

# End Source File
# Begin Source File

SOURCE=.\vil_io_image_impl.cxx

# End Source File
# Begin Source File

SOURCE=.\vil_io_memory_image.cxx

# End Source File
# Begin Source File

SOURCE=.\vil_io_memory_image_format.cxx

# End Source File
# Begin Source File

SOURCE=.\vil_io_memory_image_impl.cxx

# End Source File
# End Group
# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project

