# Microsoft Developer Studio Project File - Name="vpdfl" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# ITK DSP Header file
# This file is read by the build system of itk, and is used as the top part of
# a microsoft project dsp header file
# IF this is in a dsp file, then it is not the header, but has
# already been used, so do not edit here...

# variables to REPLACE
# 
# /I "$(IUEROOT)/vcl" /I "$(IUEROOT)/vxl" /I "$(IUEROOT)/vcl" /I "$(IUEROOT)/vcl/config.win32-VC60" /I "$(IUEROOT)/vxl" /I "$(IUEROOT)/mul"  == include path
#   -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE == compiler defines
#  == override in output directory
# vpdfl  == name of output library

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=vpdfl - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "vpdfl.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "vpdfl.mak" CFG="vpdfl - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "vpdfl - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "vpdfl - Win32 MinSizeRel" (based on "Win32 (x86) Static Library")
!MESSAGE "vpdfl - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "vpdfl - Win32 RelWithDebInfo" (based on "Win32 (x86) Static Library")
# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "vpdfl - Win32 Release"

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
# ADD CPP /I "$(IUEROOT)/vcl" /I "$(IUEROOT)/vxl" /I "$(IUEROOT)/vcl" /I "$(IUEROOT)/vcl/config.win32-VC60" /I "$(IUEROOT)/vxl" /I "$(IUEROOT)/mul"    -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE /D "vpdfl_EXPORTS"
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

!ELSEIF  "$(CFG)" == "vpdfl - Win32 Debug"

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
# ADD CPP /I "$(IUEROOT)/vcl" /I "$(IUEROOT)/vxl" /I "$(IUEROOT)/vcl" /I "$(IUEROOT)/vcl/config.win32-VC60" /I "$(IUEROOT)/vxl" /I "$(IUEROOT)/mul"    -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE /D "vpdfl_EXPORTS"
# ADD CPP /W3 /Zm1000 /GX /GR
# ADD CPP /MDd /Zi /Od /GZ
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
!ELSEIF  "$(CFG)" == "vpdfl - Win32 MinSizeRel"

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
# ADD CPP /I "$(IUEROOT)/vcl" /I "$(IUEROOT)/vxl" /I "$(IUEROOT)/vcl" /I "$(IUEROOT)/vcl/config.win32-VC60" /I "$(IUEROOT)/vxl" /I "$(IUEROOT)/mul"    -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE /D "vpdfl_EXPORTS"
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

!ELSEIF  "$(CFG)" == "vpdfl - Win32 RelWithDebInfo"

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
# ADD CPP /I "$(IUEROOT)/vcl" /I "$(IUEROOT)/vxl" /I "$(IUEROOT)/vcl" /I "$(IUEROOT)/vcl/config.win32-VC60" /I "$(IUEROOT)/vxl" /I "$(IUEROOT)/mul"    -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE /D "vpdfl_EXPORTS"
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

# Name "vpdfl - Win32 Release"
# Name "vpdfl - Win32 Debug"
# Name "vpdfl - Win32 MinSizeRel"
# Name "vpdfl - Win32 RelWithDebInfo"


# Begin Source File

SOURCE=.\"$(IUEROOT)/mul/vpdfl/CMakeLists.txt"

!IF  "$(CFG)" == "vpdfl - Win32 Release"
USERDEP__HACK=\
	"$(IUEROOT)/CMakeLists.txt"\
	"$(IUEROOT)/mul/CMakeLists.txt"\
	"$(IUEROOT)/mul/vpdfl/CMakeLists.txt"
# Begin Custom Build

"vpdfl.dsp" :  "$(SOURCE)" "$(INTDIR)" "$(OUTDIR)"
	"$(CMAKE_ROOT)/Source/cmake.exe" "$(IUEROOT)/mul/vpdfl/CMakeLists.txt" -DSP -H"$(IUEROOT)" -S"$(IUEROOT)/mul/vpdfl" -O"$(IUEROOT)/mul/vpdfl" -B"$(IUEROOT)"

# End Custom Build

!ELSEIF  "$(CFG)" == "vpdfl - Win32 Debug"
USERDEP__HACK=\
	"$(IUEROOT)/CMakeLists.txt"\
	"$(IUEROOT)/mul/CMakeLists.txt"\
	"$(IUEROOT)/mul/vpdfl/CMakeLists.txt"
