# Microsoft Developer Studio Project File - Name="vnl_algo" - Package Owner=<4>
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
# vnl_algo  == name of output library

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=vnl_algo - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "vnl_algo.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "vnl_algo.mak" CFG="vnl_algo - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "vnl_algo - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "vnl_algo - Win32 MinSizeRel" (based on "Win32 (x86) Static Library")
!MESSAGE "vnl_algo - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "vnl_algo - Win32 RelWithDebInfo" (based on "Win32 (x86) Static Library")
# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "vnl_algo - Win32 Release"

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
# ADD CPP /I "c:/awf/src/vxl/vcl" /I "c:/awf/src/vxl/vxl" /I "c:/awf/src/vxl/vcl" /I "c:/awf/src/vxl/vcl/config.win32-VC60" /I "c:/awf/src/vxl/vxl"    -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE /D "vnl_algo_EXPORTS"
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

!ELSEIF  "$(CFG)" == "vnl_algo - Win32 Debug"

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
# ADD CPP /I "c:/awf/src/vxl/vcl" /I "c:/awf/src/vxl/vxl" /I "c:/awf/src/vxl/vcl" /I "c:/awf/src/vxl/vcl/config.win32-VC60" /I "c:/awf/src/vxl/vxl"    -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE /D "vnl_algo_EXPORTS"
# ADD CPP /W3 /Zm1000 /GX /GR
# ADD CPP /MDd /Zi /Od /GZ
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
!ELSEIF  "$(CFG)" == "vnl_algo - Win32 MinSizeRel"

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
# ADD CPP /I "c:/awf/src/vxl/vcl" /I "c:/awf/src/vxl/vxl" /I "c:/awf/src/vxl/vcl" /I "c:/awf/src/vxl/vcl/config.win32-VC60" /I "c:/awf/src/vxl/vxl"    -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE /D "vnl_algo_EXPORTS"
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

!ELSEIF  "$(CFG)" == "vnl_algo - Win32 RelWithDebInfo"

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
# ADD CPP /I "c:/awf/src/vxl/vcl" /I "c:/awf/src/vxl/vxl" /I "c:/awf/src/vxl/vcl" /I "c:/awf/src/vxl/vcl/config.win32-VC60" /I "c:/awf/src/vxl/vxl"    -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE /D "vnl_algo_EXPORTS"
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

# Name "vnl_algo - Win32 Release"
# Name "vnl_algo - Win32 Debug"
# Name "vnl_algo - Win32 MinSizeRel"
# Name "vnl_algo - Win32 RelWithDebInfo"


# Begin Source File

SOURCE="c:/awf/src/vxl/vxl/vnl/algo/CMakeLists.txt"

!IF  "$(CFG)" == "vnl_algo - Win32 Release"
USERDEP__HACK=\
	"c:/awf/src/vxl/CMakeLists.txt"\
	"c:/awf/src/vxl/vxl/CMakeLists.txt"\
	"c:/awf/src/vxl/vxl/vnl/CMakeLists.txt"\
	"c:/awf/src/vxl/vxl/vnl/algo/CMakeLists.txt"
# Begin Custom Build

"vnl_algo.dsp" :  "$(SOURCE)" "$(INTDIR)" "$(OUTDIR)"
	"c:/awf/src/CMake/Source/cmake.exe" "c:/awf/src/vxl/vxl/vnl/algo/CMakeLists.txt" -DSP -H"c:/awf/src/vxl" -S"c:/awf/src/vxl/vxl/vnl/algo" -O"c:/awf/src/vxl/vxl/vnl/algo" -B"c:/awf/src/vxl"

# End Custom Build

!ELSEIF  "$(CFG)" == "vnl_algo - Win32 Debug"
USERDEP__HACK=\
	"c:/awf/src/vxl/CMakeLists.txt"\
	"c:/awf/src/vxl/vxl/CMakeLists.txt"\
	"c:/awf/src/vxl/vxl/vnl/CMakeLists.txt"\
	"c:/awf/src/vxl/vxl/vnl/algo/CMakeLists.txt"
# Begin Custom Build

"vnl_algo.dsp" :  "$(SOURCE)" "$(INTDIR)" "$(OUTDIR)"
	"c:/awf/src/CMake/Source/cmake.exe" "c:/awf/src/vxl/vxl/vnl/algo/CMakeLists.txt" -DSP -H"c:/awf/src/vxl" -S"c:/awf/src/vxl/vxl/vnl/algo" -O"c:/awf/src/vxl/vxl/vnl/algo" -B"c:/awf/src/vxl"

