# Microsoft Developer Studio Project File - Name="netlib" - Package Owner=<4>
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
# netlib  == name of output library

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=netlib - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "netlib.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "netlib.mak" CFG="netlib - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "netlib - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "netlib - Win32 MinSizeRel" (based on "Win32 (x86) Static Library")
!MESSAGE "netlib - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "netlib - Win32 RelWithDebInfo" (based on "Win32 (x86) Static Library")
# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "netlib - Win32 Release"

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
# ADD CPP /I "c:/awf/src/vxl/vcl" /I "c:/awf/src/vxl/vxl" /I "c:/awf/src/vxl/vcl" /I "c:/awf/src/vxl/vcl/config.win32-VC60"    -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE /D "netlib_EXPORTS"
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

!ELSEIF  "$(CFG)" == "netlib - Win32 Debug"

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
# ADD CPP /I "c:/awf/src/vxl/vcl" /I "c:/awf/src/vxl/vxl" /I "c:/awf/src/vxl/vcl" /I "c:/awf/src/vxl/vcl/config.win32-VC60"    -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE /D "netlib_EXPORTS"
# ADD CPP /W3 /Zm1000 /GX /GR
# ADD CPP /MDd /Zi /Od /GZ
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
!ELSEIF  "$(CFG)" == "netlib - Win32 MinSizeRel"

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
# ADD CPP /I "c:/awf/src/vxl/vcl" /I "c:/awf/src/vxl/vxl" /I "c:/awf/src/vxl/vcl" /I "c:/awf/src/vxl/vcl/config.win32-VC60"    -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE /D "netlib_EXPORTS"
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

!ELSEIF  "$(CFG)" == "netlib - Win32 RelWithDebInfo"

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
# ADD CPP /I "c:/awf/src/vxl/vcl" /I "c:/awf/src/vxl/vxl" /I "c:/awf/src/vxl/vcl" /I "c:/awf/src/vxl/vcl/config.win32-VC60"    -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE /D "netlib_EXPORTS"
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

# Name "netlib - Win32 Release"
# Name "netlib - Win32 Debug"
# Name "netlib - Win32 MinSizeRel"
# Name "netlib - Win32 RelWithDebInfo"


# Begin Source File

SOURCE="c:/awf/src/vxl/v3p/netlib/CMakeLists.txt"

!IF  "$(CFG)" == "netlib - Win32 Release"
USERDEP__HACK=\
	"c:/awf/src/vxl/CMakeLists.txt"\
	"c:/awf/src/vxl/config.cmake/Modules/FindNativeJPEG.cmake"\
	"c:/awf/src/vxl/config.cmake/Modules/FindNativePNG.cmake"\
	"c:/awf/src/vxl/config.cmake/Modules/FindNativeTIFF.cmake"\
	"c:/awf/src/vxl/config.cmake/Modules/FindNativeZLIB.cmake"\
	"c:/awf/src/vxl/v3p/CMakeLists.txt"\
	"c:/awf/src/vxl/v3p/netlib/CMakeLists.txt"
# Begin Custom Build

"netlib.dsp" :  "$(SOURCE)" "$(INTDIR)" "$(OUTDIR)"
	"c:/awf/src/CMake/Source/cmake.exe" "c:/awf/src/vxl/v3p/netlib/CMakeLists.txt" -DSP -H"c:/awf/src/vxl" -S"c:/awf/src/vxl/v3p/netlib" -O"c:/awf/src/vxl/v3p/netlib" -B"c:/awf/src/vxl"

# End Custom Build

!ELSEIF  "$(CFG)" == "netlib - Win32 Debug"
USERDEP__HACK=\
	"c:/awf/src/vxl/CMakeLists.txt"\
	"c:/awf/src/vxl/config.cmake/Modules/FindNativeJPEG.cmake"\
	"c:/awf/src/vxl/config.cmake/Modules/FindNativePNG.cmake"\
	"c:/awf/src/vxl/config.cmake/Modules/FindNativeTIFF.cmake"\
	"c:/awf/src/vxl/config.cmake/Modules/FindNativeZLIB.cmake"\
	"c:/awf/src/vxl/v3p/CMakeLists.txt"\
	"c:/awf/src/vxl/v3p/netlib/CMakeLists.txt"
# Begin Custom Build

