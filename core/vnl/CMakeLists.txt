# ./vxl/vnl/CMakeLists.txt

ADD_LIBRARY(vnl vnl_sources)

SOURCE_FILES( vnl_sources
  vnl_alloc.cxx
  vnl_bignum.cxx
  vnl_block.cxx
  vnl_math.cxx
  vnl_copy.cxx
  vnl_complex.cxx
  vnl_error.cxx
  vnl_test.cxx
  vnl_matlab_print.cxx
  vnl_matlab_print_format.cxx
  vnl_matlab_print_scalar.cxx
  vnl_matlab_print2.cxx
  vnl_matlab_header.cxx
  vnl_matlab_write.cxx
  vnl_matlab_read.cxx
  vnl_matlab_filewrite.cxx
  vnl_matops.cxx
  vnl_real_polynomial.cxx
  vnl_real_npolynomial.cxx
#vnl_binary # deprecated: use vsl instead
  vnl_fastops.cxx
  vnl_linear_operators_3.cxx
  vnl_numeric_limits.cxx
  vnl_numeric_traits.cxx
  vnl_complex_traits.cxx
  vnl_int_matrix.cxx
  vnl_int_2.cxx
  vnl_int_3.cxx
  vnl_int_4.cxx
  vnl_float_2.cxx
  vnl_float_3.cxx
  vnl_float_4.cxx
  vnl_double_2.cxx
  vnl_double_3.cxx
  vnl_double_4.cxx
  vnl_double_2x3.cxx
  vnl_double_3x2.cxx
  vnl_cost_function.cxx
  vnl_least_squares_function.cxx
  vnl_least_squares_cost_function.cxx
  vnl_nonlinear_minimizer.cxx
  vnl_linear_system.cxx
  vnl_sparse_matrix_linear_system.cxx
  vnl_rotation_matrix.cxx
  vnl_cross_product_matrix.cxx
  vnl_identity_3x3.cxx
  vnl_trace.cxx
  vnl_sample.cxx
  vnl_unary_function.cxx
  vnl_transpose.cxx
  vnl_rational.cxx
)

AUX_SOURCE_DIRECTORY(Templates vnl_sources)

# VNL_ALLOC is a fast but currently thread-unsafe allocator
# used in vnl_vectors. The default is to use it but to  
# use the slower, thread-safe allocation, in your 
# CMakeListsLocal.txt file add the following lines 
# SET(VNL_C_VECTOR_USE_VNL_ALLOC_DEFINED 1)
# ADD_DEFINITIONS(-DVNL_C_VECTOR_USE_VNL_ALLOC=0)

IF(NOT VNL_C_VECTOR_USE_VNL_ALLOC_DEFINED)
  ADD_DEFINITIONS(-DVNL_C_VECTOR_USE_VNL_ALLOC=1)
ENDIF(NOT VNL_C_VECTOR_USE_VNL_ALLOC_DEFINED)



IF (WIN32)
  IF (NOT CYGWIN)
    INCLUDE(${allvxl_SOURCE_DIR}/vxl/vnl/CMakeListsHeaders.txt)
  ENDIF (NOT CYGWIN)
ENDIF(WIN32)

SUBDIRS(algo)
SUBDIRS(io)

IF( BUILD_EXAMPLES )
  SUBDIRS( examples )
ENDIF( BUILD_EXAMPLES )

IF( BUILD_TESTING )
  SUBDIRS(tests)
ENDIF( BUILD_TESTING )
