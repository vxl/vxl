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
# /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/vxl" /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/vcl/config.win32-VC60"  == include path
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
# ADD BASE CPP /nologo /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB"  /FD /c
# ADD CPP /nologo /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB"  /FD /c
# ADD CPP /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/vxl" /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/vcl/config.win32-VC60"    -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE /D "netlib_EXPORTS"
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
# ADD CPP /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/vxl" /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/vcl/config.win32-VC60"    -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE /D "netlib_EXPORTS"
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
# PROP Default_Filter ""
# Begin Source File

SOURCE=.\F77_aloc.c

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

SOURCE=.\cg.c

# End Source File
# Begin Source File

SOURCE=.\cqrdc.c

# End Source File
# Begin Source File

SOURCE=.\cqrsl.c

# End Source File
# Begin Source File

SOURCE=.\cscal.c

# End Source File
# Begin Source File

SOURCE=.\csrot.c

# End Source File
# Begin Source File

SOURCE=.\csvdc.c

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

SOURCE=.\dgemv.c

# End Source File
# Begin Source File

SOURCE=.\dgeqpf.c

# End Source File
# Begin Source File

SOURCE=.\dgeqr2.c

# End Source File
# Begin Source File

SOURCE=.\dger.c

# End Source File
# Begin Source File

SOURCE=.\dgerq2.c

# End Source File
# Begin Source File

SOURCE=.\dggsvd.c

# End Source File
# Begin Source File

SOURCE=.\dggsvp.c

# End Source File
# Begin Source File

SOURCE=.\dgpfa.c

# End Source File
# Begin Source File

SOURCE=.\dgpfa2f.c

# End Source File
# Begin Source File

SOURCE=.\dgpfa3f.c

# End Source File
# Begin Source File

SOURCE=.\dgpfa5f.c

# End Source File
# Begin Source File

SOURCE=.\dlabad.c

# End Source File
# Begin Source File

SOURCE=.\dlacpy.c

# End Source File
# Begin Source File

SOURCE=.\dladiv.c

# End Source File
# Begin Source File

SOURCE=.\dlags2.c

# End Source File
# Begin Source File

SOURCE=.\dlamch.c

# End Source File
# Begin Source File

SOURCE=.\dlange.c

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

SOURCE=.\dlarfg.c

# End Source File
# Begin Source File

SOURCE=.\dlartg.c

# End Source File
# Begin Source File

SOURCE=.\dlas2.c

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

SOURCE=.\dnlaso.c

# End Source File
# Begin Source File

SOURCE=.\dnrm2.c

# End Source File
# Begin Source File

SOURCE=.\dorg2r.c

# End Source File
# Begin Source File

SOURCE=.\dorm2r.c

# End Source File
# Begin Source File

SOURCE=.\dormr2.c

# End Source File
# Begin Source File

SOURCE=.\dpoco.c

# End Source File
# Begin Source File

SOURCE=.\dpodi.c

# End Source File
# Begin Source File

SOURCE=.\dpofa.c

# End Source File
# Begin Source File

SOURCE=.\dposl.c

# End Source File
# Begin Source File

SOURCE=.\dqrdc.c

# End Source File
# Begin Source File

SOURCE=.\dqrsl.c

# End Source File
# Begin Source File

SOURCE=.\drot.c

# End Source File
# Begin Source File

SOURCE=.\drotg.c

# End Source File
# Begin Source File

SOURCE=.\dscal.c

# End Source File
# Begin Source File

SOURCE=.\dsetgpfa.c

# End Source File
# Begin Source File

SOURCE=.\dsvdc.c

# End Source File
# Begin Source File

SOURCE=.\dswap.c

# End Source File
# Begin Source File

SOURCE=.\dtgsja.c

# End Source File
# Begin Source File

SOURCE=.\dtrans.c

# End Source File
# Begin Source File

SOURCE=.\dzasum.c

# End Source File
# Begin Source File

SOURCE=.\enorm.c

# End Source File
# Begin Source File

SOURCE=.\exit.c

# End Source File
# Begin Source File

SOURCE=.\fdjac2.c

# End Source File
# Begin Source File

SOURCE=.\gpfa.c

# End Source File
# Begin Source File

SOURCE=.\gpfa2f.c

# End Source File
# Begin Source File

SOURCE=.\gpfa3f.c

# End Source File
# Begin Source File

SOURCE=.\gpfa5f.c

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

SOURCE=.\lbfgs-lb1.c

# End Source File
# Begin Source File

SOURCE=.\lbfgs.c

# End Source File
# Begin Source File

