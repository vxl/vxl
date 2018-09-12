# mul/vimt/algo/CMakeLists.txt

set( vimt_algo_sources
  vimt_correlate_2d.h
  vimt_find_peaks.h
  vimt_find_troughs.h
  vimt_normalised_correlation_2d.h
  vimt_pyramid_values_at_point.h
  # This file merely forces the creation of an actual library
  vimt_dummy.cxx
)

vxl_add_library(LIBRARY_NAME vimt_algo LIBRARY_SOURCES ${vimt_algo_sources})
target_link_libraries( vimt_algo vimt ${VXL_LIB_PREFIX}vil_algo ${VXL_LIB_PREFIX}vgl ${VXL_LIB_PREFIX}vcl )

if( BUILD_TESTING )
  add_subdirectory(tests)
endif()

if (VXL_BUILD_OBJECT_LIBRARIES)
  add_library(vimt_algo-obj OBJECT ${vimt_algo_sources})
endif()
