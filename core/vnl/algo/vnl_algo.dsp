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
# /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/core" /I "$(VXLROOT)/v3p/netlib" /I "$(VXLROOT)/vcl/config.win32" == include path
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
!MESSAGE "vnl_algo - Win32 Release" (based on "Win32 (x86) Static Library")
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
# ADD BASE CPP /nologo /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /FD /c
# ADD CPP /nologo /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /FD /c
# ADD CPP /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/core" /I "$(VXLROOT)/v3p/netlib" /I "$(VXLROOT)/vcl/config.win32" -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE /D "vnl_algo_EXPORTS"
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
# ADD BASE CPP /nologo /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /FD /c
# ADD CPP /nologo /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /FD /GZ /c
# ADD CPP /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/core" /I "$(VXLROOT)/v3p/netlib" /I "$(VXLROOT)/vcl/config.win32" -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE /D "vnl_algo_EXPORTS"
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

# Name "vnl_algo - Win32 Release"
# Name "vnl_algo - Win32 Debug"

# Begin Group "Source Files"
# PROP Default_Filter ""
# Begin Source File
SOURCE=.\Templates/vcl_vector+vnl_amoeba_SimplexCorner-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vnl_adjugate+double-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vnl_convolve+double.double-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vnl_convolve+int.double-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vnl_convolve+int.int-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vnl_determinant+double-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vnl_determinant+float-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vnl_determinant+vcl_complex+double--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vnl_determinant+vcl_complex+float--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vnl_fft_1d+double-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vnl_fft_1d+float-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vnl_fft_2d+double-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vnl_fft_2d+float-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vnl_fft_base+1.double-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vnl_fft_base+1.float-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vnl_fft_base+2.double-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vnl_fft_base+2.float-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vnl_fft_prime_factors+double-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vnl_fft_prime_factors+float-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vnl_matrix_inverse+double-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vnl_orthogonal_complement+double-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vnl_orthogonal_complement+vcl_complex+double--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vnl_orthogonal_complement+vcl_complex+float--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vnl_qr+double-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vnl_qr+float-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vnl_qr+vcl_complex+double--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vnl_qr+vcl_complex+float--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vnl_scatter_3x3+double-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vnl_scatter_3x3+float-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vnl_svd+double-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vnl_svd+float-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vnl_svd+vcl_complex+double--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vnl_svd+vcl_complex+float--.cxx
# End Source File
# Begin Source File
SOURCE=.\vnl_amoeba.cxx
# End Source File
# Begin Source File
SOURCE=.\vnl_brent.cxx
# End Source File
# Begin Source File
SOURCE=.\vnl_chi_squared.cxx
# End Source File
# Begin Source File
SOURCE=.\vnl_cholesky.cxx
# End Source File
# Begin Source File
SOURCE=.\vnl_complex_eigensystem.cxx
# End Source File
# Begin Source File
SOURCE=.\vnl_conjugate_gradient.cxx
# End Source File
# Begin Source File
SOURCE=.\vnl_cpoly_roots.cxx
# End Source File
# Begin Source File
SOURCE=.\vnl_discrete_diff.cxx
# End Source File
# Begin Source File
SOURCE=.\vnl_fft.cxx
# End Source File
# Begin Source File
SOURCE=.\vnl_gaussian_kernel_1d.cxx
# End Source File
# Begin Source File
SOURCE=.\vnl_generalized_eigensystem.cxx
# End Source File
# Begin Source File
SOURCE=.\vnl_generalized_schur.cxx
# End Source File
# Begin Source File
SOURCE=.\vnl_lbfgs.cxx
# End Source File
# Begin Source File
SOURCE=.\vnl_levenberg_marquardt.cxx
# End Source File
# Begin Source File
SOURCE=.\vnl_lsqr.cxx
# End Source File
# Begin Source File
SOURCE=.\vnl_powell.cxx
# End Source File
# Begin Source File
SOURCE=.\vnl_real_eigensystem.cxx
# End Source File
# Begin Source File
SOURCE=.\vnl_rnpoly_solve.cxx
# End Source File
# Begin Source File
SOURCE=.\vnl_rpoly_roots.cxx
# End Source File
# Begin Source File
SOURCE=.\vnl_sparse_symmetric_eigensystem.cxx
# End Source File
# Begin Source File
SOURCE=.\vnl_symmetric_eigensystem.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vnl_svd_economy+double-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vnl_svd_economy+float-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vnl_svd_economy+vcl_complex+double--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vnl_svd_economy+vcl_complex+float--.cxx
# End Source File
# End Group
# Begin Group "Header Files"
# PROP Default_Filter ""
# Begin Source File
SOURCE=.\vnl_adjugate.h
# End Source File
# Begin Source File
SOURCE=.\vnl_adjugate.txx
# End Source File
# Begin Source File
SOURCE=.\vnl_amoeba.h
# End Source File
# Begin Source File
SOURCE=.\vnl_brent.h
# End Source File
# Begin Source File
SOURCE=.\vnl_chi_squared.h
# End Source File
# Begin Source File
SOURCE=.\vnl_cholesky.h
# End Source File
# Begin Source File
SOURCE=.\vnl_complex_eigensystem.h
# End Source File
# Begin Source File
SOURCE=.\vnl_conjugate_gradient.h
# End Source File
# Begin Source File
SOURCE=.\vnl_convolve.h
# End Source File
# Begin Source File
SOURCE=.\vnl_convolve.txx
# End Source File
# Begin Source File
SOURCE=.\vnl_cpoly_roots.h
# End Source File
# Begin Source File
SOURCE=.\vnl_determinant.h
# End Source File
# Begin Source File
SOURCE=.\vnl_determinant.txx
# End Source File
# Begin Source File
SOURCE=.\vnl_discrete_diff.h
# End Source File
# Begin Source File
SOURCE=.\vnl_fft.h
# End Source File
# Begin Source File
SOURCE=.\vnl_fft_1d.h
# End Source File
# Begin Source File
SOURCE=.\vnl_fft_1d.txx
# End Source File
# Begin Source File
SOURCE=.\vnl_fft_2d.h
# End Source File
# Begin Source File
SOURCE=.\vnl_fft_2d.txx
# End Source File
# Begin Source File
SOURCE=.\vnl_fft_base.h
# End Source File
# Begin Source File
SOURCE=.\vnl_fft_base.txx
# End Source File
# Begin Source File
SOURCE=.\vnl_fft_prime_factors.h
# End Source File
# Begin Source File
SOURCE=.\vnl_fft_prime_factors.txx
# End Source File
# Begin Source File
SOURCE=.\vnl_gaussian_kernel_1d.h
# End Source File
# Begin Source File
SOURCE=.\vnl_generalized_eigensystem.h
# End Source File
# Begin Source File
SOURCE=.\vnl_generalized_schur.h
# End Source File
# Begin Source File
SOURCE=.\vnl_lbfgs.h
# End Source File
# Begin Source File
SOURCE=.\vnl_levenberg_marquardt.h
# End Source File
# Begin Source File
SOURCE=.\vnl_lsqr.h
# End Source File
# Begin Source File
SOURCE=.\vnl_matrix_inverse.h
# End Source File
# Begin Source File
SOURCE=.\vnl_matrix_inverse.txx
# End Source File
# Begin Source File
SOURCE=.\vnl_orthogonal_complement.h
# End Source File
# Begin Source File
SOURCE=.\vnl_orthogonal_complement.txx
# End Source File
# Begin Source File
SOURCE=.\vnl_powell.h
# End Source File
# Begin Source File
SOURCE=.\vnl_qr.h
# End Source File
# Begin Source File
SOURCE=.\vnl_qr.txx
# End Source File
# Begin Source File
SOURCE=.\vnl_real_eigensystem.h
# End Source File
# Begin Source File
SOURCE=.\vnl_rnpoly_solve.h
# End Source File
# Begin Source File
SOURCE=.\vnl_rpoly_roots.h
# End Source File
# Begin Source File
SOURCE=.\vnl_scatter_3x3.h
# End Source File
# Begin Source File
SOURCE=.\vnl_scatter_3x3.txx
# End Source File
# Begin Source File
SOURCE=.\vnl_sparse_symmetric_eigensystem.h
# End Source File
# Begin Source File
SOURCE=.\vnl_svd.h
# End Source File
# Begin Source File
SOURCE=.\vnl_svd.txx
# End Source File
# Begin Source File
SOURCE=.\vnl_symmetric_eigensystem.h
# End Source File
# Begin Source File
SOURCE=.\vnl_algo_fwd.h
# End Source File
# Begin Source File
SOURCE=.\vnl_netlib.h
# End Source File
# Begin Source File
SOURCE=.\vnl_svd_economy.txx
# End Source File
# Begin Source File
SOURCE=.\vnl_svd_economy.h
# End Source File
# End Group
# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
