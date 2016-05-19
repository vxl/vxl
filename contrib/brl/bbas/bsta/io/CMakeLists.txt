# basic/bsta/io/CMakeLists.txt
# Brown statistics io Library.

set(bsta_io_sources
  bsta_io_mixture.h
  bsta_io_mixture_fixed.h
  bsta_io_attributes.h
  bsta_io_gaussian_full.h
  bsta_io_gaussian_indep.h
  bsta_io_gaussian_sphere.h
  bsta_io_parzen_sphere.h
  bsta_io_von_mises.h
  bsta_io_histogram.h bsta_io_histogram.cxx
  bsta_io_beta.h
)
#aux_source_directory(Templates bsta_io_sources)

vxl_add_library(LIBRARY_NAME bsta_io LIBRARY_SOURCES  ${bsta_io_sources})
target_link_libraries(bsta_io bsta ${VXL_LIB_PREFIX}vnl_io ${VXL_LIB_PREFIX}vbl_io ${VXL_LIB_PREFIX}vbl)

if( BUILD_TESTING )
  add_subdirectory(tests)
endif()