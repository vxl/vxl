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
  sqrdc.c
  dqrdc.c
  cqrdc.c
  zqrdc.c
  sqrsl.c
  dqrsl.c
  zqrsl.c
  cqrsl.c
  dpofa.c
  dpoco.c
  dposl.c
  dpodi.c
  isamax.c
  idamax.c

  # EISPACK
  rsg.c
  rg.c
  rs.c
  tql1.c
  tql2.c
  tred1.c
  tred2.c

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

  # Temperton FFT routines
  dgpfa.c
  dgpfa3f.c
  dsetgpfa.c
  gpfa2f.c
  gpfa5f.c
  dgpfa2f.c
  dgpfa5f.c
  gpfa.c
  gpfa3f.c
  setgpfa.c

  # NAPACK
  cg.c

  # from DATAPAC
  camsun.c

  # from LASO2
  dnlaso.c
  snlaso.c

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
  zgeev.c
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
  SET_SOURCE_FILES_PROPERTIES(gpfa5f.c PROPERTIES COMPILE_FLAGS -O0)
  SET_SOURCE_FILES_PROPERTIES(dgpfa5f.c PROPERTIES COMPILE_FLAGS -O0)
  SET_SOURCE_FILES_PROPERTIES(zlarfx.c PROPERTIES COMPILE_FLAGS -Os)
  SET_SOURCE_FILES_PROPERTIES(rg.c PROPERTIES COMPILE_FLAGS -O1)
  SET_SOURCE_FILES_PROPERTIES(dnlaso.c PROPERTIES COMPILE_FLAGS -O0)
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
  blas/cdotc.c
  blas/cscal.c
  blas/csrot.c
  blas/cswap.c
  blas/daxpy.c
  blas/dcabs1.c
  blas/ddot.c
  blas/dnrm2.c
  blas/drot.c
  blas/drotg.c
  blas/dscal.c
  blas/dswap.c
  blas/dznrm2.c
  blas/saxpy.c
  blas/scnrm2.c
  blas/sdot.c
  blas/snrm2.c
  blas/srot.c
  blas/srotg.c
  blas/sscal.c
  blas/sswap.c
  blas/zaxpy.c
  blas/zdotc.c
  blas/zdrot.c
  blas/zscal.c
  blas/zswap.c
  )
SET(V3P_NETLIB_linpack_SOURCES
  linpack/sqrdc.c
  linpack/dqrdc.c
  linpack/cqrdc.c
  linpack/zqrdc.c

  linpack/csvdc.c
  linpack/dsvdc.c
  linpack/ssvdc.c
  linpack/zsvdc.c
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
ENDIF(CMAKE_COMPILER_IS_GNUCC)

# Create a netlib library with mangled symbols.
ADD_LIBRARY(v3p_netlib
  ${V3P_NETLIB_libf2c_SOURCES}
  ${V3P_NETLIB_blas_SOURCES}
  ${V3P_NETLIB_linpack_SOURCES}
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
