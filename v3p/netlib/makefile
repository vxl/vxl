# Imakefile for netlib library
# This library contains public domain routines from netlib.att.com

USES = # nothing!
USE_STDC++ := 0
IULIBS = # no dependencies whatsoever

LIBRARY := netlib
VERSION := 1.0

#*********************
# Std Fortran Library
#*********************
SOURCES += F77_aloc.c
SOURCES += exit.c
SOURCES += s_cmp.c
SOURCES += d_lg10.c
SOURCES += s_cat.c
SOURCES += s_copy.c
SOURCES += d_sign.c
SOURCES += d_cnjg.c
SOURCES += d_imag.c
SOURCES += z_div.c
SOURCES += z_abs.c
SOURCES += z_sqrt.c
SOURCES += cabs.c
SOURCES += c_abs.c
SOURCES += c_div.c
SOURCES += sig_die.c
SOURCES += r_sign.c
SOURCES += r_imag.c
SOURCES += r_cnjg.c

SOURCES += pow_ii.c
SOURCES += pow_ri.c
SOURCES += pow_di.c
SOURCES += pow_dd.c

# BLAS
SOURCES += izamax.c

SOURCES += daxpy.c
SOURCES += dcopy.c
SOURCES += ddot.c
SOURCES += dnrm2.c
SOURCES += drot.c
SOURCES += drotg.c
SOURCES += dscal.c
SOURCES += dasum.c
SOURCES += dzasum.c
SOURCES += dswap.c
SOURCES += dgemv.c
SOURCES += dger.c

SOURCES += saxpy.c
SOURCES += scopy.c
SOURCES += sdot.c
SOURCES += snrm2.c
SOURCES += srot.c
SOURCES += srotg.c
SOURCES += sscal.c
SOURCES += sasum.c
SOURCES += sswap.c
SOURCES += sgemv.c
SOURCES += sger.c

SOURCES += caxpy.c
SOURCES += ccopy.c
SOURCES += cdotc.c
SOURCES += csrot.c
SOURCES += scnrm2.c
SOURCES += cscal.c
SOURCES += cswap.c

SOURCES += zaxpy.c
SOURCES += zcopy.c
SOURCES += zdotc.c
SOURCES += zdotu.c
SOURCES += zdrot.c
SOURCES += dznrm2.c
SOURCES += zdscal.c
SOURCES += zgemm.c
SOURCES += zgemv.c
SOURCES += zgerc.c
SOURCES += zscal.c
SOURCES += ztrmm.c
SOURCES += ztrmv.c
SOURCES += ztrsv.c
SOURCES += zswap.c

# LINALG
SOURCES += lsqr.c

# LINPACK
SOURCES += isamax.c
SOURCES += idamax.c

# ITPACK
SOURCES += dsrc2c.c

# MINPACK
SOURCES += pythag.c
SOURCES += rpoly.c

# TOMS Misc
SOURCES += dtrans.c
SOURCES += trans.c

# General
SOURCES += machineparams.c

# from DATAPAC
SOURCES += camsun.c

# from LAPACK
SOURCES += ilaenv.c
SOURCES += izmax1.c

SOURCES += xerbla.c
SOURCES += urand.c
SOURCES += lsame.c
SOURCES += dlamch.c
SOURCES += slamch.c

SOURCES += dgeqpf.c
SOURCES += dgeqr2.c
SOURCES += dgerq2.c
SOURCES += dggsvd.c
SOURCES += dggsvp.c
SOURCES += dlacpy.c
SOURCES += dlags2.c
SOURCES += dlange.c
SOURCES += dlapll.c
SOURCES += dlapmt.c
SOURCES += dlapy2.c
SOURCES += dlarf.c
SOURCES += dlarfg.c
SOURCES += dlartg.c
SOURCES += dlas2.c
SOURCES += dlaset.c
SOURCES += dlassq.c
SOURCES += dlasv2.c
SOURCES += dlabad.c
SOURCES += dladiv.c
SOURCES += dlapy3.c