# Begin Custom Build

"vpdfl.dsp" :  "$(SOURCE)" "$(INTDIR)" "$(OUTDIR)"
	"$(CMAKE_ROOT)/Source/cmake.exe" "$(IUEROOT)/mul/vpdfl/CMakeLists.txt" -DSP -H"$(IUEROOT)" -S"$(IUEROOT)/mul/vpdfl" -O"$(IUEROOT)/mul/vpdfl" -B"$(IUEROOT)"

# End Custom Build

!ELSEIF  "$(CFG)" == "vpdfl - Win32 MinSizeRel"
USERDEP__HACK=\
	"$(IUEROOT)/CMakeLists.txt"\
	"$(IUEROOT)/mul/CMakeLists.txt"\
	"$(IUEROOT)/mul/vpdfl/CMakeLists.txt"
# Begin Custom Build

"vpdfl.dsp" :  "$(SOURCE)" "$(INTDIR)" "$(OUTDIR)"
	"$(CMAKE_ROOT)/Source/cmake.exe" "$(IUEROOT)/mul/vpdfl/CMakeLists.txt" -DSP -H"$(IUEROOT)" -S"$(IUEROOT)/mul/vpdfl" -O"$(IUEROOT)/mul/vpdfl" -B"$(IUEROOT)"

# End Custom Build

!ELSEIF  "$(CFG)" == "vpdfl - Win32 RelWithDebInfo"
USERDEP__HACK=\
	"$(IUEROOT)/CMakeLists.txt"\
	"$(IUEROOT)/mul/CMakeLists.txt"\
	"$(IUEROOT)/mul/vpdfl/CMakeLists.txt"
# Begin Custom Build

"vpdfl.dsp" :  "$(SOURCE)" "$(INTDIR)" "$(OUTDIR)"
	"$(CMAKE_ROOT)/Source/cmake.exe" "$(IUEROOT)/mul/vpdfl/CMakeLists.txt" -DSP -H"$(IUEROOT)" -S"$(IUEROOT)/mul/vpdfl" -O"$(IUEROOT)/mul/vpdfl" -B"$(IUEROOT)"

# End Custom Build

!ENDIF

# End Source File
# Begin Group "Source Files"
# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Templates/mbl_cloneable_ptr+vpdfl_builder_base-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/mbl_cloneable_ptr+vpdfl_pdf_base-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_vector+vpdfl_builder_base~-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_vector+vpdfl_pdf_base~-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_vector+vpdfl_sampler_base~-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vsl_binary_loader+vpdfl_builder_base-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vsl_binary_loader+vpdfl_pdf_base-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vsl_vector_io+vpdfl_builder_base~-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vsl_vector_io+vpdfl_pdf_base~-.cxx

# End Source File
# Begin Source File

SOURCE=.\vpdfl_axis_gaussian.cxx

# End Source File
# Begin Source File

SOURCE=.\vpdfl_axis_gaussian_builder.cxx

# End Source File
# Begin Source File

SOURCE=.\vpdfl_axis_gaussian_sampler.cxx

# End Source File
# Begin Source File

SOURCE=.\vpdfl_builder_base.cxx

# End Source File
# Begin Source File

SOURCE=.\vpdfl_gaussian.cxx

# End Source File
# Begin Source File

SOURCE=.\vpdfl_gaussian_builder.cxx

# End Source File
# Begin Source File

SOURCE=.\vpdfl_gaussian_sampler.cxx

# End Source File
# Begin Source File

SOURCE=.\vpdfl_mixture.cxx

# End Source File
# Begin Source File

SOURCE=.\vpdfl_mixture_builder.cxx

# End Source File
# Begin Source File

SOURCE=.\vpdfl_mixture_sampler.cxx

# End Source File
# Begin Source File

SOURCE=.\vpdfl_pc_gaussian.cxx

# End Source File
# Begin Source File

SOURCE=.\vpdfl_pc_gaussian_builder.cxx

# End Source File
# Begin Source File

SOURCE=.\vpdfl_pc_gaussian_sampler.cxx

# End Source File
# Begin Source File

SOURCE=.\vpdfl_pdf_base.cxx

# End Source File
# Begin Source File

SOURCE=.\vpdfl_prob_chi2.cxx

# End Source File
# Begin Source File

SOURCE=.\vpdfl_sampler_base.cxx

# End Source File
# End Group
# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project