"netlib.dsp" :  "$(SOURCE)" "$(INTDIR)" "$(OUTDIR)"
	"c:/awf/src/CMake/Source/cmake.exe" "c:/awf/src/vxl/v3p/netlib/CMakeLists.txt" -DSP -H"c:/awf/src/vxl" -S"c:/awf/src/vxl/v3p/netlib" -O"c:/awf/src/vxl/v3p/netlib" -B"c:/awf/src/vxl"

# End Custom Build

!ELSEIF  "$(CFG)" == "netlib - Win32 MinSizeRel"
USERDEP__HACK=\
	"c:/awf/src/vxl/CMakeLists.txt"\
	"c:/awf/src/vxl/config.cmake/Modules/FindNativeJPEG.cmake"\
	"c:/awf/src/vxl/config.cmake/Modules/FindNativePNG.cmake"\
	"c:/awf/src/vxl/config.cmake/Modules/FindNativeTIFF.cmake"\
	"c:/awf/src/vxl/config.cmake/Modules/FindNativeZLIB.cmake"\
	"c:/awf/src/vxl/v3p/CMakeLists.txt"\
	"c:/awf/src/vxl/v3p/netlib/CMakeLists.txt"
# Begin Custom Build

"netlib.dsp" :  "$(SOURCE)" "$(INTDIR)" "$(OUTDIR)"
	"c:/awf/src/CMake/Source/cmake.exe" "c:/awf/src/vxl/v3p/netlib/CMakeLists.txt" -DSP -H"c:/awf/src/vxl" -S"c:/awf/src/vxl/v3p/netlib" -O"c:/awf/src/vxl/v3p/netlib" -B"c:/awf/src/vxl"

# End Custom Build

!ELSEIF  "$(CFG)" == "netlib - Win32 RelWithDebInfo"
USERDEP__HACK=\
	"c:/awf/src/vxl/CMakeLists.txt"\
	"c:/awf/src/vxl/config.cmake/Modules/FindNativeJPEG.cmake"\
	"c:/awf/src/vxl/config.cmake/Modules/FindNativePNG.cmake"\
	"c:/awf/src/vxl/config.cmake/Modules/FindNativeTIFF.cmake"\
	"c:/awf/src/vxl/config.cmake/Modules/FindNativeZLIB.cmake"\
	"c:/awf/src/vxl/v3p/CMakeLists.txt"\
	"c:/awf/src/vxl/v3p/netlib/CMakeLists.txt"
# Begin Custom Build

"netlib.dsp" :  "$(SOURCE)" "$(INTDIR)" "$(OUTDIR)"
	"c:/awf/src/CMake/Source/cmake.exe" "c:/awf/src/vxl/v3p/netlib/CMakeLists.txt" -DSP -H"c:/awf/src/vxl" -S"c:/awf/src/vxl/v3p/netlib" -O"c:/awf/src/vxl/v3p/netlib" -B"c:/awf/src/vxl"

# End Custom Build

!ENDIF

