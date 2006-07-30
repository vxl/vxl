# Imakefile for netlib library
# This library contains public domain routines from netlib.att.com

USES = # nothing!
USE_STDC++ := 0
IULIBS = # no dependencies whatsoever

LIBRARY := netlib
VERSION := 1.0

# TOMS Misc
#SOURCES += dtrans.c
#SOURCES += trans.c

# General
#SOURCES += machineparams.c

# The "Triangle" program of Jonathan Richard Shewchuk
SOURCES += triangle.c

# List sources for each library component.

# V3P_NETLIB_header_files
SOURCES += v3p_netlib_init.c
MANPAGE_SOURCES += v3p_f2c.h
MANPAGE_SOURCES += v3p_f2c_mangle.h
MANPAGE_SOURCES += v3p_f2c_original.h
MANPAGE_SOURCES += v3p_f2c_unmangle.h
MANPAGE_SOURCES += v3p_netlib.h
MANPAGE_SOURCES += v3p_netlib_mangle.h
MANPAGE_SOURCES += v3p_netlib_prototypes.h
MANPAGE_SOURCES += v3p_netlib_unmangle.h

#*********************
# Std Fortran Library
#*********************
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
SOURCES += blas/dgemm.c
SOURCES += blas/dgemv.c
SOURCES += blas/dger.c
SOURCES += blas/dtrmm.c
SOURCES += blas/dtrmv.c
SOURCES += blas/dtrsv.c
SOURCES += blas/isamax.c
SOURCES += blas/sgemv.c
SOURCES += blas/sger.c
SOURCES += blas/slamch.c
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
# V3P_NETLIB_linalg_SOURCES
SOURCES += linalg/lsqr.c
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
SOURCES += lapack/double/dgecon.c
SOURCES += lapack/double/dgeqr2.c
SOURCES += lapack/double/dgeqrf.c
SOURCES += lapack/double/dgerq2.c
SOURCES += lapack/double/dgesc2.c
SOURCES += lapack/double/dgetc2.c
SOURCES += lapack/double/dggbak.c
SOURCES += lapack/double/dggbal.c
SOURCES += lapack/double/dgges.c
SOURCES += lapack/double/dgghrd.c
SOURCES += lapack/double/dhgeqz.c
SOURCES += lapack/double/dlacon.c
SOURCES += lapack/double/dlacpy.c
SOURCES += lapack/double/dlag2.c
SOURCES += lapack/double/dlagv2.c
SOURCES += lapack/double/dlange.c
SOURCES += lapack/double/dlanhs.c
SOURCES += lapack/double/dlarf.c
SOURCES += lapack/double/dlarfb.c
SOURCES += lapack/double/dlarfg.c
SOURCES += lapack/double/dlarft.c
SOURCES += lapack/double/dlartg.c
SOURCES += lapack/double/dlascl.c
SOURCES += lapack/double/dlaset.c
SOURCES += lapack/double/dlassq.c
SOURCES += lapack/double/dlasv2.c
SOURCES += lapack/double/dlaswp.c
SOURCES += lapack/double/dlatdf.c
SOURCES += lapack/double/dlatrs.c
SOURCES += lapack/double/dorg2r.c
SOURCES += lapack/double/dorgqr.c
SOURCES += lapack/double/dorgr2.c
SOURCES += lapack/double/dorm2r.c
SOURCES += lapack/double/dormqr.c
SOURCES += lapack/double/dormr2.c
SOURCES += lapack/double/drscl.c
SOURCES += lapack/double/dtgex2.c
SOURCES += lapack/double/dtgexc.c
SOURCES += lapack/double/dtgsen.c
SOURCES += lapack/double/dtgsy2.c
SOURCES += lapack/double/dtgsyl.c
SOURCES += lapack/double/dlaev2.c
SOURCES += lapack/double/dspr.c
SOURCES += lapack/double/dsptrf.c
SOURCES += lapack/double/dsptrs.c
SOURCES += lapack/single/sgeqr2.c
SOURCES += lapack/single/sgeqpf.c
SOURCES += lapack/single/sgerq2.c
SOURCES += lapack/single/sggsvd.c
SOURCES += lapack/single/sggsvp.c
SOURCES += lapack/single/slacpy.c
SOURCES += lapack/single/slags2.c
SOURCES += lapack/single/slange.c
SOURCES += lapack/single/slapll.c
SOURCES += lapack/single/slapmt.c
SOURCES += lapack/single/slapy2.c
SOURCES += lapack/single/slarf.c
SOURCES += lapack/single/slarfg.c
SOURCES += lapack/single/slartg.c
SOURCES += lapack/single/slas2.c
SOURCES += lapack/single/slaset.c
SOURCES += lapack/single/slassq.c
SOURCES += lapack/single/slasv2.c
SOURCES += lapack/single/sorg2r.c
SOURCES += lapack/single/sorm2r.c
SOURCES += lapack/single/sormr2.c
SOURCES += lapack/single/stgsja.c
#SOURCES += izmax1.c
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
# V3P_NETLIB_toms_SOURCES
SOURCES += toms/rpoly.c
# V3P_NETLIB_datapac_SOURCES
SOURCES += datapac/camsun.c
# V3P_NETLIB_mathews_SOURCES
SOURCES += mathews/adaquad.c
SOURCES += mathews/simpson.c
SOURCES += mathews/trapezod.c

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
