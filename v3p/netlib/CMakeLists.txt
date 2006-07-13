# ./v3p/netlib/CMakeLists.txt

PROJECT( netlib C )

SET( netlib_sources
  f2c.h
  netlib.h

  # Std Fortran Library
  F77_aloc.c
  exit.c
  s_cmp.c
  d_lg10.c
  s_cat.c
  s_copy.c
  d_sign.c
  d_cnjg.c
  d_imag.c
  z_div.c
  z_abs.c
  z_sqrt.c
  cabs.c
  c_abs.c
  c_div.c
  sig_die.c
  r_sign.c
  r_imag.c
  r_cnjg.c

  pow_ii.c
  pow_ri.c
  pow_di.c
  pow_dd.c

  # BLAS
  izamax.c

  daxpy.c
  dcopy.c
  ddot.c
  dnrm2.c
  drot.c
  drotg.c
  dscal.c
  dasum.c
  dzasum.c
  dswap.c
  dgemv.c
  dger.c

  saxpy.c
  scopy.c
  sdot.c
  snrm2.c
  srot.c
  srotg.c
  sscal.c
  sasum.c
  sswap.c
  sgemv.c
  sger.c
  caxpy.c
  ccopy.c
  cdotc.c
  csrot.c
  scnrm2.c
  cscal.c
  cswap.c
  zaxpy.c
  zcopy.c
  zdotc.c
  zdotu.c
  zdrot.c
  dznrm2.c
  zdscal.c
  zgemm.c
  zgemv.c
  zgerc.c
  zscal.c
  ztrmm.c
  ztrmv.c
  ztrsv.c
  zswap.c

  # LINPACK
  isamax.c
  idamax.c

  # ITPACK
  dsrc2c.c

  # MINPACK
  pythag.c
  rpoly.c

  # TOMS Misc
  dtrans.c
  trans.c

  # General
  machineparams.c

  # from DATAPAC
  camsun.c

  # from LAPACK
  ilaenv.c
  izmax1.c

  xerbla.c
  urand.c
  lsame.c
  dlamch.c
  slamch.c
  
  dgeqpf.c
  dgeqr2.c
  dgerq2.c
  dggsvd.c
  dggsvp.c
  dlacpy.c
  dlags2.c
  dlange.c
  dlapll.c
  dlapmt.c
  dlapy2.c
  dlarf.c
  dlarfg.c
  dlartg.c
  dlas2.c
  dlaset.c
  dlassq.c
  dlasv2.c
  dlabad.c
  dladiv.c
  dlapy3.c

  dorg2r.c
  dorm2r.c
  dormr2.c
  dtgsja.c
  sggsvd.c
  sgeqpf.c
  sgeqr2.c
  sgerq2.c
  sggsvp.c
  slacpy.c
  slags2.c
  slange.c
  slapll.c
  slapmt.c
  slapy2.c
  slarf.c
  slarfg.c
  slartg.c
  slas2.c
  slaset.c
  slassq.c
  slasv2.c
  sorg2r.c
  sorm2r.c
  sormr2.c
  stgsja.c

  zgebak.c
  zgebal.c
  zgehd2.c
  zgehrd.c
  zhseqr.c
  zlacgv.c
  zlacpy.c
  zladiv.c
  zlahqr.c
  zlahrd.c
  zlange.c
  zlanhs.c
  zlarf.c
  zlarfb.c
  zlarfg.c
  zlarft.c
  zlarfx.c
  zlascl.c
  zlaset.c
  zlassq.c
  zlatrs.c
  ztrevc.c
  zung2r.c
  zunghr.c
  zungqr.c

  # Added by fsm for generalized chur
  dgecon.c
  dgemm.c
  dgeqrf.c
  dgesc2.c
  dgetc2.c
  dggbak.c
  dggbal.c
  dgges.c
  dgghrd.c
  dhgeqz.c
  dlacon.c
  dlag2.c
  dlagv2.c
  dlanhs.c
  dlarfb.c
  dlarft.c
  dlascl.c
  dlaswp.c
  dlatdf.c
  dlatrs.c
  dorgqr.c
  dorgr2.c
  dormqr.c
  drscl.c
  dtgex2.c
  dtgexc.c
  dtgsen.c
  dtgsy2.c
  dtgsyl.c
  dtrmm.c
  dtrmv.c
  dtrsv.c
  i_dnnt.c

  # The "Triangle" program of Jonathan Richard Shewchuk
  triangle.c triangle.h

  # numeric integration
  trapezod.c
  simpson.c
  adaquad.c
)

