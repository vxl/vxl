# Microsoft Developer Studio Project File - Name="vcl" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# ITK DSP Header file
# This file is read by the build system of itk, and is used as the top part of
# a microsoft project dsp header file
# IF this is in a dsp file, then it is not the header, but has
# already been used, so do not edit here...

# variables to REPLACE
# 
# /I "$(IUEROOT)/vcl" /I "$(IUEROOT)/vxl" /I "$(IUEROOT)/vcl" /I "$(IUEROOT)/vcl/config.win32-VC60" /I "config.win32-vc60"  == include path
#   -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE -DBUILDING_VCL_DLL == compiler defines
#  == override in output directory
# vcl  == name of output library

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=vcl - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "vcl.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "vcl.mak" CFG="vcl - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "vcl - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "vcl - Win32 MinSizeRel" (based on "Win32 (x86) Static Library")
!MESSAGE "vcl - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "vcl - Win32 RelWithDebInfo" (based on "Win32 (x86) Static Library")
# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "vcl - Win32 Release"

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
# ADD CPP /I "$(IUEROOT)/vcl" /I "$(IUEROOT)/vxl" /I "$(IUEROOT)/vcl" /I "$(IUEROOT)/vcl/config.win32-VC60" /I "config.win32-vc60"    -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE -DBUILDING_VCL_DLL /D "vcl_EXPORTS"
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

!ELSEIF  "$(CFG)" == "vcl - Win32 Debug"

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
# ADD CPP /I "$(IUEROOT)/vcl" /I "$(IUEROOT)/vxl" /I "$(IUEROOT)/vcl" /I "$(IUEROOT)/vcl/config.win32-VC60" /I "config.win32-vc60"    -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE -DBUILDING_VCL_DLL /D "vcl_EXPORTS"
# ADD CPP /W3 /Zm1000 /GX /GR
# ADD CPP /MDd /Zi /Od /GZ
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
!ELSEIF  "$(CFG)" == "vcl - Win32 MinSizeRel"

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
# ADD CPP /I "$(IUEROOT)/vcl" /I "$(IUEROOT)/vxl" /I "$(IUEROOT)/vcl" /I "$(IUEROOT)/vcl/config.win32-VC60" /I "config.win32-vc60"    -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE -DBUILDING_VCL_DLL /D "vcl_EXPORTS"
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

!ELSEIF  "$(CFG)" == "vcl - Win32 RelWithDebInfo"

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
# ADD CPP /I "$(IUEROOT)/vcl" /I "$(IUEROOT)/vxl" /I "$(IUEROOT)/vcl" /I "$(IUEROOT)/vcl/config.win32-VC60" /I "config.win32-vc60"    -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE -DBUILDING_VCL_DLL /D "vcl_EXPORTS"
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

# Name "vcl - Win32 Release"
# Name "vcl - Win32 Debug"
# Name "vcl - Win32 MinSizeRel"
# Name "vcl - Win32 RelWithDebInfo"


# Begin Source File

SOURCE=.\CMakeLists.txt

!IF  "$(CFG)" == "vcl - Win32 Release"
USERDEP__HACK=\
	"$(IUEROOT)/CMakeLists.txt"\
	"$(IUEROOT)/vcl/CMakeLists.txt"
# Begin Custom Build

"vcl.dsp" :  "$(SOURCE)" "$(INTDIR)" "$(OUTDIR)"
	"$(CMAKE_ROOT)/Source/cmake.exe" "$(IUEROOT)/vcl/CMakeLists.txt" -DSP -H"$(IUEROOT)" -S"$(IUEROOT)/vcl" -O"$(IUEROOT)/vcl" -B"$(IUEROOT)"

# End Custom Build

!ELSEIF  "$(CFG)" == "vcl - Win32 Debug"
USERDEP__HACK=\
	"$(IUEROOT)/CMakeLists.txt"\
	"$(IUEROOT)/vcl/CMakeLists.txt"
# Begin Custom Build

"vcl.dsp" :  "$(SOURCE)" "$(INTDIR)" "$(OUTDIR)"
	"$(CMAKE_ROOT)/Source/cmake.exe" "$(IUEROOT)/vcl/CMakeLists.txt" -DSP -H"$(IUEROOT)" -S"$(IUEROOT)/vcl" -O"$(IUEROOT)/vcl" -B"$(IUEROOT)"

# End Custom Build

!ELSEIF  "$(CFG)" == "vcl - Win32 MinSizeRel"
USERDEP__HACK=\
	"$(IUEROOT)/CMakeLists.txt"\
	"$(IUEROOT)/vcl/CMakeLists.txt"
# Begin Custom Build

"vcl.dsp" :  "$(SOURCE)" "$(INTDIR)" "$(OUTDIR)"
	"$(CMAKE_ROOT)/Source/cmake.exe" "$(IUEROOT)/vcl/CMakeLists.txt" -DSP -H"$(IUEROOT)" -S"$(IUEROOT)/vcl" -O"$(IUEROOT)/vcl" -B"$(IUEROOT)"

