# ./vxl/vnl/io/CMakeLists.txt

ADD_LIBRARY(vnl_io vnl_io_sources )

SOURCE_FILES(vnl_io_sources
  vnl_io_nonlinear_minimizer.cxx
  vnl_io_real_npolynomial.cxx
  vnl_io_real_polynomial.cxx
  vnl_io_rational.cxx
  vnl_io_bignum.cxx
)

AUX_SOURCE_DIRECTORY(Templates vnl_io_sources)

IF (WIN32)
  IF (NOT CYGWIN)
    INCLUDE(${allvxl_SOURCE_DIR}/vxl/vnl/io/CMakeListsHeaders.txt)
  ENDIF (NOT CYGWIN)
ENDIF(WIN32)

IF( BUILD_TESTING)
  SUBDIRS(tests)
ENDIF( BUILD_TESTING)