IF(CMAKE_COMPILER_IS_GNUCC)
# These really need to be inserted later in the options list to gcc,
# but this isn't supported in CMake 1.8.3
  SET_SOURCE_FILES_PROPERTIES(zlarfx.c PROPERTIES COMPILE_FLAGS -Os)
ENDIF(CMAKE_COMPILER_IS_GNUCC)

ADD_LIBRARY( netlib ${netlib_sources} )
INSTALL_TARGETS( /lib netlib )
INSTALL_FILES( /include/v3p/netlib .h netlib )

IF(UNIX)
  TARGET_LINK_LIBRARIES( netlib m )
ENDIF(UNIX)

IF(BUILD_TESTING)
  SUBDIRS(tests)
ENDIF(BUILD_TESTING)

IF(BUILD_EXAMPLES)
  SUBDIRS(examples)
ENDIF(BUILD_EXAMPLES)

# Allow sources in subdirectories to see the include files.
INCLUDE_DIRECTORIES(${netlib_SOURCE_DIR})

# Inform header files they are being included from a source inside the
# v3p_netlib library.
ADD_DEFINITIONS(-DV3P_NETLIB_SRC)

# List sources for each library component.
SET(V3P_NETLIB_header_sources
  v3p_f2c.h
  v3p_f2c_mangle.h
  v3p_f2c_original.h
  v3p_f2c_unmangle.h
  v3p_netlib.h
  v3p_netlib_mangle.h
  v3p_netlib_prototypes.h
  v3p_netlib_unmangle.h
  )
SET(V3P_NETLIB_libf2c_SOURCES
  libf2c/f77_aloc.c
  libf2c/exit_.c
  libf2c/c_abs.c
  libf2c/c_div.c
  libf2c/c_sqrt.c
  libf2c/cabs.c
  libf2c/d_cnjg.c
  libf2c/d_imag.c
  libf2c/d_lg10.c
  libf2c/d_sign.c
  libf2c/i_dnnt.c
  libf2c/pow_dd.c
  libf2c/pow_di.c
  libf2c/pow_ii.c
  libf2c/pow_ri.c
  libf2c/r_cnjg.c
  libf2c/r_imag.c
  libf2c/r_sign.c
  libf2c/s_cat.c
  libf2c/s_cmp.c
  libf2c/s_copy.c
  libf2c/sig_die.c
  libf2c/z_abs.c
  libf2c/z_div.c
  libf2c/z_sqrt.c
  )
SET(V3P_NETLIB_blas_SOURCES
  blas/caxpy.c blas/caxpy.h
  blas/ccopy.c blas/ccopy.h
  blas/cdotc.c blas/cdotc.h
  blas/cscal.c blas/cscal.h
  blas/csrot.c blas/csrot.h
  blas/cswap.c blas/cswap.h
  blas/dasum.c blas/dasum.h
  blas/daxpy.c blas/daxpy.h
  blas/dcabs1.c blas/dcabs1.h
  blas/dcopy.c blas/dcopy.h
  blas/ddot.c blas/ddot.h
  blas/dlamch.c blas/dlamch.h
  blas/dnrm2.c blas/dnrm2.h
  blas/drot.c blas/drot.h
  blas/drotg.c blas/drotg.h
  blas/dscal.c blas/dscal.h
  blas/dswap.c blas/dswap.h
  blas/dzasum.c blas/dzasum.h
  blas/dznrm2.c blas/dznrm2.h
  blas/idamax.c blas/idamax.h
  blas/izamax.c blas/izamax.h
  blas/saxpy.c blas/saxpy.h
  blas/scnrm2.c blas/scnrm2.h
  blas/scopy.c blas/scopy.h
  blas/sdot.c blas/sdot.h
  blas/snrm2.c blas/snrm2.h
  blas/srot.c blas/srot.h
  blas/srotg.c blas/srotg.h
  blas/sscal.c blas/sscal.h
  blas/sswap.c blas/sswap.h
  blas/xerbla.c blas/xerbla.h
  blas/zaxpy.c blas/zaxpy.h
  blas/zcopy.c blas/zcopy.h
  blas/zdotc.c blas/zdotc.h
  blas/zdotu.c blas/zdotu.h
  blas/zdrot.c blas/zdrot.h
  blas/zdscal.c blas/zdscal.h
  blas/zgemm.c blas/zgemm.h
  blas/zgemv.c blas/zgemv.h
  blas/zgerc.c blas/zgerc.h
  blas/zscal.c blas/zscal.h
  blas/zswap.c blas/zswap.h
  blas/ztrmm.c blas/ztrmm.h
  blas/ztrmv.c blas/ztrmv.h
  blas/ztrsv.c blas/ztrsv.h
  )
