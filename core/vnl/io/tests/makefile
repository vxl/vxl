USES += core
include ../config.mk
include ${IUEROOT}/core/testlib/config.mk

TESTMAIN = test_driver.cxx
TESTS += golden_test_vnl_io.cxx
TESTS += test_bignum_io.cxx
TESTS += test_diag_matrix_io.cxx
TESTS += test_include.cxx
TESTS += test_matrix_fixed_io.cxx
TESTS += test_matrix_io.cxx
TESTS += test_nonlinear_minimizer_io.cxx
TESTS += test_rational_io.cxx
TESTS += test_real_npolynomial_io.cxx
TESTS += test_real_polynomial_io.cxx
TESTS += test_sparse_matrix_io.cxx
TESTS += test_sym_matrix_io.cxx
TESTS += test_vector_fixed_io.cxx
TESTS += test_vector_io.cxx

################################
include ${IUEROOT}/config/top.mk
################################

ifeq "$(TJ_GCC30)" "1"# otherwise gcc 3.0 needs too much virtual memory
FLAGS_test_vnl_io.cxx := -O0
endif