# End Custom Build

!ELSEIF  "$(CFG)" == "vnl_algo - Win32 MinSizeRel"
USERDEP__HACK=\
	"c:/awf/src/vxl/CMakeLists.txt"\
	"c:/awf/src/vxl/vxl/CMakeLists.txt"\
	"c:/awf/src/vxl/vxl/vnl/CMakeLists.txt"\
	"c:/awf/src/vxl/vxl/vnl/algo/CMakeLists.txt"
# Begin Custom Build

"vnl_algo.dsp" :  "$(SOURCE)" "$(INTDIR)" "$(OUTDIR)"
	"c:/awf/src/CMake/Source/cmake.exe" "c:/awf/src/vxl/vxl/vnl/algo/CMakeLists.txt" -DSP -H"c:/awf/src/vxl" -S"c:/awf/src/vxl/vxl/vnl/algo" -O"c:/awf/src/vxl/vxl/vnl/algo" -B"c:/awf/src/vxl"

# End Custom Build

!ELSEIF  "$(CFG)" == "vnl_algo - Win32 RelWithDebInfo"
USERDEP__HACK=\
	"c:/awf/src/vxl/CMakeLists.txt"\
	"c:/awf/src/vxl/vxl/CMakeLists.txt"\
	"c:/awf/src/vxl/vxl/vnl/CMakeLists.txt"\
	"c:/awf/src/vxl/vxl/vnl/algo/CMakeLists.txt"
# Begin Custom Build

"vnl_algo.dsp" :  "$(SOURCE)" "$(INTDIR)" "$(OUTDIR)"
	"c:/awf/src/CMake/Source/cmake.exe" "c:/awf/src/vxl/vxl/vnl/algo/CMakeLists.txt" -DSP -H"c:/awf/src/vxl" -S"c:/awf/src/vxl/vxl/vnl/algo" -O"c:/awf/src/vxl/vxl/vnl/algo" -B"c:/awf/src/vxl"

# End Custom Build

!ENDIF

# End Source File
# Begin Group "Source Files"
# PROP Default_Filter ""
# Begin Source File

