# ./tbl/vepl/CMakeLists.txt

ADD_LIBRARY(vepl vepl_sources)

SOURCE_FILES(vepl_sources
# vepl_convert.txx
  vepl_threshold.cxx
  vepl_add_random_noise.cxx
  vepl_monadic.cxx
  vepl_dyadic.cxx
  vepl_erode_disk.cxx
  vepl_dilate_disk.cxx
  vepl_sobel.cxx
  vepl_gaussian_convolution.cxx
  vepl_x_gradient.cxx
  vepl_y_gradient.cxx
  vepl_gradient_mag.cxx
  vepl_gradient_dir.cxx
  vepl_median.cxx
  vepl_moment.cxx
  vepl_histogram.cxx
)

AUX_SOURCE_DIRECTORY(Templates vepl_sources)

IF (WIN32)
  IF (NOT CYGWIN)
    INCLUDE(${allvxl_SOURCE_DIR}/tbl/vepl/CMakeListsHeaders.txt)
  ENDIF (NOT CYGWIN)
ENDIF(WIN32)

IF( BUILD_EXAMPLES )
  SUBDIRS( examples )
ENDIF( BUILD_EXAMPLES )

IF( BUILD_TESTING )
  SUBDIRS(tests)
ENDIF( BUILD_TESTING )
