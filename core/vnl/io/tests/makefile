USES += vxl 
include ${IUEROOT}/vxl/vnl/io/config.mk
include ${IUEROOT}/vxl/testlib/config.mk

TESTS += vnl_io_test_all.cxx

# TESTS += golden_test_vnl_io.cxx
TESTS += test_vnl_io.cxx # includes test_vector_io.cxx test_real_polynomial_io.cxx test_real_npolynomial_io.cxx test_matrix_io.cxx test_diag_matrix_io.cxx test_matrix_fixed_io.cxx test_sparse_matrix_io.cxx test_vector_fixed_io.cxx test_nonlinear_minimizer_io.cxx golden_test_vnl_io.cxx

#######################################
include ${IUEROOT}/config/top-params.mk
#######################################

ifeq "$(TJ_GCC30)" "1"
FLAGS_test_vnl_io.cxx += -O0 # otherwise gcc 3.0 needs too much virtual memory
endif

#########################################
include ${IUEROOT}/config/static-tests.mk
#########################################