SOURCES += dorg2r.c
SOURCES += dorm2r.c
SOURCES += dormr2.c
SOURCES += dtgsja.c
SOURCES += sggsvd.c
SOURCES += sgeqpf.c
SOURCES += sgeqr2.c
SOURCES += sgerq2.c
SOURCES += sggsvp.c
SOURCES += slacpy.c
SOURCES += slags2.c
SOURCES += slange.c
SOURCES += slapll.c
SOURCES += slapmt.c
SOURCES += slapy2.c
SOURCES += slarf.c
SOURCES += slarfg.c
SOURCES += slartg.c
SOURCES += slas2.c
SOURCES += slaset.c
SOURCES += slassq.c
SOURCES += slasv2.c
SOURCES += sorg2r.c
SOURCES += sorm2r.c
SOURCES += sormr2.c
SOURCES += stgsja.c

SOURCES += zgebak.c
SOURCES += zgebal.c
#SOURCES += zgeev.c
SOURCES += zgehd2.c
SOURCES += zgehrd.c
SOURCES += zhseqr.c
SOURCES += zlacgv.c
SOURCES += zlacpy.c
SOURCES += zladiv.c
SOURCES += zlahqr.c
SOURCES += zlahrd.c
SOURCES += zlange.c
SOURCES += zlanhs.c
SOURCES += zlarf.c
SOURCES += zlarfb.c
SOURCES += zlarfg.c
SOURCES += zlarft.c
SOURCES += zlarfx.c
SOURCES += zlascl.c
SOURCES += zlaset.c
SOURCES += zlassq.c
SOURCES += zlatrs.c
SOURCES += ztrevc.c
SOURCES += zung2r.c
SOURCES += zunghr.c
SOURCES += zungqr.c

# Added by fsm for generalized chur
SOURCES += dgecon.c
SOURCES += dgemm.c
SOURCES += dgeqrf.c
SOURCES += dgesc2.c
SOURCES += dgetc2.c
SOURCES += dggbak.c
SOURCES += dggbal.c
SOURCES += dgges.c
SOURCES += dgghrd.c
SOURCES += dhgeqz.c
SOURCES += dlacon.c
SOURCES += dlag2.c
SOURCES += dlagv2.c
SOURCES += dlanhs.c
SOURCES += dlarfb.c
SOURCES += dlarft.c
SOURCES += dlascl.c
SOURCES += dlaswp.c
SOURCES += dlatdf.c
SOURCES += dlatrs.c
SOURCES += dorgqr.c
SOURCES += dorgr2.c
SOURCES += dormqr.c
SOURCES += drscl.c
SOURCES += dtgex2.c
SOURCES += dtgexc.c
SOURCES += dtgsen.c
SOURCES += dtgsy2.c
SOURCES += dtgsyl.c
SOURCES += dtrmm.c
SOURCES += dtrmv.c
SOURCES += dtrsv.c
SOURCES += i_dnnt.c

# The "Triangle" program of Jonathan Richard Shewchuk
SOURCES += triangle.c

# numeric integration
SOURCES += trapezod.c
SOURCES += simpson.c
SOURCES += adaquad.c

MANPAGE_SOURCES += f2c.h
MANPAGE_SOURCES += netlib.h

# List sources for each library component.

# V3P_NETLIB_header_files
MANPAGE_SOURCES += v3p_f2c.h
MANPAGE_SOURCES += v3p_f2c_mangle.h
MANPAGE_SOURCES += v3p_f2c_original.h
MANPAGE_SOURCES += v3p_f2c_unmangle.h
MANPAGE_SOURCES += v3p_netlib.h
MANPAGE_SOURCES += v3p_netlib_mangle.h
MANPAGE_SOURCES += v3p_netlib_prototypes.h
MANPAGE_SOURCES += v3p_netlib_unmangle.h