SET(V3P_NETLIB_linpack_SOURCES
  linpack/sqrdc.c linpack/sqrdc.h
  linpack/dqrdc.c linpack/dqrdc.h
  linpack/cqrdc.c linpack/cqrdc.h
  linpack/zqrdc.c linpack/zqrdc.h
  linpack/sqrsl.c linpack/sqrsl.h
  linpack/dqrsl.c linpack/dqrsl.h
  linpack/cqrsl.c linpack/cqrsl.h
  linpack/zqrsl.c linpack/zqrsl.h

  linpack/csvdc.c linpack/csvdc.h
  linpack/dsvdc.c linpack/dsvdc.h
  linpack/ssvdc.c linpack/ssvdc.h
  linpack/zsvdc.c linpack/zsvdc.h

  linpack/dpoco.c linpack/dpoco.h
  linpack/dpodi.c linpack/dpodi.h
  linpack/dpofa.c linpack/dpofa.h
  linpack/dposl.c linpack/dposl.h
  )
SET(V3P_NETLIB_temperton_SOURCES
  temperton/dgpfa.c temperton/dgpfa.h
  temperton/dgpfa2f.c temperton/dgpfa2f.h
  temperton/dgpfa3f.c temperton/dgpfa3f.h
  temperton/dgpfa5f.c temperton/dgpfa5f.h
  temperton/gpfa.c temperton/gpfa.h
  temperton/gpfa2f.c temperton/gpfa2f.h
  temperton/gpfa3f.c temperton/gpfa3f.h
  temperton/gpfa5f.c temperton/gpfa5f.h
  temperton/setdgpfa.c temperton/setdgpfa.h
  temperton/setgpfa.c temperton/setgpfa.h
  )
SET(V3P_NETLIB_eispack_SOURCES
  eispack/balanc.c eispack/balanc.h
  eispack/balbak.c eispack/balbak.h
  eispack/cdiv.c eispack/cdiv.h
  eispack/elmhes.c eispack/elmhes.h
  eispack/eltran.c eispack/eltran.h
  eispack/epslon.c eispack/epslon.h
  eispack/hqr.c eispack/hqr.h
  eispack/hqr2.c eispack/hqr2.h
  eispack/otqlrat.c eispack/otqlrat.h
  eispack/pythag.c eispack/pythag.h
  eispack/rebak.c eispack/rebak.h
  eispack/reduc.c eispack/reduc.h
  eispack/rg.c eispack/rg.h
  eispack/rs.c eispack/rs.h
  eispack/rsg.c eispack/rsg.h
  eispack/tql1.c eispack/tql1.h
  eispack/tql2.c eispack/tql2.h
  eispack/tred1.c eispack/tred1.h
  eispack/tred2.c eispack/tred2.h
  )
SET(V3P_NETLIB_laso_SOURCES
  laso/dlabax.c laso/dlabax.h
  laso/dlabcm.c laso/dlabcm.h
  laso/dlabfc.c laso/dlabfc.h
  laso/dlaeig.c laso/dlaeig.h
  laso/dlager.c laso/dlager.h
  laso/dlaran.c laso/dlaran.h
  laso/dmvpc.c laso/dmvpc.h
  laso/dnlaso.c laso/dnlaso.h
  laso/dnppla.c laso/dnppla.h
  laso/dortqr.c laso/dortqr.h
  laso/dvsort.c laso/dvsort.h
  laso/urand.c laso/urand.h
  )
