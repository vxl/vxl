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
  bsta_io_histogram.h
  bsta_io_beta.h
  dummy.cxx  # remove when other source files are added
)

# This directory currently contains only headers with template code
# so there is no library to be built.  If cxx files or template
# instantiations are added then the library targets below
# should be uncommented

ADD_LIBRARY(bsta_io  ${bsta_io_sources})
TARGET_LINK_LIBRARIES(bsta_io bsta vnl_io vbl_io vbl)

IF( BUILD_TESTING )
  SUBDIRS(tests)
ENDIF( BUILD_TESTING )