# End Custom Build

!ELSEIF  "$(CFG)" == "vcl - Win32 RelWithDebInfo"
USERDEP__HACK=\
	"$(IUEROOT)/CMakeLists.txt"\
	"$(IUEROOT)/vcl/CMakeLists.txt"
# Begin Custom Build

"vcl.dsp" :  "$(SOURCE)" "$(INTDIR)" "$(OUTDIR)"
	"$(CMAKE_ROOT)/Source/cmake.exe" "$(IUEROOT)/vcl/CMakeLists.txt" -DSP -H"$(IUEROOT)" -S"$(IUEROOT)/vcl" -O"$(IUEROOT)/vcl" -B"$(IUEROOT)"

# End Custom Build

!ENDIF

# End Source File
# Begin Group "Source Files"
# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Templates/alloc-instances.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/complex-instances.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/fstream-instances.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/stream-instances.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/string-instances.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_algorithm+bool-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_algorithm+double-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_algorithm+float-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_algorithm+int-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_algorithm+uchar-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_algorithm+unsigned-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_algorithm+ushort-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_algorithm+vcl_pair+double.int--.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_algorithm+vcl_pair+float.int--.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_deque+int-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_list+float-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_list+float~-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_list+int-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_list+uint-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_list+ulong-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_list+vcl_pair+void~.int--.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_list+vcl_pair+void~.void~--.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_list+vcl_string-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_list+void~-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_map+double.int-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_map+int.double-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_map+int.int-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_map+int.uint-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_map+int.vcl_string-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_map+int.void~-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_map+long.void~-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_map+ulong.vcl_pair+void~.int--.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_map+unsigned_int.double-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_map+unsigned_int.int-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_map+unsigned_int.unsigned_int-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_map+unsigned_int.void~-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_map+vcl_pair+int.int-.vcl_pair+float.float--.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_map+vcl_pair+int.int-.void~-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_map+vcl_pair+uint.uint-.double-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_map+vcl_pair+uint.uint-.int-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_map+vcl_string.int-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_map+vcl_string.vcl_string-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_map+vcl_string.void~-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_map+void~.int-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_map+void~.vcl_pair+int.int--.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_map+void~.vcl_pair+ulong.int--.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_map+void~.void~-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_ostream_iterator+int-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_pair+float.float-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_pair+void~.int-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_queue+double-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_set+int-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_set+unsigned-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_set+vcl_pair+int.int--.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_set+vcl_string-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_set+void~-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_unique+double~-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_vector+bool-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_vector+bool~-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_vector+char-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_vector+char~-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_vector+const_char~-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_vector+double-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_vector+double_const~-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_vector+double~-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_vector+float-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_vector+float~-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_vector+int-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_vector+int~-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_vector+long-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_vector+long_double-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_vector+long_double~-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_vector+long~-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_vector+schar-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_vector+schar~-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_vector+uchar-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_vector+uchar~-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_vector+uint-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_vector+uint~-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_vector+ulong-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_vector+ulong~-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_vector+vcl_complex+double--.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_vector+vcl_complex+double-~-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_vector+vcl_complex+float--.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_vector+vcl_complex+float-~-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_vector+vcl_complex+long_double--.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_vector+vcl_complex+long_double-~-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_vector+vcl_map+int.int--.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_vector+vcl_map+uint.uint--.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_vector+vcl_pair+char~.int--.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_vector+vcl_pair+double.double--.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_vector+vcl_pair+double.int--.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_vector+vcl_pair+float.float--.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_vector+vcl_pair+float.int--.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_vector+vcl_pair+int.double~--.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_vector+vcl_pair+int.int--.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_vector+vcl_pair+int.int-~-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_vector+vcl_pair+int.vcl_string--.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_vector+vcl_pair+uint.uint--.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_vector+vcl_pair+vcl_string.vcl_string--.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_vector+vcl_string-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_vector+vcl_vector+double--.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_vector+vcl_vector+int--.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_vector+vcl_vector+int-~-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_vector+vcl_vector+unsigned--.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_vector+vcl_vector+unsigned-~-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_vector+void~-.cxx

# End Source File
# Begin Source File

SOURCE=.\emulation/vcl_alloc.cxx

# End Source File
# Begin Source File

SOURCE=.\emulation/vcl_hashtable.cxx

# End Source File
# Begin Source File

SOURCE=.\emulation/vcl_rbtree_instances.cxx

# End Source File
# Begin Source File

SOURCE=.\emulation/vcl_straits.cxx

# End Source File
# Begin Source File

SOURCE=.\emulation/vcl_string_instances.cxx

# End Source File
# Begin Source File

SOURCE=.\vcl_cassert.cxx

# End Source File
# Begin Source File

SOURCE=.\vcl_cmath.cxx

# End Source File
# Begin Source File

SOURCE=.\vcl_cstdlib.cxx

# End Source File
# Begin Source File

SOURCE=.\vcl_deprecated.cxx

# End Source File
# End Group
# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project