SOURCE=.\lmder.c

# End Source File
# Begin Source File

SOURCE=.\lmder1.c

# End Source File
# Begin Source File

SOURCE=.\lmdif.c

# End Source File
# Begin Source File

SOURCE=.\lmpar.c

# End Source File
# Begin Source File

SOURCE=.\lsame.c

# End Source File
# Begin Source File

SOURCE=.\lsqr.c

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

SOURCE=.\qrfac.c

# End Source File
# Begin Source File

SOURCE=.\qrsolv.c

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

SOURCE=.\rg.c

# End Source File
# Begin Source File

SOURCE=.\rpoly.c

# End Source File
# Begin Source File

SOURCE=.\rs.c

# End Source File
# Begin Source File

SOURCE=.\rsg.c

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

SOURCE=.\setgpfa.c

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

SOURCE=.\snlaso.c

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

SOURCE=.\sqrdc.c

# End Source File
# Begin Source File

SOURCE=.\sqrsl.c

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

SOURCE=.\ssvdc.c

# End Source File
# Begin Source File

SOURCE=.\sswap.c

# End Source File
# Begin Source File

SOURCE=.\stgsja.c

# End Source File
# Begin Source File

SOURCE=.\tql1.c

# End Source File
# Begin Source File

SOURCE=.\tql2.c

# End Source File
# Begin Source File

SOURCE=.\trans.c

# End Source File
# Begin Source File

SOURCE=.\tred1.c

# End Source File
# Begin Source File

SOURCE=.\tred2.c

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

SOURCE=.\zgeev.c

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

SOURCE=.\zqrdc.c

# End Source File
# Begin Source File

SOURCE=.\zqrsl.c

# End Source File
# Begin Source File

SOURCE=.\zscal.c

# End Source File
# Begin Source File

SOURCE=.\zsvdc.c

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

SOURCE=.\i_dnnt.c

# End Source File
# Begin Source File

SOURCE=.\dtrsv.c

# End Source File
# Begin Source File

SOURCE=.\dtrmv.c

# End Source File
# Begin Source File

SOURCE=.\dtrmm.c

# End Source File
# Begin Source File

SOURCE=.\dtgsyl.c

# End Source File
# Begin Source File

SOURCE=.\dtgsy2.c

# End Source File
# Begin Source File

SOURCE=.\dtgsen.c

# End Source File
# Begin Source File

SOURCE=.\dtgexc.c

# End Source File
# Begin Source File

SOURCE=.\dtgex2.c

# End Source File
# Begin Source File

SOURCE=.\drscl.c

# End Source File
# Begin Source File

SOURCE=.\dormqr.c

# End Source File
# Begin Source File

SOURCE=.\dorgr2.c

# End Source File
# Begin Source File

SOURCE=.\dorgqr.c

# End Source File
# Begin Source File

SOURCE=.\dlatrs.c

# End Source File
# Begin Source File

SOURCE=.\dlatdf.c

# End Source File
# Begin Source File

SOURCE=.\dlaswp.c

# End Source File
# Begin Source File

SOURCE=.\dlascl.c

# End Source File
# Begin Source File

SOURCE=.\dlarft.c

# End Source File
# Begin Source File

SOURCE=.\dlarfb.c

# End Source File
# Begin Source File

SOURCE=.\dlanhs.c

# End Source File
# Begin Source File

SOURCE=.\dlagv2.c

# End Source File
# Begin Source File

SOURCE=.\dlag2.c

# End Source File
# Begin Source File

SOURCE=.\dlacon.c

# End Source File
# Begin Source File

SOURCE=.\dhgeqz.c

# End Source File
# Begin Source File

SOURCE=.\dgghrd.c

# End Source File
# Begin Source File

SOURCE=.\dgges.c

# End Source File
# Begin Source File

SOURCE=.\dggbal.c

# End Source File
# Begin Source File

SOURCE=.\dggbak.c

# End Source File
# Begin Source File

SOURCE=.\dgetc2.c

# End Source File
# Begin Source File

SOURCE=.\dgesc2.c

# End Source File
# Begin Source File

SOURCE=.\dgeqrf.c

# End Source File
# Begin Source File

SOURCE=.\dgemm.c

# End Source File
# Begin Source File

SOURCE=.\dgecon.c

# End Source File
# Begin Source File

SOURCE=.\urand.c

# End Source File
# Begin Source File

SOURCE=.\dznrm2.c

# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\f2c.h

# End Source File
# Begin Source File

SOURCE=.\netlib.h

# End Source File
# End Group
# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project

