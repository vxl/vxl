#
# ./vxl/vnl/io/CMakeLists.txt
#
#   Ian Scott
#

ADD_LIBRARY(vnl_io vnl_io_sources )

SOURCE_FILES(vnl_io_sources
vnl_io_nonlinear_minimizer
vnl_io_real_npolynomial
vnl_io_real_polynomial
vnl_io_rational
)

AUX_SOURCE_DIRECTORY(Templates vnl_io_sources )

IF( BUILD_TESTS)
  SUBDIRS(tests)
ENDIF( BUILD_TESTS)
