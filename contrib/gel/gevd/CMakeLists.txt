# gel/gevd/CMakeLists.txt

SET(gevd_sources
                                gevd_pixel.h
                                gevd_xpixel.h
    gevd_detector.cxx           gevd_detector.h
    gevd_detector_params.cxx    gevd_detector_params.h
    gevd_param_mixin.cxx        gevd_param_mixin.h
                                gevd_status_mixin.h
    gevd_memory_mixin.cxx       gevd_memory_mixin.h
    gevd_bufferxy.cxx           gevd_bufferxy.h
    gevd_float_operators.cxx    gevd_float_operators.h
    gevd_noise.cxx              gevd_noise.h
    gevd_step.cxx               gevd_step.h
    gevd_contour.cxx            gevd_contour.h
    gevd_clean_edgels.cxx       gevd_clean_edgels.h
    gevd_edgel_regions.cxx      gevd_edgel_regions.h
    gevd_region_edge.cxx        gevd_region_edge.h
    gevd_region_proc.cxx        gevd_region_proc.h

    gevd_region_proc_params.cxx gevd_region_proc_params.h
)

IF(CMAKE_COMPILER_IS_GNUCXX)
  SET_SOURCE_FILES_PROPERTIES(gevd_contour.cxx PROPERTIES COMPILE_FLAGS -O0)
ENDIF(CMAKE_COMPILER_IS_GNUCXX)

ADD_LIBRARY(gevd ${gevd_sources})
TARGET_LINK_LIBRARIES(gevd vtol vsol vdgl vil1 vnl vbl)

IF (BUILD_TESTING)
  SUBDIRS(tests)
ENDIF (BUILD_TESTING)
