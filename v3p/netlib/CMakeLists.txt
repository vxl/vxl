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

  # LINALG
  lsqr.c

  # LINPACK
  isamax.c
  idamax.c

  # ITPACK
  dsrc2c.c

  # MINPACK
  enorm.c
  fdjac2.c
  lmpar.c
  qrfac.c
  qrsolv.c
  lmdif.c
  lmder.c
  lmder1.c
  pythag.c
  rpoly.c

  # TOMS Misc
  dtrans.c
  trans.c

  # General
  machineparams.c

  # NAPACK
  cg.c

  # from DATAPAC
  camsun.c

  # from OPT
  lbfgs.c lbfgs.h
  lbfgs-lb1.c

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
  triangle.h triangle.c

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
  blas/caxpy.c
  blas/ccopy.c
  blas/cdotc.c
  blas/cscal.c
  blas/csrot.c
  blas/cswap.c
  blas/dasum.c
  blas/daxpy.c
  blas/dcabs1.c
  blas/dcopy.c
  blas/ddot.c
  blas/dlamch.c
  blas/dnrm2.c
  blas/drot.c
  blas/drotg.c
  blas/dscal.c
  blas/dswap.c
  blas/dzasum.c
  blas/dznrm2.c
  blas/idamax.c
  blas/izamax.c
  blas/saxpy.c
  blas/scnrm2.c
  blas/scopy.c
  blas/sdot.c
  blas/snrm2.c
  blas/srot.c
  blas/srotg.c
  blas/sscal.c
  blas/sswap.c
  blas/xerbla.c
  blas/zaxpy.c
  blas/zcopy.c
  blas/zdotc.c
  blas/zdotu.c
  blas/zdrot.c
  blas/zdscal.c
  blas/zgemm.c
  blas/zgemv.c
  blas/zgerc.c
  blas/zscal.c
  blas/zswap.c
  blas/ztrmm.c
  blas/ztrmv.c
  blas/ztrsv.c
  )
SET(V3P_NETLIB_linpack_SOURCES
  linpack/sqrdc.c
  linpack/dqrdc.c
  linpack/cqrdc.c
  linpack/zqrdc.c
  linpack/sqrsl.c
  linpack/dqrsl.c
  linpack/cqrsl.c
  linpack/zqrsl.c

  linpack/csvdc.c
  linpack/dsvdc.c
  linpack/ssvdc.c
  linpack/zsvdc.c

  linpack/dpoco.c
  linpack/dpodi.c
  linpack/dpofa.c
  linpack/dposl.c
  )
SET(V3P_NETLIB_temperton_SOURCES
  temperton/dgpfa.c
  temperton/dgpfa2f.c
  temperton/dgpfa3f.c
  temperton/dgpfa5f.c
  temperton/gpfa.c
  temperton/gpfa2f.c
  temperton/gpfa3f.c
  temperton/gpfa5f.c
  temperton/setdgpfa.c
  temperton/setgpfa.c
  )
SET(V3P_NETLIB_eispack_SOURCES
  eispack/balanc.c
  eispack/balbak.c
  eispack/cdiv.c
  eispack/elmhes.c
  eispack/eltran.c
  eispack/epslon.c
  eispack/hqr.c
  eispack/hqr2.c
  eispack/otqlrat.c
  eispack/pythag.c
  eispack/rebak.c
  eispack/reduc.c
  eispack/rg.c
  eispack/rs.c
  eispack/rsg.c
  eispack/tql1.c
  eispack/tql2.c
  eispack/tred1.c
  eispack/tred2.c
  )
SET(V3P_NETLIB_laso_SOURCES
  laso/dlabax.c
  laso/dlabcm.c
  laso/dlabfc.c
  laso/dlaeig.c
  laso/dlager.c
  laso/dlaran.c
  laso/dmvpc.c
  laso/dnlaso.c
  laso/dnppla.c
  laso/dortqr.c
  laso/dvsort.c
  laso/urand.c
  )
SET(V3P_NETLIB_lapack_SOURCES
  lapack/complex16/zgebak.c
  lapack/complex16/zgebal.c
  lapack/complex16/zgeev.c
  lapack/complex16/zgehd2.c
  lapack/complex16/zgehrd.c
  lapack/complex16/zhseqr.c
  lapack/complex16/zlacgv.c
  lapack/complex16/zlacpy.c
  lapack/complex16/zladiv.c
  lapack/complex16/zlahqr.c
  lapack/complex16/zlahrd.c
  lapack/complex16/zlange.c
  lapack/complex16/zlanhs.c
  lapack/complex16/zlarf.c
  lapack/complex16/zlarfb.c
  lapack/complex16/zlarfg.c
  lapack/complex16/zlarft.c
  lapack/complex16/zlarfx.c
  lapack/complex16/zlascl.c
  lapack/complex16/zlaset.c
  lapack/complex16/zlassq.c
  lapack/complex16/zlatrs.c
  lapack/complex16/ztrevc.c
  lapack/complex16/zung2r.c
  lapack/complex16/zunghr.c
  lapack/complex16/zungqr.c
  lapack/double/dlabad.c
  lapack/double/dladiv.c
  lapack/double/dlapy2.c
  lapack/double/dlapy3.c
  lapack/util/ieeeck.c
  lapack/util/ilaenv.c
  lapack/util/lsame.c
  )
SET(V3P_NETLIB_napack_SOURCES
  napack/cg.c
  )
SET(V3P_NETLIB_minpack_SOURCES
  minpack/dpmpar.c
  minpack/enorm.c
  minpack/fdjac2.c
  minpack/lmder.c
  minpack/lmder1.c
  minpack/lmdif.c
  minpack/lmpar.c
  minpack/qrfac.c
  minpack/qrsolv.c
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
  v3p_f2c.h
  v3p_f2c_mangle.h
  v3p_f2c_original.h
  v3p_f2c_unmangle.h
  v3p_netlib.h
  v3p_netlib_mangle.h
  v3p_netlib_prototypes.h
  v3p_netlib_unmangle.h
  )
IF(UNIX)
  TARGET_LINK_LIBRARIES( v3p_netlib m )
ENDIF(UNIX)