# V3P_NETLIB_libf2c_SOURCES
SOURCES += libf2c/f77_aloc.c
SOURCES += libf2c/exit_.c
SOURCES += libf2c/c_abs.c
SOURCES += libf2c/c_div.c
SOURCES += libf2c/c_sqrt.c
SOURCES += libf2c/cabs.c
SOURCES += libf2c/d_cnjg.c
SOURCES += libf2c/d_imag.c
SOURCES += libf2c/d_lg10.c
SOURCES += libf2c/d_sign.c
SOURCES += libf2c/i_dnnt.c
SOURCES += libf2c/pow_dd.c
SOURCES += libf2c/pow_di.c
SOURCES += libf2c/pow_ii.c
SOURCES += libf2c/pow_ri.c
SOURCES += libf2c/r_cnjg.c
SOURCES += libf2c/r_imag.c
SOURCES += libf2c/r_sign.c
SOURCES += libf2c/s_cat.c
SOURCES += libf2c/s_cmp.c
SOURCES += libf2c/s_copy.c
SOURCES += libf2c/sig_die.c
SOURCES += libf2c/z_abs.c
SOURCES += libf2c/z_div.c
SOURCES += libf2c/z_sqrt.c
# V3P_NETLIB_blas_SOURCES
SOURCES += blas/caxpy.c
SOURCES += blas/ccopy.c
SOURCES += blas/cdotc.c
SOURCES += blas/cscal.c
SOURCES += blas/csrot.c
SOURCES += blas/cswap.c
SOURCES += blas/dasum.c
SOURCES += blas/daxpy.c
SOURCES += blas/dcabs1.c
SOURCES += blas/dcopy.c
SOURCES += blas/ddot.c
SOURCES += blas/dlamch.c
SOURCES += blas/dnrm2.c
SOURCES += blas/drot.c
SOURCES += blas/drotg.c
SOURCES += blas/dscal.c
SOURCES += blas/dswap.c
SOURCES += blas/dzasum.c
SOURCES += blas/dznrm2.c
SOURCES += blas/idamax.c
SOURCES += blas/izamax.c
SOURCES += blas/saxpy.c
SOURCES += blas/scnrm2.c
SOURCES += blas/scopy.c
SOURCES += blas/sdot.c
SOURCES += blas/snrm2.c
SOURCES += blas/srot.c
SOURCES += blas/srotg.c
SOURCES += blas/sscal.c
SOURCES += blas/sswap.c
SOURCES += blas/xerbla.c
SOURCES += blas/zaxpy.c
SOURCES += blas/zcopy.c
SOURCES += blas/zdotc.c
SOURCES += blas/zdotu.c
SOURCES += blas/zdrot.c
SOURCES += blas/zdscal.c
SOURCES += blas/zgemm.c
SOURCES += blas/zgemv.c
SOURCES += blas/zgerc.c
SOURCES += blas/zscal.c
SOURCES += blas/zswap.c
SOURCES += blas/ztrmm.c
SOURCES += blas/ztrmv.c
SOURCES += blas/ztrsv.c
# V3P_NETLIB_linpack_SOURCES
SOURCES += linpack/sqrdc.c # xQRDC
SOURCES += linpack/dqrdc.c
SOURCES += linpack/cqrdc.c
SOURCES += linpack/zqrdc.c
SOURCES += linpack/sqrsl.c # xQRSL
SOURCES += linpack/dqrsl.c
SOURCES += linpack/cqrsl.c
SOURCES += linpack/zqrsl.c

SOURCES += linpack/csvdc.c # xSVD
SOURCES += linpack/dsvdc.c
SOURCES += linpack/ssvdc.c
SOURCES += linpack/zsvdc.c