# End Source File
# Begin Group "Source Files"
# PROP Default_Filter ""
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/netlib/F77_aloc.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/netlib/c_abs.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/netlib/c_div.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/netlib/cabs.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/netlib/camsun.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/netlib/caxpy.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/netlib/ccopy.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/netlib/cdotc.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/netlib/cg.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/netlib/cqrdc.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/netlib/cqrsl.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/netlib/cscal.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/netlib/csrot.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/netlib/csvdc.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/netlib/cswap.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/netlib/d_cnjg.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/netlib/d_imag.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/netlib/d_lg10.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/netlib/d_sign.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/netlib/dasum.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/netlib/daxpy.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/netlib/dcopy.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/netlib/ddot.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/netlib/dgemv.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/netlib/dgeqpf.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/netlib/dgeqr2.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/netlib/dger.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/netlib/dgerq2.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/netlib/dggsvd.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/netlib/dggsvp.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/netlib/dgpfa.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/netlib/dgpfa2f.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/netlib/dgpfa3f.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/netlib/dgpfa5f.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/netlib/dlabad.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/netlib/dlacpy.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/netlib/dladiv.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/netlib/dlags2.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/netlib/dlamch.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/netlib/dlange.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/netlib/dlapll.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/netlib/dlapmt.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/netlib/dlapy2.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/netlib/dlapy3.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/netlib/dlarf.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/netlib/dlarfg.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/netlib/dlartg.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/netlib/dlas2.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/netlib/dlaset.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/netlib/dlassq.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/netlib/dlasv2.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/netlib/dnlaso.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/netlib/dnrm2.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/netlib/dorg2r.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/netlib/dorm2r.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/netlib/dormr2.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/netlib/dpoco.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/netlib/dpodi.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/netlib/dpofa.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/netlib/dposl.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/netlib/dqrdc.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/netlib/dqrsl.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/netlib/drot.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/netlib/drotg.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/netlib/dscal.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/netlib/dsetgpfa.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/netlib/dsvdc.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/netlib/dswap.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/netlib/dtgsja.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/netlib/dtrans.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/netlib/dzasum.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/netlib/enorm.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/netlib/exit.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/netlib/fdjac2.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/netlib/gpfa.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/netlib/gpfa2f.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/netlib/gpfa3f.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/netlib/gpfa5f.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/netlib/idamax.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/netlib/ilaenv.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/netlib/isamax.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/netlib/izamax.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/netlib/izmax1.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/netlib/lbfgs-lb1.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/netlib/lbfgs.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/netlib/lmder.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/netlib/lmder1.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/netlib/lmdif.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/netlib/lmpar.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/netlib/lsame.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/netlib/lsqr.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/netlib/machineparams.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/netlib/pow_dd.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/netlib/pow_di.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/netlib/pow_ii.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/netlib/pow_ri.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/netlib/pythag.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/netlib/qrfac.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/netlib/qrsolv.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/netlib/r_cnjg.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/netlib/r_imag.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/netlib/r_sign.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/netlib/rg.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/netlib/rpoly.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/netlib/rs.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/netlib/rsg.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/netlib/s_cat.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/netlib/s_cmp.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/netlib/s_copy.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/netlib/sasum.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/netlib/saxpy.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/netlib/scnrm2.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/netlib/scopy.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/netlib/sdot.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/netlib/setgpfa.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/netlib/sgemv.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/netlib/sgeqpf.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/netlib/sgeqr2.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/netlib/sger.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/netlib/sgerq2.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/netlib/sggsvd.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/netlib/sggsvp.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/netlib/sig_die.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/netlib/slacpy.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/netlib/slags2.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/netlib/slamch.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/netlib/slange.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/netlib/slapll.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/netlib/slapmt.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/netlib/slapy2.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/netlib/slarf.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/netlib/slarfg.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/netlib/slartg.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/netlib/slas2.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/netlib/slaset.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/netlib/slassq.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/netlib/slasv2.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/netlib/snlaso.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/netlib/snrm2.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/netlib/sorg2r.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/netlib/sorm2r.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/netlib/sormr2.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/netlib/sqrdc.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/netlib/sqrsl.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/netlib/srot.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/netlib/srotg.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/netlib/sscal.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/netlib/ssvdc.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/netlib/sswap.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/netlib/stgsja.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/netlib/tql1.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/netlib/tql2.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/netlib/trans.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/netlib/tred1.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/netlib/tred2.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/netlib/xerbla.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/netlib/z_abs.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/netlib/z_div.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/netlib/z_sqrt.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/netlib/zaxpy.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/netlib/zcopy.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/netlib/zdotc.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/netlib/zdotu.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/netlib/zdrot.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/netlib/zdscal.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/netlib/zgebak.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/netlib/zgebal.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/netlib/zgeev.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/netlib/zgehd2.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/netlib/zgehrd.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/netlib/zgemm.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/netlib/zgemv.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/netlib/zgerc.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/netlib/zhseqr.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/netlib/zlacgv.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/netlib/zlacpy.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/netlib/zladiv.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/netlib/zlahqr.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/netlib/zlahrd.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/netlib/zlange.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/netlib/zlanhs.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/netlib/zlarf.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/netlib/zlarfb.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/netlib/zlarfg.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/netlib/zlarft.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/netlib/zlarfx.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/netlib/zlascl.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/netlib/zlaset.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/netlib/zlassq.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/netlib/zlatrs.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/netlib/zqrdc.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/netlib/zqrsl.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/netlib/zscal.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/netlib/zsvdc.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/netlib/zswap.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/netlib/ztrevc.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/netlib/ztrmm.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/netlib/ztrmv.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/netlib/ztrsv.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/netlib/zung2r.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/netlib/zunghr.c

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/v3p/netlib/zungqr.c

# End Source File
# End Group
# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project

