# basic/bsta/io/CMakeLists.txt
# Brown statistics io Library.

SET(bsta_io_sources
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
#AUX_SOURCE_DIRECTORY(Templates bsta_io_sources)

ADD_LIBRARY(bsta_io  ${bsta_io_sources})
TARGET_LINK_LIBRARIES(bsta_io bsta vnl_io vbl_io vbl)

IF( BUILD_TESTING )
  SUBDIRS(tests)
ENDIF( BUILD_TESTING )

INSTALL_TARGETS(/lib bsta_io)
INSTALL_NOBASE_HEADER_FILES(/include/vxl/contrib/brl/bbas/bsta/io ${bsta_io_sources})
