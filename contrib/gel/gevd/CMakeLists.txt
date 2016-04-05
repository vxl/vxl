# gel/gevd/CMakeLists.txt

doxygen_add_library(contrib/gel/gevd
  DEPENDS contrib/gel/vtol core/vil1
  PACKAGE contrib/gel
  DESCRIPTION "Van-duc region finder"
  )

set(gevd_sources
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
    gevd_fold.cxx               gevd_fold.h
    gevd_contour.cxx            gevd_contour.h
    gevd_clean_edgels.cxx       gevd_clean_edgels.h
    gevd_edgel_regions.cxx      gevd_edgel_regions.h
    gevd_region_edge.cxx        gevd_region_edge.h
    gevd_region_proc.cxx        gevd_region_proc.h

    gevd_region_proc_params.cxx gevd_region_proc_params.h
)

if(CMAKE_COMPILER_IS_GNUCXX)
  set_source_files_properties(gevd_contour.cxx PROPERTIES COMPILE_FLAGS -O0)
endif()

vxl_add_library(LIBRARY_NAME gevd LIBRARY_SOURCES ${gevd_sources})
target_link_libraries(gevd vtol vsol vdgl ${VXL_LIB_PREFIX}vil1 ${VXL_LIB_PREFIX}vil ${VXL_LIB_PREFIX}vnl ${VXL_LIB_PREFIX}vgl ${VXL_LIB_PREFIX}vbl ${VXL_LIB_PREFIX}vul)

if(BUILD_TESTING)
  add_subdirectory(tests)
endif()
