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

add_library(bsta_io  ${bsta_io_sources})
target_link_libraries(bsta_io bsta vnl_io vbl_io vbl)

if( BUILD_TESTING )
  add_subdirectory(tests)
endif()

install_targets(/lib bsta_io)
INSTALL_NOBASE_HEADER_FILES(/include/vxl/contrib/brl/bbas/bsta/io ${bsta_io_sources})
