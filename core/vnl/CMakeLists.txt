#
# vxl/vnl/CMakeLists.txt
#
#   geoff.cross@crd.ge.com
#

ADD_LIBRARY(vnl vnl_sources)

SOURCE_FILES( vnl_sources
vnl_alloc
vnl_block
vnl_math
vnl_copy
vnl_complex
vnl_error
vnl_test
vnl_matlab_print
vnl_matlab_print_format
vnl_matlab_print_scalar
vnl_matlab_print2
vnl_matlab_header
vnl_matlab_write
vnl_matlab_read
vnl_matlab_filewrite
vnl_matops
vnl_real_polynomial
vnl_real_npolynomial
#vnl_binary # deprecated: use vsl instead
vnl_fastops
vnl_linear_operators_3
vnl_numeric_limits
vnl_numeric_traits
vnl_complex_traits
vnl_int_matrix
vnl_int_2
vnl_int_3
vnl_int_4
vnl_float_2
vnl_float_3
vnl_float_4
vnl_double_2
vnl_double_3
vnl_double_4
vnl_double_2x3
vnl_double_3x2
vnl_cost_function
vnl_least_squares_function
vnl_least_squares_cost_function
vnl_nonlinear_minimizer
vnl_linear_system
vnl_sparse_matrix_linear_system
vnl_rotation_matrix
vnl_cross_product_matrix
vnl_identity_3x3
vnl_trace
vnl_sample
vnl_unary_function
vnl_transpose
)

AUX_SOURCE_DIRECTORY(Templates vnl_sources)

SUBDIRS(algo)

IF( BUILD_TESTS)
  SUBDIRS(tests)
ENDIF( BUILD_TESTS)
