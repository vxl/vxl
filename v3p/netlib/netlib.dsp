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
SOURCE=.\F77_aloc.c
# End Source File
# Begin Source File
SOURCE=.\adaquad.c
# End Source File
# Begin Source File
SOURCE=.\c_abs.c
# End Source File
# Begin Source File
SOURCE=.\c_div.c
# End Source File
# Begin Source File
SOURCE=.\cabs.c
# End Source File
# Begin Source File
SOURCE=.\camsun.c
# End Source File
# Begin Source File
SOURCE=.\caxpy.c
# End Source File
# Begin Source File
SOURCE=.\ccopy.c
# End Source File
# Begin Source File
SOURCE=.\cdotc.c
# End Source File
# Begin Source File
SOURCE=.\cscal.c
# End Source File
# Begin Source File
SOURCE=.\csrot.c
# End Source File
# Begin Source File
SOURCE=.\cswap.c
# End Source File
# Begin Source File
SOURCE=.\d_cnjg.c
# End Source File
# Begin Source File
SOURCE=.\d_imag.c
# End Source File
# Begin Source File
SOURCE=.\d_lg10.c
# End Source File
# Begin Source File
SOURCE=.\d_sign.c
# End Source File
# Begin Source File
SOURCE=.\dasum.c
# End Source File
# Begin Source File
SOURCE=.\daxpy.c
# End Source File
# Begin Source File
SOURCE=.\dcopy.c
# End Source File
# Begin Source File
SOURCE=.\ddot.c
# End Source File
# Begin Source File
SOURCE=.\dgecon.c
# End Source File
# Begin Source File
SOURCE=.\dgemm.c
# End Source File
# Begin Source File
SOURCE=.\dgemv.c
# End Source File
# Begin Source File
SOURCE=.\dgeqpf.c
# End Source File
# Begin Source File
SOURCE=.\dgeqr2.c
# End Source File
# Begin Source File
SOURCE=.\dgeqrf.c
# End Source File
# Begin Source File
SOURCE=.\dger.c
# End Source File
# Begin Source File
SOURCE=.\dgerq2.c
# End Source File
# Begin Source File
SOURCE=.\dgesc2.c
# End Source File
# Begin Source File
SOURCE=.\dgetc2.c
# End Source File
# Begin Source File
SOURCE=.\dggbak.c
# End Source File
# Begin Source File
SOURCE=.\dggbal.c
# End Source File
# Begin Source File
SOURCE=.\dgges.c
# End Source File
# Begin Source File
SOURCE=.\dgghrd.c
# End Source File
# Begin Source File
SOURCE=.\dggsvd.c
# End Source File
# Begin Source File
SOURCE=.\dggsvp.c
# End Source File
# Begin Source File
SOURCE=.\dhgeqz.c
# End Source File
# Begin Source File
SOURCE=.\dlabad.c
# End Source File
# Begin Source File
SOURCE=.\dlacon.c
# End Source File
# Begin Source File
SOURCE=.\dlacpy.c
# End Source File
# Begin Source File
SOURCE=.\dladiv.c
# End Source File
# Begin Source File
SOURCE=.\dlag2.c
# End Source File
# Begin Source File
SOURCE=.\dlags2.c
# End Source File
# Begin Source File
SOURCE=.\dlagv2.c
# End Source File
# Begin Source File
SOURCE=.\dlamch.c
# End Source File
# Begin Source File
SOURCE=.\dlange.c
# End Source File
# Begin Source File
SOURCE=.\dlanhs.c
# End Source File
# Begin Source File
SOURCE=.\dlapll.c
# End Source File
# Begin Source File
SOURCE=.\dlapmt.c
# End Source File
# Begin Source File
SOURCE=.\dlapy2.c
# End Source File
# Begin Source File
SOURCE=.\dlapy3.c
# End Source File
# Begin Source File
SOURCE=.\dlarf.c
# End Source File
# Begin Source File
SOURCE=.\dlarfb.c
# End Source File
# Begin Source File
SOURCE=.\dlarfg.c
# End Source File
# Begin Source File
SOURCE=.\dlarft.c
# End Source File
# Begin Source File
SOURCE=.\dlartg.c
# End Source File
# Begin Source File
SOURCE=.\dlas2.c
# End Source File
# Begin Source File
SOURCE=.\dlascl.c
# End Source File
# Begin Source File
SOURCE=.\dlaset.c
# End Source File
# Begin Source File
SOURCE=.\dlassq.c
# End Source File
# Begin Source File
SOURCE=.\dlasv2.c
# End Source File
# Begin Source File
SOURCE=.\dlaswp.c
# End Source File
# Begin Source File
SOURCE=.\dlatdf.c
# End Source File
# Begin Source File
SOURCE=.\dlatrs.c
# End Source File
# Begin Source File
SOURCE=.\dnrm2.c
# End Source File
# Begin Source File
SOURCE=.\dorg2r.c
# End Source File
# Begin Source File
SOURCE=.\dorgqr.c
# End Source File
# Begin Source File
SOURCE=.\dorgr2.c
# End Source File
# Begin Source File
SOURCE=.\dorm2r.c
# End Source File
# Begin Source File
SOURCE=.\dormqr.c
# End Source File
# Begin Source File
SOURCE=.\dormr2.c
# End Source File
# Begin Source File
SOURCE=.\drot.c
# End Source File
# Begin Source File
SOURCE=.\drotg.c
# End Source File
# Begin Source File
SOURCE=.\drscl.c
# End Source File
# Begin Source File
SOURCE=.\dscal.c
# End Source File
# Begin Source File
SOURCE=.\dsrc2c.c
# End Source File
# Begin Source File
SOURCE=.\dswap.c
# End Source File
# Begin Source File
SOURCE=.\dtgex2.c
# End Source File
# Begin Source File
SOURCE=.\dtgexc.c
# End Source File
# Begin Source File
SOURCE=.\dtgsen.c
# End Source File
# Begin Source File
SOURCE=.\dtgsja.c
# End Source File
# Begin Source File
SOURCE=.\dtgsy2.c
# End Source File
# Begin Source File
SOURCE=.\dtgsyl.c
# End Source File
# Begin Source File
SOURCE=.\dtrans.c
# End Source File
# Begin Source File
SOURCE=.\dtrmm.c
# End Source File
# Begin Source File
SOURCE=.\dtrmv.c
# End Source File
# Begin Source File
SOURCE=.\dtrsv.c
# End Source File
# Begin Source File
SOURCE=.\dzasum.c
# End Source File
# Begin Source File
SOURCE=.\dznrm2.c
# End Source File
# Begin Source File
SOURCE=.\exit.c
# End Source File
# Begin Source File
SOURCE=.\i_dnnt.c
# End Source File
# Begin Source File
SOURCE=.\idamax.c
# End Source File
# Begin Source File
SOURCE=.\ilaenv.c
# End Source File
# Begin Source File
SOURCE=.\isamax.c
# End Source File
# Begin Source File
SOURCE=.\izamax.c
# End Source File
# Begin Source File
SOURCE=.\izmax1.c
# End Source File
# Begin Source File
SOURCE=.\lsame.c
# End Source File
# Begin Source File
SOURCE=.\linalg/lsqr.c
# End Source File
# Begin Source File
SOURCE=.\machineparams.c
# End Source File
# Begin Source File
SOURCE=.\pow_dd.c
# End Source File
# Begin Source File
SOURCE=.\pow_di.c
# End Source File
# Begin Source File
SOURCE=.\pow_ii.c
# End Source File
# Begin Source File
SOURCE=.\pow_ri.c
# End Source File
# Begin Source File
SOURCE=.\pythag.c
# End Source File
# Begin Source File
SOURCE=.\r_cnjg.c
# End Source File
# Begin Source File
SOURCE=.\r_imag.c
# End Source File
# Begin Source File
SOURCE=.\r_sign.c
# End Source File
# Begin Source File
SOURCE=.\rpoly.c
# End Source File
# Begin Source File
SOURCE=.\s_cat.c
# End Source File
# Begin Source File
SOURCE=.\s_cmp.c
# End Source File
# Begin Source File
SOURCE=.\s_copy.c
# End Source File
# Begin Source File
SOURCE=.\sasum.c
# End Source File
# Begin Source File
SOURCE=.\saxpy.c
# End Source File
# Begin Source File
SOURCE=.\scnrm2.c
# End Source File
# Begin Source File
SOURCE=.\scopy.c
# End Source File
# Begin Source File
SOURCE=.\sdot.c
# End Source File
# Begin Source File
SOURCE=.\sgemv.c
# End Source File
# Begin Source File
SOURCE=.\sgeqpf.c
# End Source File
# Begin Source File
SOURCE=.\sgeqr2.c
# End Source File
# Begin Source File
SOURCE=.\sger.c
# End Source File
# Begin Source File
SOURCE=.\sgerq2.c
# End Source File
# Begin Source File
SOURCE=.\sggsvd.c
# End Source File
# Begin Source File
SOURCE=.\sggsvp.c
# End Source File
# Begin Source File
SOURCE=.\sig_die.c
# End Source File
# Begin Source File
SOURCE=.\simpson.c
# End Source File
# Begin Source File
SOURCE=.\slacpy.c
# End Source File
# Begin Source File
SOURCE=.\slags2.c
# End Source File
# Begin Source File
SOURCE=.\slamch.c
# End Source File
# Begin Source File
SOURCE=.\slange.c
# End Source File
# Begin Source File
SOURCE=.\slapll.c
# End Source File
# Begin Source File
SOURCE=.\slapmt.c
# End Source File
# Begin Source File
SOURCE=.\slapy2.c
# End Source File
# Begin Source File
SOURCE=.\slarf.c
# End Source File
# Begin Source File
SOURCE=.\slarfg.c
# End Source File
# Begin Source File
SOURCE=.\slartg.c
# End Source File
# Begin Source File
SOURCE=.\slas2.c
# End Source File
# Begin Source File
SOURCE=.\slaset.c
# End Source File
# Begin Source File
SOURCE=.\slassq.c
# End Source File
# Begin Source File
SOURCE=.\slasv2.c
# End Source File
# Begin Source File
SOURCE=.\snrm2.c
# End Source File
# Begin Source File
SOURCE=.\sorg2r.c
# End Source File
# Begin Source File
SOURCE=.\sorm2r.c
# End Source File
# Begin Source File
SOURCE=.\sormr2.c
# End Source File
# Begin Source File
SOURCE=.\srot.c
# End Source File
# Begin Source File
SOURCE=.\srotg.c
# End Source File
# Begin Source File
SOURCE=.\sscal.c
# End Source File
# Begin Source File
SOURCE=.\sswap.c
# End Source File
# Begin Source File
SOURCE=.\stgsja.c
# End Source File
# Begin Source File
SOURCE=.\trans.c
# End Source File
# Begin Source File
SOURCE=.\trapezod.c
# End Source File
# Begin Source File
SOURCE=.\triangle.c
# End Source File
# Begin Source File
SOURCE=.\urand.c
# End Source File
# Begin Source File
SOURCE=.\xerbla.c
# End Source File
# Begin Source File
SOURCE=.\z_abs.c
# End Source File
# Begin Source File
SOURCE=.\z_div.c
# End Source File
# Begin Source File
SOURCE=.\z_sqrt.c
# End Source File
# Begin Source File
SOURCE=.\zaxpy.c
# End Source File
# Begin Source File
SOURCE=.\zcopy.c
# End Source File
# Begin Source File
SOURCE=.\zdotc.c
# End Source File
# Begin Source File
SOURCE=.\zdotu.c
# End Source File
# Begin Source File
SOURCE=.\zdrot.c
# End Source File
# Begin Source File
SOURCE=.\zdscal.c
# End Source File
# Begin Source File
SOURCE=.\zgebak.c
# End Source File
# Begin Source File
SOURCE=.\zgebal.c
# End Source File
# Begin Source File
SOURCE=.\zgehd2.c
# End Source File
# Begin Source File
SOURCE=.\zgehrd.c
# End Source File
# Begin Source File
SOURCE=.\zgemm.c
# End Source File
# Begin Source File
SOURCE=.\zgemv.c
# End Source File
# Begin Source File
SOURCE=.\zgerc.c
# End Source File
# Begin Source File
SOURCE=.\zhseqr.c
# End Source File
# Begin Source File
SOURCE=.\zlacgv.c
# End Source File
# Begin Source File
SOURCE=.\zlacpy.c
# End Source File
# Begin Source File
SOURCE=.\zladiv.c
# End Source File
# Begin Source File
SOURCE=.\zlahqr.c
# End Source File
# Begin Source File
SOURCE=.\zlahrd.c
# End Source File
# Begin Source File
SOURCE=.\zlange.c
# End Source File
# Begin Source File
SOURCE=.\zlanhs.c
# End Source File
# Begin Source File
SOURCE=.\zlarf.c
# End Source File
# Begin Source File
SOURCE=.\zlarfb.c
# End Source File
# Begin Source File
SOURCE=.\zlarfg.c
# End Source File
# Begin Source File
SOURCE=.\zlarft.c
# End Source File
# Begin Source File
SOURCE=.\zlarfx.c
# End Source File
# Begin Source File
SOURCE=.\zlascl.c
# End Source File
# Begin Source File
SOURCE=.\zlaset.c
# End Source File
# Begin Source File
SOURCE=.\zlassq.c
# End Source File
# Begin Source File
SOURCE=.\zlatrs.c
# End Source File
# Begin Source File
SOURCE=.\zscal.c
# End Source File
# Begin Source File
SOURCE=.\zswap.c
# End Source File
# Begin Source File
SOURCE=.\ztrevc.c
# End Source File
# Begin Source File
SOURCE=.\ztrmm.c
# End Source File
# Begin Source File
SOURCE=.\ztrmv.c
# End Source File
# Begin Source File
SOURCE=.\ztrsv.c
# End Source File
# Begin Source File
SOURCE=.\zung2r.c
# End Source File
# Begin Source File
SOURCE=.\zunghr.c
# End Source File
# Begin Source File
SOURCE=.\zungqr.c
# End Source File
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
SOURCE=.\netlib.h
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
# End Group
# End Target
# End Project