SET(V3P_NETLIB_lapack_SOURCES
  lapack/complex16/zgebak.c lapack/complex16/zgebak.h
  lapack/complex16/zgebal.c lapack/complex16/zgebal.h
  lapack/complex16/zgeev.c lapack/complex16/zgeev.h
  lapack/complex16/zgehd2.c lapack/complex16/zgehd2.h
  lapack/complex16/zgehrd.c lapack/complex16/zgehrd.h
  lapack/complex16/zhseqr.c lapack/complex16/zhseqr.h
  lapack/complex16/zlacgv.c lapack/complex16/zlacgv.h
  lapack/complex16/zlacpy.c lapack/complex16/zlacpy.h
  lapack/complex16/zladiv.c lapack/complex16/zladiv.h
  lapack/complex16/zlahqr.c lapack/complex16/zlahqr.h
  lapack/complex16/zlahrd.c lapack/complex16/zlahrd.h
  lapack/complex16/zlange.c lapack/complex16/zlange.h
  lapack/complex16/zlanhs.c lapack/complex16/zlanhs.h
  lapack/complex16/zlarf.c lapack/complex16/zlarf.h
  lapack/complex16/zlarfb.c lapack/complex16/zlarfb.h
  lapack/complex16/zlarfg.c lapack/complex16/zlarfg.h
  lapack/complex16/zlarft.c lapack/complex16/zlarft.h
  lapack/complex16/zlarfx.c lapack/complex16/zlarfx.h
  lapack/complex16/zlascl.c lapack/complex16/zlascl.h
  lapack/complex16/zlaset.c lapack/complex16/zlaset.h
  lapack/complex16/zlassq.c lapack/complex16/zlassq.h
  lapack/complex16/zlatrs.c lapack/complex16/zlatrs.h
  lapack/complex16/ztrevc.c lapack/complex16/ztrevc.h
  lapack/complex16/zung2r.c lapack/complex16/zung2r.h
  lapack/complex16/zunghr.c lapack/complex16/zunghr.h
  lapack/complex16/zungqr.c lapack/complex16/zungqr.h
  lapack/double/dlabad.c lapack/double/dlabad.h
  lapack/double/dladiv.c lapack/double/dladiv.h
  lapack/double/dlapy2.c lapack/double/dlapy2.h
  lapack/double/dlapy3.c lapack/double/dlapy3.h
  lapack/util/ieeeck.c lapack/util/ieeeck.h
  lapack/util/ilaenv.c lapack/util/ilaenv.h
  lapack/util/lsame.c lapack/util/lsame.h
  )
SET(V3P_NETLIB_napack_SOURCES
  napack/cg.c napack/cg.h
  )
SET(V3P_NETLIB_minpack_SOURCES
  minpack/dpmpar.c minpack/dpmpar.h
  minpack/enorm.c minpack/enorm.h
  minpack/fdjac2.c minpack/fdjac2.h
  minpack/lmder.c minpack/lmder.h
  minpack/lmder1.c minpack/lmder1.h
  minpack/lmdif.c minpack/lmdif.h
  minpack/lmpar.c minpack/lmpar.h
  minpack/qrfac.c minpack/qrfac.h
  minpack/qrsolv.c minpack/qrsolv.h
  )
SET(V3P_NETLIB_opt_SOURCES
  opt/lbfgs.c opt/lbfgs.h
  )
SET(V3P_NETLIB_linalg_SOURCES
  linalg/lsqr.c linalg/lsqr.h
  )
SET(V3P_NETLIB_toms_SOURCES
  toms/rpoly.c toms/rpoly.h
  )

# Adjust the compiler flags to avoid problems with f2c-generated code.
IF(CMAKE_COMPILER_IS_GNUCC)
  # Hide warnings in f2c-generated code.
  #SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -w")

  # Remove all optimization flags.
  #Currently this is not needed but is here for reference if someone
  #finds a need to add it.
  #FOREACH(c "" "_DEBUG" "_RELEASE" "_MINSIZEREL" "_RELWITHDEBINFO")
  #  STRING(REGEX REPLACE
  #    "-O[^ ]*( |$)" "" CMAKE_C_FLAGS${c} ${CMAKE_C_FLAGS${c}})
  #ENDFOREACH(c)

  # Disable buggy optimization on some source files.
  #SET_SOURCE_FILES_PROPERTIES(
  #  temperton/gpfa5f.c
  #  temperton/dgpfa5f.c
  #  laso/dnlaso.c
  #  PROPERTIES COMPILE_FLAGS -O0
  #  )
ENDIF(CMAKE_COMPILER_IS_GNUCC)

# Create a netlib library with mangled symbols.
ADD_LIBRARY(v3p_netlib
  ${V3P_NETLIB_libf2c_SOURCES}
  ${V3P_NETLIB_blas_SOURCES}
  ${V3P_NETLIB_linpack_SOURCES}
  ${V3P_NETLIB_temperton_SOURCES}
  ${V3P_NETLIB_eispack_SOURCES}
  ${V3P_NETLIB_laso_SOURCES}
  ${V3P_NETLIB_lapack_SOURCES}
  ${V3P_NETLIB_napack_SOURCES}
  ${V3P_NETLIB_minpack_SOURCES}
  ${V3P_NETLIB_opt_SOURCES}
  ${V3P_NETLIB_linalg_SOURCES}
  ${V3P_NETLIB_toms_SOURCES}
  ${V3P_NETLIB_header_sources}
  )
IF(UNIX)
  TARGET_LINK_LIBRARIES( v3p_netlib m )
ENDIF(UNIX)
