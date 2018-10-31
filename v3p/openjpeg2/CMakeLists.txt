project(OpenJPEG C)

include(${VXL_CMAKE_DIR}/FindOpenJPEG2.cmake)

if(NOT VXL_USING_NATIVE_OPENJPEG2)

# Defines the source code for the library
set(OPENJPEG2_SRCS
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
add_definitions( ${OPENJPEG2_DEFINITIONS} )

# Pass proper definition to preprocessor to generate shared lib
vxl_add_library(LIBRARY_NAME openjpeg2 LIBRARY_SOURCES ${OPENJPEG2_SRCS})

find_library(VXL_MATH_LIBRARY_FOUND m)
if(VXL_MATH_LIBRARY_FOUND)
  # Math library is needed for many OpenJPEG functions.
  # The previous behaviour assumed the linker had m available thus,
  # didn't explicitly link it. On some systems though,
  # specifying the link is required.
  target_link_libraries(openjpeg2 m)
endif()
mark_as_advanced(VXL_MATH_LIBRARY_FOUND)
set_target_properties(openjpeg2 PROPERTIES
  VERSION       2.0.0
  DEFINE_SYMBOL OPJ_EXPORTS)
endif()

