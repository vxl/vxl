# ./gel/vdgl/CMakeLists.txt

ADD_LIBRARY(vdgl vdgl_sources)

SOURCE_FILES(vdgl_sources
  vdgl_OrthogRegress.cxx
  vdgl_digital_curve.cxx
  vdgl_digital_region.cxx
  vdgl_intensity_face.cxx
  vdgl_edgel.cxx
  vdgl_edgel_chain.cxx
  vdgl_fit_line.cxx
  vdgl_interpolator.cxx
  vdgl_interpolator_cubic.cxx
  vdgl_interpolator_linear.cxx
  vdgl_ortho_regress.cxx
)

AUX_SOURCE_DIRECTORY(Templates vdgl_sources)

IF (WIN32)
  IF (NOT CYGWIN)
    INCLUDE(${allvxl_SOURCE_DIR}/gel/vdgl/CMakeListsHeaders.txt)
  ENDIF (NOT CYGWIN)
ENDIF(WIN32)

IF( BUILD_EXAMPLES )
  SUBDIRS( examples )
ENDIF( BUILD_EXAMPLES )

IF( BUILD_TESTING )
  SUBDIRS(tests)
ENDIF( BUILD_TESTING )
