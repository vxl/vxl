# ./gel/gevd/CMakeLists.txt

ADD_LIBRARY(gevd gevd_sources)

SOURCE_FILES( gevd_sources
  gevd_detector.cxx
  gevd_detector_params.cxx
  gevd_param_mixin.cxx
  gevd_status_mixin.cxx
  gevd_memory_mixin.cxx
  gevd_bufferxy.cxx
  gevd_float_operators.cxx
  gevd_noise.cxx
  gevd_step.cxx
  gevd_contour.cxx
)

IF (WIN32)
  IF (NOT CYGWIN)
    INCLUDE(${allvxl_SOURCE_DIR}/gel/gevd/CMakeListsHeaders.txt)
  ENDIF (NOT CYGWIN)
ENDIF(WIN32)
