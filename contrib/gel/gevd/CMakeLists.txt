# ./gel/gevd/CMakeLists.txt
#

ADD_LIBRARY(gevd gevd_sources)

SOURCE_FILES( gevd_sources
  gevd_detector
  gevd_detector_params
  gevd_param_mixin
  gevd_status_mixin
  gevd_memory_mixin
  gevd_bufferxy
  gevd_float_operators
  gevd_noise
  gevd_step
  gevd_contour
)
