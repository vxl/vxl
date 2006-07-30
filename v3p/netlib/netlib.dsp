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
# /I "$(VXLROOT)/vcl/config.win32" == include path
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
!MESSAGE "netlib - Win32 Release" (based on "Win32 (x86) Static Library")
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
# ADD BASE CPP /nologo /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /FD /c
# ADD CPP /nologo /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /FD /c
# ADD CPP /I "$(VXLROOT)/vcl/config.win32" -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE /D "netlib_EXPORTS"
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
# ADD BASE CPP /nologo /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /FD /c
# ADD CPP /nologo /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /FD /GZ /c
# ADD CPP /I "$(VXLROOT)/vcl/config.win32" -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE /D "netlib_EXPORTS"
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

# Name "netlib - Win32 Release"
# Name "netlib - Win32 Debug"

# Begin Group "Source Files"
# PROP Default_Filter "c"
# Begin Source File
SOURCE=.\blas/caxpy.c
# End Source File
# Begin Source File
SOURCE=.\blas/ccopy.c
# End Source File
# Begin Source File
SOURCE=.\blas/cdotc.c
# End Source File
# Begin Source File
SOURCE=.\blas/cscal.c
# End Source File
# Begin Source File
SOURCE=.\blas/csrot.c
# End Source File
# Begin Source File
SOURCE=.\blas/cswap.c
# End Source File
# Begin Source File
SOURCE=.\blas/dasum.c
# End Source File
# Begin Source File
SOURCE=.\blas/daxpy.c
# End Source File
# Begin Source File
SOURCE=.\blas/dcabs1.c
# End Source File
# Begin Source File
SOURCE=.\blas/dcopy.c
# End Source File
# Begin Source File
SOURCE=.\blas/ddot.c
# End Source File
# Begin Source File
SOURCE=.\blas/dlamch.c
# End Source File
# Begin Source File
SOURCE=.\blas/dnrm2.c
# End Source File
# Begin Source File
SOURCE=.\blas/drot.c
# End Source File
# Begin Source File
SOURCE=.\blas/drotg.c
# End Source File
# Begin Source File
SOURCE=.\blas/dscal.c
# End Source File
# Begin Source File
SOURCE=.\blas/dswap.c
# End Source File
# Begin Source File
SOURCE=.\blas/dzasum.c
# End Source File
# Begin Source File
SOURCE=.\blas/dznrm2.c
# End Source File
# Begin Source File
SOURCE=.\blas/idamax.c
# End Source File
# Begin Source File
SOURCE=.\blas/izamax.c
# End Source File
# Begin Source File
SOURCE=.\blas/saxpy.c
# End Source File
# Begin Source File
SOURCE=.\blas/scnrm2.c
# End Source File
# Begin Source File
SOURCE=.\blas/scopy.c
# End Source File
# Begin Source File
SOURCE=.\blas/sdot.c
# End Source File
# Begin Source File
SOURCE=.\blas/snrm2.c
# End Source File
# Begin Source File
SOURCE=.\blas/srot.c
# End Source File
# Begin Source File
SOURCE=.\blas/srotg.c
# End Source File
# Begin Source File
SOURCE=.\blas/sscal.c
# End Source File
# Begin Source File
SOURCE=.\blas/sswap.c
# End Source File
# Begin Source File
SOURCE=.\blas/xerbla.c
# End Source File
# Begin Source File
SOURCE=.\blas/zaxpy.c
# End Source File
# Begin Source File
SOURCE=.\blas/zcopy.c
# End Source File
# Begin Source File
SOURCE=.\blas/zdotc.c
# End Source File
# Begin Source File
SOURCE=.\blas/zdotu.c
# End Source File
# Begin Source File
SOURCE=.\blas/zdrot.c
# End Source File
# Begin Source File
SOURCE=.\blas/zdscal.c
# End Source File
# Begin Source File
SOURCE=.\blas/zgemm.c
# End Source File
# Begin Source File
SOURCE=.\blas/zgemv.c
# End Source File
# Begin Source File
SOURCE=.\blas/zgerc.c
# End Source File
# Begin Source File
SOURCE=.\blas/zscal.c
# End Source File
# Begin Source File
SOURCE=.\blas/zswap.c
# End Source File
# Begin Source File
SOURCE=.\blas/ztrmm.c
# End Source File
# Begin Source File
SOURCE=.\blas/ztrmv.c
# End Source File
# Begin Source File
SOURCE=.\blas/ztrsv.c
# End Source File
# Begin Source File
SOURCE=.\eispack/balanc.c
# End Source File
# Begin Source File
SOURCE=.\eispack/balbak.c
# End Source File
# Begin Source File
SOURCE=.\eispack/cdiv.c
# End Source File
# Begin Source File
SOURCE=.\eispack/elmhes.c
# End Source File
# Begin Source File
SOURCE=.\eispack/eltran.c
# End Source File
# Begin Source File
SOURCE=.\eispack/epslon.c
# End Source File
# Begin Source File
SOURCE=.\eispack/hqr.c
# End Source File
# Begin Source File
SOURCE=.\eispack/hqr2.c
# End Source File
# Begin Source File
SOURCE=.\eispack/otqlrat.c
# End Source File
# Begin Source File
SOURCE=.\eispack/pythag.c
# End Source File
# Begin Source File
SOURCE=.\eispack/rebak.c
# End Source File
# Begin Source File
SOURCE=.\eispack/reduc.c
# End Source File
# Begin Source File
SOURCE=.\eispack/rg.c
# End Source File
# Begin Source File
SOURCE=.\eispack/rs.c
# End Source File
# Begin Source File
SOURCE=.\eispack/rsg.c
# End Source File
# Begin Source File
SOURCE=.\eispack/tql1.c
# End Source File
# Begin Source File
SOURCE=.\eispack/tql2.c
# End Source File
# Begin Source File
SOURCE=.\eispack/tred1.c
# End Source File
# Begin Source File
SOURCE=.\eispack/tred2.c
# End Source File
# Begin Source File
SOURCE=.\lapack/complex16/zgebak.c
# End Source File
# Begin Source File
SOURCE=.\lapack/complex16/zgebal.c
# End Source File
# Begin Source File
SOURCE=.\lapack/complex16/zgeev.c
# End Source File
# Begin Source File
SOURCE=.\lapack/complex16/zgehd2.c
# End Source File
# Begin Source File
SOURCE=.\lapack/complex16/zgehrd.c
# End Source File
# Begin Source File
SOURCE=.\lapack/complex16/zhseqr.c
# End Source File
# Begin Source File
SOURCE=.\lapack/complex16/zlacgv.c
# End Source File
# Begin Source File
SOURCE=.\lapack/complex16/zlacpy.c
# End Source File
# Begin Source File
SOURCE=.\lapack/complex16/zladiv.c
# End Source File
# Begin Source File
SOURCE=.\lapack/complex16/zlahqr.c
# End Source File
# Begin Source File
SOURCE=.\lapack/complex16/zlahrd.c
# End Source File
# Begin Source File
SOURCE=.\lapack/complex16/zlange.c
# End Source File
# Begin Source File
SOURCE=.\lapack/complex16/zlanhs.c
# End Source File
# Begin Source File
SOURCE=.\lapack/complex16/zlarf.c
# End Source File
# Begin Source File
SOURCE=.\lapack/complex16/zlarfb.c
# End Source File
# Begin Source File
SOURCE=.\lapack/complex16/zlarfg.c
# End Source File
# Begin Source File
SOURCE=.\lapack/complex16/zlarft.c
# End Source File
# Begin Source File
SOURCE=.\lapack/complex16/zlarfx.c
# End Source File
# Begin Source File
SOURCE=.\lapack/complex16/zlascl.c
# End Source File
# Begin Source File
SOURCE=.\lapack/complex16/zlaset.c
# End Source File
# Begin Source File
SOURCE=.\lapack/complex16/zlassq.c
# End Source File
# Begin Source File
SOURCE=.\lapack/complex16/zlatrs.c
# End Source File
# Begin Source File
SOURCE=.\lapack/complex16/ztrevc.c
# End Source File
# Begin Source File
SOURCE=.\lapack/complex16/zung2r.c
# End Source File
# Begin Source File
SOURCE=.\lapack/complex16/zunghr.c
# End Source File
# Begin Source File
SOURCE=.\lapack/complex16/zungqr.c
# End Source File
# Begin Source File
SOURCE=.\lapack/double/dlabad.c
# End Source File
# Begin Source File
SOURCE=.\lapack/double/dladiv.c
# End Source File
# Begin Source File
SOURCE=.\lapack/double/dlapy2.c
# End Source File
# Begin Source File
SOURCE=.\lapack/double/dlapy3.c
# End Source File
# Begin Source File
SOURCE=.\lapack/util/ieeeck.c
# End Source File
# Begin Source File
SOURCE=.\lapack/util/ilaenv.c
# End Source File
# Begin Source File
SOURCE=.\lapack/util/lsame.c
# End Source File
# Begin Source File
SOURCE=.\laso/dlabax.c
# End Source File
# Begin Source File
SOURCE=.\laso/dlabcm.c
# End Source File
# Begin Source File
SOURCE=.\laso/dlabfc.c
# End Source File
# Begin Source File
SOURCE=.\laso/dlaeig.c
# End Source File
# Begin Source File
SOURCE=.\laso/dlager.c
# End Source File
# Begin Source File
SOURCE=.\laso/dlaran.c
# End Source File
# Begin Source File
SOURCE=.\laso/dmvpc.c
# End Source File
# Begin Source File
SOURCE=.\laso/dnlaso.c
# End Source File
# Begin Source File
SOURCE=.\laso/dnppla.c
# End Source File
# Begin Source File
SOURCE=.\laso/dortqr.c
# End Source File
# Begin Source File
SOURCE=.\laso/dvsort.c
# End Source File
# Begin Source File
SOURCE=.\laso/urand.c
# End Source File
# Begin Source File
SOURCE=.\libf2c/c_abs.c
# End Source File
# Begin Source File
SOURCE=.\libf2c/c_div.c
# End Source File
# Begin Source File
SOURCE=.\libf2c/c_sqrt.c
# End Source File
# Begin Source File
SOURCE=.\libf2c/cabs.c
# End Source File
# Begin Source File
SOURCE=.\libf2c/d_cnjg.c
# End Source File
# Begin Source File
SOURCE=.\libf2c/d_imag.c
# End Source File
# Begin Source File
SOURCE=.\libf2c/d_lg10.c
# End Source File
# Begin Source File
SOURCE=.\libf2c/d_sign.c
# End Source File
# Begin Source File
SOURCE=.\libf2c/exit_.c
# End Source File
# Begin Source File
SOURCE=.\libf2c/f77_aloc.c
# End Source File
# Begin Source File
SOURCE=.\libf2c/i_dnnt.c
# End Source File
# Begin Source File
SOURCE=.\libf2c/pow_dd.c
# End Source File
# Begin Source File
SOURCE=.\libf2c/pow_di.c
# End Source File
# Begin Source File
SOURCE=.\libf2c/pow_ii.c
# End Source File
# Begin Source File
SOURCE=.\libf2c/pow_ri.c
# End Source File
# Begin Source File
SOURCE=.\libf2c/r_cnjg.c
# End Source File
# Begin Source File
SOURCE=.\libf2c/r_imag.c
# End Source File
# Begin Source File
SOURCE=.\libf2c/r_sign.c
# End Source File
# Begin Source File
SOURCE=.\libf2c/s_cat.c
# End Source File
# Begin Source File
SOURCE=.\libf2c/s_cmp.c
# End Source File
# Begin Source File
SOURCE=.\libf2c/s_copy.c
# End Source File
# Begin Source File
SOURCE=.\libf2c/sig_die.c
# End Source File
# Begin Source File
SOURCE=.\libf2c/z_abs.c
# End Source File
# Begin Source File
SOURCE=.\libf2c/z_div.c
# End Source File
# Begin Source File
SOURCE=.\libf2c/z_sqrt.c
# End Source File
# Begin Source File
SOURCE=.\linpack/cqrdc.c
# End Source File
# Begin Source File
SOURCE=.\linpack/cqrsl.c
# End Source File
# Begin Source File
SOURCE=.\linpack/csvdc.c
# End Source File
# Begin Source File
SOURCE=.\linpack/dpoco.c
# End Source File
# Begin Source File
SOURCE=.\linpack/dpodi.c
# End Source File
# Begin Source File
SOURCE=.\linpack/dpofa.c
# End Source File
# Begin Source File
SOURCE=.\linpack/dposl.c
# End Source File
# Begin Source File
SOURCE=.\linpack/dqrdc.c
# End Source File
# Begin Source File
SOURCE=.\linpack/dqrsl.c
# End Source File
# Begin Source File
SOURCE=.\linpack/dsvdc.c
# End Source File
# Begin Source File
SOURCE=.\linpack/sqrdc.c
# End Source File
# Begin Source File
SOURCE=.\linpack/sqrsl.c
# End Source File
# Begin Source File
SOURCE=.\linpack/ssvdc.c
# End Source File
# Begin Source File
SOURCE=.\linpack/zqrdc.c
# End Source File
# Begin Source File
SOURCE=.\linpack/zqrsl.c
# End Source File
# Begin Source File
SOURCE=.\linpack/zsvdc.c
# End Source File
# Begin Source File
SOURCE=.\napack/cg.c
# End Source File
# Begin Source File
SOURCE=.\temperton/dgpfa.c
# End Source File
# Begin Source File
SOURCE=.\temperton/dgpfa2f.c
# End Source File
# Begin Source File
SOURCE=.\temperton/dgpfa3f.c
# End Source File
# Begin Source File
SOURCE=.\temperton/dgpfa5f.c
# End Source File
# Begin Source File
SOURCE=.\temperton/gpfa.c
# End Source File
# Begin Source File
SOURCE=.\temperton/gpfa2f.c
# End Source File
# Begin Source File
SOURCE=.\temperton/gpfa3f.c
# End Source File
# Begin Source File
SOURCE=.\temperton/gpfa5f.c
# End Source File
# Begin Source File
SOURCE=.\temperton/setdgpfa.c
# End Source File
# Begin Source File
SOURCE=.\temperton/setgpfa.c
# End Source File
# Begin Source File
SOURCE=.\minpack/dpmpar.c
# End Source File
# Begin Source File
SOURCE=.\minpack/enorm.c
# End Source File
# Begin Source File
SOURCE=.\minpack/fdjac2.c
# End Source File
# Begin Source File
SOURCE=.\minpack/lmder.c
# End Source File
# Begin Source File
SOURCE=.\minpack/lmder1.c
# End Source File
# Begin Source File
SOURCE=.\minpack/lmdif.c
# End Source File
# Begin Source File
SOURCE=.\minpack/lmpar.c
# End Source File
# Begin Source File
SOURCE=.\minpack/qrfac.c
# End Source File
# Begin Source File
SOURCE=.\minpack/qrsolv.c
# End Source File
# Begin Source File
SOURCE=.\opt/lbfgs.c
# End Source File
# Begin Source File
SOURCE=.\blas/dgemm.c
# End Source File
# Begin Source File
SOURCE=.\blas/dgemv.c
# End Source File
# Begin Source File
SOURCE=.\blas/dger.c
# End Source File
# Begin Source File
SOURCE=.\blas/dtrmm.c
# End Source File
# Begin Source File
SOURCE=.\blas/dtrmv.c
# End Source File
# Begin Source File
SOURCE=.\blas/dtrsv.c
# End Source File
# Begin Source File
SOURCE=.\blas/isamax.c
# End Source File
# Begin Source File
SOURCE=.\blas/sgemv.c
# End Source File
# Begin Source File
SOURCE=.\blas/sger.c
# End Source File
# Begin Source File
SOURCE=.\blas/slamch.c
# End Source File
# Begin Source File
SOURCE=.\datapac/camsun.c
# End Source File
# Begin Source File
SOURCE=.\lapack/double/dgecon.c
# End Source File
# Begin Source File
SOURCE=.\lapack/double/dgeqr2.c
# End Source File
# Begin Source File
SOURCE=.\lapack/double/dgeqrf.c
# End Source File
# Begin Source File
SOURCE=.\lapack/double/dgerq2.c
# End Source File
# Begin Source File
SOURCE=.\lapack/double/dgesc2.c
# End Source File
# Begin Source File
SOURCE=.\lapack/double/dgetc2.c
# End Source File
# Begin Source File
SOURCE=.\lapack/double/dggbak.c
# End Source File
# Begin Source File
SOURCE=.\lapack/double/dggbal.c
# End Source File
# Begin Source File
SOURCE=.\lapack/double/dgges.c
# End Source File
# Begin Source File
SOURCE=.\lapack/double/dgghrd.c
# End Source File
# Begin Source File
SOURCE=.\lapack/double/dhgeqz.c
# End Source File
# Begin Source File
SOURCE=.\lapack/double/dlacon.c
# End Source File
# Begin Source File
SOURCE=.\lapack/double/dlacpy.c
# End Source File
# Begin Source File
SOURCE=.\lapack/double/dlag2.c
# End Source File
# Begin Source File
SOURCE=.\lapack/double/dlagv2.c
# End Source File
# Begin Source File
SOURCE=.\lapack/double/dlange.c
# End Source File
# Begin Source File
SOURCE=.\lapack/double/dlanhs.c
# End Source File
# Begin Source File
SOURCE=.\lapack/double/dlarf.c
# End Source File
# Begin Source File
SOURCE=.\lapack/double/dlarfb.c
# End Source File
# Begin Source File
SOURCE=.\lapack/double/dlarfg.c
# End Source File
# Begin Source File
SOURCE=.\lapack/double/dlarft.c
# End Source File
# Begin Source File
SOURCE=.\lapack/double/dlartg.c
# End Source File
# Begin Source File
SOURCE=.\lapack/double/dlascl.c
# End Source File
# Begin Source File
SOURCE=.\lapack/double/dlaset.c
# End Source File
# Begin Source File
SOURCE=.\lapack/double/dlassq.c
# End Source File
# Begin Source File
SOURCE=.\lapack/double/dlasv2.c
# End Source File
# Begin Source File
SOURCE=.\lapack/double/dlaswp.c
# End Source File
# Begin Source File
SOURCE=.\lapack/double/dlatdf.c
# End Source File
# Begin Source File
SOURCE=.\lapack/double/dlatrs.c
# End Source File
# Begin Source File
SOURCE=.\lapack/double/dorg2r.c
# End Source File
# Begin Source File
SOURCE=.\lapack/double/dorgqr.c
# End Source File
# Begin Source File
SOURCE=.\lapack/double/dorgr2.c
# End Source File
# Begin Source File
SOURCE=.\lapack/double/dorm2r.c
# End Source File
# Begin Source File
SOURCE=.\lapack/double/dormqr.c
# End Source File
# Begin Source File
SOURCE=.\lapack/double/dormr2.c
# End Source File
# Begin Source File
SOURCE=.\lapack/double/drscl.c
# End Source File
# Begin Source File
SOURCE=.\lapack/double/dtgex2.c
# End Source File
# Begin Source File
SOURCE=.\lapack/double/dtgexc.c
# End Source File
# Begin Source File
SOURCE=.\lapack/double/dtgsen.c
# End Source File
# Begin Source File
SOURCE=.\lapack/double/dtgsy2.c
# End Source File
# Begin Source File
SOURCE=.\lapack/double/dtgsyl.c
# End Source File
# Begin Source File
SOURCE=.\lapack/double/dlaev2.c
# End Source File
# Begin Source File
SOURCE=.\lapack/double/dspr.c
# End Source File
# Begin Source File
SOURCE=.\lapack/double/dsptrf.c
# End Source File
# Begin Source File
SOURCE=.\lapack/double/dsptrs.c
# End Source File
# Begin Source File
SOURCE=.\lapack/single/sgeqr2.c
# End Source File
# Begin Source File
SOURCE=.\lapack/single/sgerq2.c
# End Source File
# Begin Source File
SOURCE=.\lapack/single/sggsvd.c
# End Source File
# Begin Source File
SOURCE=.\lapack/single/sggsvp.c
# End Source File
# Begin Source File
SOURCE=.\lapack/single/slacpy.c
# End Source File
# Begin Source File
SOURCE=.\lapack/single/slags2.c
# End Source File
# Begin Source File
SOURCE=.\lapack/single/slange.c
# End Source File
# Begin Source File
SOURCE=.\lapack/single/slapll.c
# End Source File
# Begin Source File
SOURCE=.\lapack/single/slapmt.c
# End Source File
# Begin Source File
SOURCE=.\lapack/single/slapy2.c
# End Source File
# Begin Source File
SOURCE=.\lapack/single/slarf.c
# End Source File
# Begin Source File
SOURCE=.\lapack/single/slarfg.c
# End Source File
# Begin Source File
SOURCE=.\lapack/single/slartg.c
# End Source File
# Begin Source File
SOURCE=.\lapack/single/slas2.c
# End Source File
# Begin Source File
SOURCE=.\lapack/single/slaset.c
# End Source File
# Begin Source File
SOURCE=.\lapack/single/slassq.c
# End Source File
# Begin Source File
SOURCE=.\lapack/single/slasv2.c
# End Source File
# Begin Source File
SOURCE=.\lapack/single/sorg2r.c
# End Source File
# Begin Source File
SOURCE=.\lapack/single/sorm2r.c
# End Source File
# Begin Source File
SOURCE=.\lapack/single/sormr2.c
# End Source File
# Begin Source File
SOURCE=.\lapack/single/stgsja.c
# End Source File
# Begin Source File
SOURCE=.\linalg/lsqr.c
# End Source File
# Begin Source File
SOURCE=.\mathews/adaquad.c
# End Source File
# Begin Source File
SOURCE=.\mathews/simpson.c
# End Source File
# Begin Source File
SOURCE=.\mathews/trapezod.c
# End Source File
# Begin Source File
SOURCE=.\toms/rpoly.c
# End Source File
# Begin Source File
SOURCE=.\triangle.c
# End Source File
# Begin Source File
SOURCE=.\lapack/single/sgeqpf.c
# End Source File
# Begin Source File
SOURCE=.\v3p_netlib_init.c
# End Source File
# End Group
# Begin Group "Header Files"
# PROP Default_Filter "h"
# Begin Source File
SOURCE=.\v3p_f2c.h
# End Source File
# Begin Source File
SOURCE=.\triangle.h
# End Source File
# Begin Source File
SOURCE=.\f2c.h
# End Source File
# Begin Source File
SOURCE=.\v3p_f2c_mangle.h
# End Source File
# Begin Source File
SOURCE=.\v3p_f2c_original.h
# End Source File
# Begin Source File
SOURCE=.\v3p_f2c_unmangle.h
# End Source File
# Begin Source File
SOURCE=.\v3p_netlib.h
# End Source File
# Begin Source File
SOURCE=.\v3p_netlib_mangle.h
# End Source File
# Begin Source File
SOURCE=.\v3p_netlib_prototypes.h
# End Source File
# Begin Source File
SOURCE=.\v3p_netlib_unmangle.h
# End Source File
# Begin Source File
SOURCE=.\blas/caxpy.h
# End Source File
# Begin Source File
SOURCE=.\blas/ccopy.h
# End Source File
# Begin Source File
SOURCE=.\blas/cdotc.h
# End Source File
# Begin Source File
SOURCE=.\blas/cscal.h
# End Source File
# Begin Source File
SOURCE=.\blas/csrot.h
# End Source File
# Begin Source File
SOURCE=.\blas/cswap.h
# End Source File
# Begin Source File
SOURCE=.\blas/dasum.h
# End Source File
# Begin Source File
SOURCE=.\blas/daxpy.h
# End Source File
# Begin Source File
SOURCE=.\blas/dcabs1.h
# End Source File
# Begin Source File
SOURCE=.\blas/dcopy.h
# End Source File
# Begin Source File
SOURCE=.\blas/ddot.h
# End Source File
# Begin Source File
SOURCE=.\blas/dlamch.h
# End Source File
# Begin Source File
SOURCE=.\blas/dnrm2.h
# End Source File
# Begin Source File
SOURCE=.\blas/drot.h
# End Source File
# Begin Source File
SOURCE=.\blas/drotg.h
# End Source File
# Begin Source File
SOURCE=.\blas/dscal.h
# End Source File
# Begin Source File
SOURCE=.\blas/dswap.h
# End Source File
# Begin Source File
SOURCE=.\blas/dzasum.h
# End Source File
# Begin Source File
SOURCE=.\blas/dznrm2.h
# End Source File
# Begin Source File
SOURCE=.\blas/idamax.h
# End Source File
# Begin Source File
SOURCE=.\blas/izamax.h
# End Source File
# Begin Source File
SOURCE=.\blas/saxpy.h
# End Source File
# Begin Source File
SOURCE=.\blas/scnrm2.h
# End Source File
# Begin Source File
SOURCE=.\blas/scopy.h
# End Source File
# Begin Source File
SOURCE=.\blas/sdot.h
# End Source File
# Begin Source File
SOURCE=.\blas/snrm2.h
# End Source File
# Begin Source File
SOURCE=.\blas/srot.h
# End Source File
# Begin Source File
SOURCE=.\blas/srotg.h
# End Source File
# Begin Source File
SOURCE=.\blas/sscal.h
# End Source File
# Begin Source File
SOURCE=.\blas/sswap.h
# End Source File
# Begin Source File
SOURCE=.\blas/xerbla.h
# End Source File
# Begin Source File
SOURCE=.\blas/zaxpy.h
# End Source File
# Begin Source File
SOURCE=.\blas/zcopy.h
# End Source File
# Begin Source File
SOURCE=.\blas/zdotc.h
# End Source File
# Begin Source File
SOURCE=.\blas/zdotu.h
# End Source File
# Begin Source File
SOURCE=.\blas/zdrot.h
# End Source File
# Begin Source File
SOURCE=.\blas/zdscal.h
# End Source File
# Begin Source File
SOURCE=.\blas/zgemm.h
# End Source File
# Begin Source File
SOURCE=.\blas/zgemv.h
# End Source File
# Begin Source File
SOURCE=.\blas/zgerc.h
# End Source File
# Begin Source File
SOURCE=.\blas/zscal.h
# End Source File
# Begin Source File
SOURCE=.\blas/zswap.h
# End Source File
# Begin Source File
SOURCE=.\blas/ztrmm.h
# End Source File
# Begin Source File
SOURCE=.\blas/ztrmv.h
# End Source File
# Begin Source File
SOURCE=.\blas/ztrsv.h
# End Source File
# Begin Source File
SOURCE=.\eispack/balanc.h
# End Source File
# Begin Source File
SOURCE=.\eispack/balbak.h
# End Source File
# Begin Source File
SOURCE=.\eispack/cdiv.h
# End Source File
# Begin Source File
SOURCE=.\eispack/elmhes.h
# End Source File
# Begin Source File
SOURCE=.\eispack/eltran.h
# End Source File
# Begin Source File
SOURCE=.\eispack/epslon.h
# End Source File
# Begin Source File
SOURCE=.\eispack/hqr.h
# End Source File
# Begin Source File
SOURCE=.\eispack/hqr2.h
# End Source File
# Begin Source File
SOURCE=.\eispack/otqlrat.h
# End Source File
# Begin Source File
SOURCE=.\eispack/pythag.h
# End Source File
# Begin Source File
SOURCE=.\eispack/rebak.h
# End Source File
# Begin Source File
SOURCE=.\eispack/reduc.h
# End Source File
# Begin Source File
SOURCE=.\eispack/rg.h
# End Source File
# Begin Source File
SOURCE=.\eispack/rs.h
# End Source File
# Begin Source File
SOURCE=.\eispack/rsg.h
# End Source File
# Begin Source File
SOURCE=.\eispack/tql1.h
# End Source File
# Begin Source File
SOURCE=.\eispack/tql2.h
# End Source File
# Begin Source File
SOURCE=.\eispack/tred1.h
# End Source File
# Begin Source File
SOURCE=.\eispack/tred2.h
# End Source File
# Begin Source File
SOURCE=.\lapack/complex16/zgebak.h
# End Source File
# Begin Source File
SOURCE=.\lapack/complex16/zgebal.h
# End Source File
# Begin Source File
SOURCE=.\lapack/complex16/zgeev.h
# End Source File
# Begin Source File
SOURCE=.\lapack/complex16/zgehd2.h
# End Source File
# Begin Source File
SOURCE=.\lapack/complex16/zgehrd.h
# End Source File
# Begin Source File
SOURCE=.\lapack/complex16/zhseqr.h
# End Source File
# Begin Source File
SOURCE=.\lapack/complex16/zlacgv.h
# End Source File
# Begin Source File
SOURCE=.\lapack/complex16/zlacpy.h
# End Source File
# Begin Source File
SOURCE=.\lapack/complex16/zladiv.h
# End Source File
# Begin Source File
SOURCE=.\lapack/complex16/zlahqr.h
# End Source File
# Begin Source File
SOURCE=.\lapack/complex16/zlahrd.h
# End Source File
# Begin Source File
SOURCE=.\lapack/complex16/zlange.h
# End Source File
# Begin Source File
SOURCE=.\lapack/complex16/zlanhs.h
# End Source File
# Begin Source File
SOURCE=.\lapack/complex16/zlarf.h
# End Source File
# Begin Source File
SOURCE=.\lapack/complex16/zlarfb.h
# End Source File
# Begin Source File
SOURCE=.\lapack/complex16/zlarfg.h
# End Source File
# Begin Source File
SOURCE=.\lapack/complex16/zlarft.h
# End Source File
# Begin Source File
SOURCE=.\lapack/complex16/zlarfx.h
# End Source File
# Begin Source File
SOURCE=.\lapack/complex16/zlascl.h
# End Source File
# Begin Source File
SOURCE=.\lapack/complex16/zlaset.h
# End Source File
# Begin Source File
SOURCE=.\lapack/complex16/zlassq.h
# End Source File
# Begin Source File
SOURCE=.\lapack/complex16/zlatrs.h
# End Source File
# Begin Source File
SOURCE=.\lapack/complex16/ztrevc.h
# End Source File
# Begin Source File
SOURCE=.\lapack/complex16/zung2r.h
# End Source File
# Begin Source File
SOURCE=.\lapack/complex16/zunghr.h
# End Source File
# Begin Source File
SOURCE=.\lapack/complex16/zungqr.h
# End Source File
# Begin Source File
SOURCE=.\lapack/double/dlabad.h
# End Source File
# Begin Source File
SOURCE=.\lapack/double/dladiv.h
# End Source File
# Begin Source File
SOURCE=.\lapack/double/dlapy2.h
# End Source File
# Begin Source File
SOURCE=.\lapack/double/dlapy3.h
# End Source File
# Begin Source File
SOURCE=.\lapack/util/ieeeck.h
# End Source File
# Begin Source File
SOURCE=.\lapack/util/ilaenv.h
# End Source File
# Begin Source File
SOURCE=.\lapack/util/lsame.h
# End Source File
# Begin Source File
SOURCE=.\laso/dlabax.h
# End Source File
# Begin Source File
SOURCE=.\laso/dlabcm.h
# End Source File
# Begin Source File
SOURCE=.\laso/dlabfc.h
# End Source File
# Begin Source File
SOURCE=.\laso/dlaeig.h
# End Source File
# Begin Source File
SOURCE=.\laso/dlager.h
# End Source File
# Begin Source File
SOURCE=.\laso/dlaran.h
# End Source File
# Begin Source File
SOURCE=.\laso/dmvpc.h
# End Source File
# Begin Source File
SOURCE=.\laso/dnlaso.h
# End Source File
# Begin Source File
SOURCE=.\laso/dnppla.h
# End Source File
# Begin Source File
SOURCE=.\laso/dortqr.h
# End Source File
# Begin Source File
SOURCE=.\laso/dvsort.h
# End Source File
# Begin Source File
SOURCE=.\laso/urand.h
# End Source File
# Begin Source File
SOURCE=.\linpack/cqrdc.h
# End Source File
# Begin Source File
SOURCE=.\linpack/cqrsl.h
# End Source File
# Begin Source File
SOURCE=.\linpack/csvdc.h
# End Source File
# Begin Source File
SOURCE=.\linpack/dpoco.h
# End Source File
# Begin Source File
SOURCE=.\linpack/dpodi.h
# End Source File
# Begin Source File
SOURCE=.\linpack/dpofa.h
# End Source File
# Begin Source File
SOURCE=.\linpack/dposl.h
# End Source File
# Begin Source File
SOURCE=.\linpack/dqrdc.h
# End Source File
# Begin Source File
SOURCE=.\linpack/dqrsl.h
# End Source File
# Begin Source File
SOURCE=.\linpack/dsvdc.h
# End Source File
# Begin Source File
SOURCE=.\linpack/sqrdc.h
# End Source File
# Begin Source File
SOURCE=.\linpack/sqrsl.h
# End Source File
# Begin Source File
SOURCE=.\linpack/ssvdc.h
# End Source File
# Begin Source File
SOURCE=.\linpack/zqrdc.h
# End Source File
# Begin Source File
SOURCE=.\linpack/zqrsl.h
# End Source File
# Begin Source File
SOURCE=.\linpack/zsvdc.h
# End Source File
# Begin Source File
SOURCE=.\napack/cg.h
# End Source File
# Begin Source File
SOURCE=.\temperton/dgpfa.h
# End Source File
# Begin Source File
SOURCE=.\temperton/dgpfa2f.h
# End Source File
# Begin Source File
SOURCE=.\temperton/dgpfa3f.h
# End Source File
# Begin Source File
SOURCE=.\temperton/dgpfa5f.h
# End Source File
# Begin Source File
SOURCE=.\temperton/gpfa.h
# End Source File
# Begin Source File
SOURCE=.\temperton/gpfa2f.h
# End Source File
# Begin Source File
SOURCE=.\temperton/gpfa3f.h
# End Source File
# Begin Source File
SOURCE=.\temperton/gpfa5f.h
# End Source File
# Begin Source File
SOURCE=.\temperton/setdgpfa.h
# End Source File
# Begin Source File
SOURCE=.\temperton/setgpfa.h
# End Source File
# Begin Source File
SOURCE=.\minpack/dpmpar.h
# End Source File
# Begin Source File
SOURCE=.\minpack/enorm.h
# End Source File
# Begin Source File
SOURCE=.\minpack/fdjac2.h
# End Source File
# Begin Source File
SOURCE=.\minpack/lmder.h
# End Source File
# Begin Source File
SOURCE=.\minpack/lmder1.h
# End Source File
# Begin Source File
SOURCE=.\minpack/lmdif.h
# End Source File
# Begin Source File
SOURCE=.\minpack/lmpar.h
# End Source File
# Begin Source File
SOURCE=.\minpack/qrfac.h
# End Source File
# Begin Source File
SOURCE=.\minpack/qrsolv.h
# End Source File
# Begin Source File
SOURCE=.\linalg/lsqr.h
# End Source File
# Begin Source File
SOURCE=.\opt/lbfgs.h
# End Source File
# Begin Source File
SOURCE=.\blas/dgemm.h
# End Source File
# Begin Source File
SOURCE=.\blas/dgemv.h
# End Source File
# Begin Source File
SOURCE=.\blas/dger.h
# End Source File
# Begin Source File
SOURCE=.\blas/dtrmm.h
# End Source File
# Begin Source File
SOURCE=.\blas/dtrmv.h
# End Source File
# Begin Source File
SOURCE=.\blas/dtrsv.h
# End Source File
# Begin Source File
SOURCE=.\blas/isamax.h
# End Source File
# Begin Source File
SOURCE=.\blas/sgemv.h
# End Source File
# Begin Source File
SOURCE=.\blas/sger.h
# End Source File
# Begin Source File
SOURCE=.\blas/slamch.h
# End Source File
# Begin Source File
SOURCE=.\datapac/camsun.h
# End Source File
# End Group
# End Target
# End Project