SOURCE=c:/awf/src/vxl/vxl/vnl/algo/Templates/fsm_qr+double-.cxx

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/vxl/vnl/algo/Templates/fsm_qr+float-.cxx

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/vxl/vnl/algo/Templates/fsm_qr+long_double-.cxx

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/vxl/vnl/algo/Templates/fsm_qr+vcl_complex+double--.cxx

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/vxl/vnl/algo/Templates/fsm_qr+vcl_complex+float--.cxx

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/vxl/vnl/algo/Templates/fsm_qr+vcl_complex+long_double--.cxx

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/vxl/vnl/algo/Templates/fsm_svdc+double-.cxx

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/vxl/vnl/algo/Templates/fsm_svdc+float-.cxx

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/vxl/vnl/algo/Templates/fsm_svdc+long_double-.cxx

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/vxl/vnl/algo/Templates/fsm_svdc+vcl_complex+double--.cxx

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/vxl/vnl/algo/Templates/fsm_svdc+vcl_complex+float--.cxx

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/vxl/vnl/algo/Templates/fsm_svdc+vcl_complex+long_double--.cxx

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/vxl/vnl/algo/Templates/vcl_vector+vnl_amoeba_SimplexCorner-.cxx

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/vxl/vnl/algo/Templates/vnl_adjugate+double-.cxx

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/vxl/vnl/algo/Templates/vnl_convolve+double.double-.cxx

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/vxl/vnl/algo/Templates/vnl_convolve+int.double-.cxx

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/vxl/vnl/algo/Templates/vnl_convolve+int.int-.cxx

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/vxl/vnl/algo/Templates/vnl_determinant+double-.cxx

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/vxl/vnl/algo/Templates/vnl_determinant+float-.cxx

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/vxl/vnl/algo/Templates/vnl_determinant+long_double-.cxx

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/vxl/vnl/algo/Templates/vnl_determinant+long_double_complex-.cxx

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/vxl/vnl/algo/Templates/vnl_determinant+vcl_complex+double--.cxx

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/vxl/vnl/algo/Templates/vnl_determinant+vcl_complex+float--.cxx

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/vxl/vnl/algo/Templates/vnl_fft1d+double-.cxx

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/vxl/vnl/algo/Templates/vnl_fft1d+float-.cxx

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/vxl/vnl/algo/Templates/vnl_fft2d+double-.cxx

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/vxl/vnl/algo/Templates/vnl_fft2d+float-.cxx

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/vxl/vnl/algo/Templates/vnl_fft_1d+double-.cxx

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/vxl/vnl/algo/Templates/vnl_fft_1d+float-.cxx

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/vxl/vnl/algo/Templates/vnl_fft_2d+double-.cxx

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/vxl/vnl/algo/Templates/vnl_fft_2d+float-.cxx

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/vxl/vnl/algo/Templates/vnl_fft_base+1.double-.cxx

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/vxl/vnl/algo/Templates/vnl_fft_base+1.float-.cxx

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/vxl/vnl/algo/Templates/vnl_fft_base+2.double-.cxx

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/vxl/vnl/algo/Templates/vnl_fft_base+2.float-.cxx

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/vxl/vnl/algo/Templates/vnl_fft_prime_factors+double-.cxx

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/vxl/vnl/algo/Templates/vnl_fft_prime_factors+float-.cxx

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/vxl/vnl/algo/Templates/vnl_matrix_inverse+double-.cxx

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/vxl/vnl/algo/Templates/vnl_orthogonal_complement+double-.cxx

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/vxl/vnl/algo/Templates/vnl_orthogonal_complement+vcl_complex+double--.cxx

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/vxl/vnl/algo/Templates/vnl_orthogonal_complement+vcl_complex+float--.cxx

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/vxl/vnl/algo/Templates/vnl_orthogonal_complement+vcl_complex+long_double--.cxx

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/vxl/vnl/algo/Templates/vnl_qr+double-.cxx

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/vxl/vnl/algo/Templates/vnl_qr+float-.cxx

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/vxl/vnl/algo/Templates/vnl_qr+long_double-.cxx

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/vxl/vnl/algo/Templates/vnl_qr+vcl_complex+double--.cxx

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/vxl/vnl/algo/Templates/vnl_qr+vcl_complex+float--.cxx

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/vxl/vnl/algo/Templates/vnl_qr+vcl_complex+long_double--.cxx

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/vxl/vnl/algo/Templates/vnl_scatter_3x3+double-.cxx

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/vxl/vnl/algo/Templates/vnl_scatter_3x3+float-.cxx

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/vxl/vnl/algo/Templates/vnl_svd+double-.cxx

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/vxl/vnl/algo/Templates/vnl_svd+float-.cxx

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/vxl/vnl/algo/Templates/vnl_svd+long_double-.cxx

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/vxl/vnl/algo/Templates/vnl_svd+vcl_complex+double--.cxx

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/vxl/vnl/algo/Templates/vnl_svd+vcl_complex+float--.cxx

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/vxl/vnl/algo/Templates/vnl_svd+vcl_complex+long_double--.cxx

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/vxl/vnl/algo/vnl_amoeba.cxx

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/vxl/vnl/algo/vnl_brent.cxx

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/vxl/vnl/algo/vnl_chi_squared.cxx

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/vxl/vnl/algo/vnl_cholesky.cxx

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/vxl/vnl/algo/vnl_complex_eigensystem.cxx

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/vxl/vnl/algo/vnl_conjugate_gradient.cxx

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/vxl/vnl/algo/vnl_cpoly_roots.cxx

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/vxl/vnl/algo/vnl_discrete_diff.cxx

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/vxl/vnl/algo/vnl_fft.cxx

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/vxl/vnl/algo/vnl_fftxd_prime_factors.cxx

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/vxl/vnl/algo/vnl_gaussian_kernel_1d.cxx

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/vxl/vnl/algo/vnl_generalized_eigensystem.cxx

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/vxl/vnl/algo/vnl_lbfgs.cxx

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/vxl/vnl/algo/vnl_levenberg_marquardt.cxx

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/vxl/vnl/algo/vnl_lsqr.cxx

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/vxl/vnl/algo/vnl_powell.cxx

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/vxl/vnl/algo/vnl_real_eigensystem.cxx

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/vxl/vnl/algo/vnl_rnpoly_solve.cxx

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/vxl/vnl/algo/vnl_rpoly_roots.cxx

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/vxl/vnl/algo/vnl_sparse_symmetric_eigensystem.cxx

# End Source File
# Begin Source File

SOURCE=c:/awf/src/vxl/vxl/vnl/algo/vnl_symmetric_eigensystem.cxx

# End Source File
# End Group
# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project