SOURCES += linpack/dpoco.c #
SOURCES += linpack/dpodi.c
SOURCES += linpack/dpofa.c
SOURCES += linpack/dposl.c
# V3P_NETLIB_temperton_SOURCES
SOURCES += temperton/dgpfa.c  # FFT
SOURCES += temperton/dgpfa2f.c
SOURCES += temperton/dgpfa3f.c
SOURCES += temperton/dgpfa5f.c
SOURCES += temperton/gpfa.c
SOURCES += temperton/gpfa2f.c
SOURCES += temperton/gpfa3f.c
SOURCES += temperton/gpfa5f.c
SOURCES += temperton/setdgpfa.c
SOURCES += temperton/setgpfa.c
# V3P_NETLIB_eispack_SOURCES
SOURCES += eispack/balanc.c
SOURCES += eispack/balbak.c
SOURCES += eispack/cdiv.c
SOURCES += eispack/elmhes.c
SOURCES += eispack/eltran.c
SOURCES += eispack/epslon.c
SOURCES += eispack/hqr.c
SOURCES += eispack/hqr2.c
SOURCES += eispack/otqlrat.c
SOURCES += eispack/pythag.c
SOURCES += eispack/rebak.c
SOURCES += eispack/reduc.c
SOURCES += eispack/rg.c
SOURCES += eispack/rs.c
SOURCES += eispack/rsg.c
SOURCES += eispack/tql1.c
SOURCES += eispack/tql2.c
SOURCES += eispack/tred1.c
SOURCES += eispack/tred2.c
# V3P_NETLIB_laso_SOURCES
SOURCES += laso/dlabax.c
SOURCES += laso/dlabcm.c
SOURCES += laso/dlabfc.c
SOURCES += laso/dlaeig.c
SOURCES += laso/dlager.c
SOURCES += laso/dlaran.c
SOURCES += laso/dmvpc.c
SOURCES += laso/dnlaso.c
SOURCES += laso/dnppla.c
SOURCES += laso/dortqr.c
SOURCES += laso/dvsort.c
SOURCES += laso/urand.c
# V3P_NETLIB_lapack_SOURCES
SOURCES += lapack/complex16/zgebak.c
SOURCES += lapack/complex16/zgebal.c
SOURCES += lapack/complex16/zgeev.c
SOURCES += lapack/complex16/zgehd2.c
SOURCES += lapack/complex16/zgehrd.c
SOURCES += lapack/complex16/zhseqr.c
SOURCES += lapack/complex16/zlacgv.c
SOURCES += lapack/complex16/zlacpy.c
SOURCES += lapack/complex16/zladiv.c
SOURCES += lapack/complex16/zlahqr.c
SOURCES += lapack/complex16/zlahrd.c
SOURCES += lapack/complex16/zlange.c
SOURCES += lapack/complex16/zlanhs.c
SOURCES += lapack/complex16/zlarf.c
SOURCES += lapack/complex16/zlarfb.c
SOURCES += lapack/complex16/zlarfg.c
SOURCES += lapack/complex16/zlarft.c
SOURCES += lapack/complex16/zlarfx.c
SOURCES += lapack/complex16/zlascl.c
SOURCES += lapack/complex16/zlaset.c
SOURCES += lapack/complex16/zlassq.c
SOURCES += lapack/complex16/zlatrs.c
SOURCES += lapack/complex16/ztrevc.c
SOURCES += lapack/complex16/zung2r.c
SOURCES += lapack/complex16/zunghr.c
SOURCES += lapack/complex16/zungqr.c
SOURCES += lapack/double/dlabad.c
SOURCES += lapack/double/dladiv.c
SOURCES += lapack/double/dlapy2.c
SOURCES += lapack/double/dlapy3.c
SOURCES += lapack/util/ieeeck.c
SOURCES += lapack/util/ilaenv.c
SOURCES += lapack/util/lsame.c
# V3P_NETLIB_napack_SOURCES
SOURCES += napack/cg.c
# V3P_NETLIB_minpack_SOURCES
SOURCES += minpack/dpmpar.c
SOURCES += minpack/enorm.c
SOURCES += minpack/fdjac2.c
SOURCES += minpack/lmder.c
SOURCES += minpack/lmder1.c
SOURCES += minpack/lmdif.c
SOURCES += minpack/lmpar.c
SOURCES += minpack/qrfac.c
SOURCES += minpack/qrsolv.c
# V3P_NETLIB_opt_SOURCES
SOURCES += opt/lbfgs.c

#######################################
include ${VXLROOT}/config/top-params.mk
#######################################

ifndef NOTEST
SUBDIRS += tests examples
endif

######################################
include ${VXLROOT}/config/top-rules.mk
######################################

# capes
ifeq ($(OS),Linux2)
  CFLAGS += $(optimize)
endif

# fsm
ifeq ($(OS),SunOS5)
  CFLAGS += $(optimize)
endif
ifeq ($(IU_COMPILER),gcc-2.95)
  CFLAGS += -fstrict-aliasing
endif

ifeq ($(OS),HP-UX10.20)
  FLAGS_gpfa5f.c = -O0
  FLAGS_dgpfa5f.c = -O0
endif
ifeq ($(IU_COMPILER),gcc)
  FLAGS_zlarfx.c = -Os
endif

ifneq ($(OS),win32)
#   debug :=
#   APP_CFLAGS = $(optimize)
endif

ifeq ($(OS),SunOS5)
  pic := $(PIC)
endif
