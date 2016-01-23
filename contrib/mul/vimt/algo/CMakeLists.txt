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

add_library(vimt_algo ${vimt_algo_sources})
install_targets(/lib vimt_algo)
INSTALL_NOBASE_HEADER_FILES(/include/vxl/contrib/mul/vimt/algo ${vimt_algo_sources})
target_link_libraries( vimt_algo vimt vil_algo vgl vcl )

if( BUILD_TESTING )
  add_subdirectory(tests)
endif()
