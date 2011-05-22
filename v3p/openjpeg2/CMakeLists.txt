PROJECT(OpenJPEG C)

INCLUDE(${VXL_CMAKE_DIR}/FindOpenJPEG2.cmake)

IF(NOT VXL_USING_NATIVE_OPENJPEG2)

# Defines the source code for the library
SET(OPENJPEG2_SRCS
  bio.h            bio.c
  cio.h            cio.c
  dwt.h            dwt.c
  event.h          event.c
  fix.h
  function_list.h  function_list.c
  image.h          image.c
  int.h
  invert.h         invert.c
  j2k.h            j2k.c
  j2k_lib.h        j2k_lib.c
  jp2.h            jp2.c
  jpt.h            jpt.c
  mct.h            mct.c
  mqc.h            mqc.c
  openjpeg.h       openjpeg.c
  opj_configure.h
  opj_includes.h
  opj_malloc.h
  pi.h             pi.c
  profile.h        profile.c
  raw.h            raw.c
  t1.h             t1.c
  t1_luts.h
  t2.h             t2.c
  tcd.h            tcd.c
  tgt.h            tgt.c
)
ADD_DEFINITIONS( ${OPENJPEG2_DEFINITIONS} )

# Pass proper definition to preprocessor to generate shared lib
ADD_LIBRARY(openjpeg2 ${OPENJPEG2_SRCS})
SET_TARGET_PROPERTIES(openjpeg2 PROPERTIES
                      VERSION       2.0.0
                      DEFINE_SYMBOL OPJ_EXPORTS)

# Install library
INSTALL_TARGETS(/lib openjpeg2)

ENDIF(NOT VXL_USING_NATIVE_OPENJPEG2)

