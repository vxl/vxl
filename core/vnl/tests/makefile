USES := vxl
include ${IUEROOT}/vxl/vnl/algo/config.mk
include ${IUEROOT}/vxl/vul/config.mk # vul_timer used
include ${IUEROOT}/vxl/vpl/config.mk # vpl_unlink used in test_matlab
include ${IUEROOT}/vxl/testlib/config.mk

TESTMAIN = test_driver.cxx
SOURCES += test_util.cxx
TESTS += test_include.cxx
TESTS += test_fft.cxx
TESTS += test_fft1d.cxx
TESTS += test_fft2d.cxx
TESTS += test_matlab.cxx
TESTS += test_math.cxx
TESTS += test_vector.cxx
TESTS += test_resize.cxx
TESTS += test_matrix.cxx
TESTS += test_matrix_fixed.cxx
TESTS += test_file_matrix.cxx
TESTS += test_rnpoly_roots.cxx
TESTS += test_sparse_matrix.cxx
TESTS += test_qsvd.cxx
TESTS += test_sample.cxx
TESTS += test_functions.cxx
TESTS += test_numeric_limits.cxx
TESTS += test_numeric_traits.cxx
TESTS += test_rpoly_roots.cxx
TESTS += test_cpoly_roots.cxx
TESTS += test_complex.cxx
TESTS += test_determinant.cxx
TESTS += test_qr.cxx
TESTS += test_svd.cxx
TESTS += test_real_eigensystem.cxx
TESTS += test_complex_eigensystem.cxx
TESTS += test_symmetric_eigensystem.cxx
TESTS += test_generalized_eigensystem.cxx
TESTS += test_matrix_exp.cxx
TESTS += test_transpose.cxx
TESTS += test_levenberg_marquardt.cxx
#TESTS += $(wildcard test_tensor.cxx)
TESTS += test_minimizers.cxx
TESTS += test_rational.cxx
TESTS += test_convolve.cxx
TESTS += test_real_polynomial.cxx
TESTS += test_sym_matrix.cxx
TESTS += test_amoeba.cxx
TESTS += test_bignum.cxx
TESTS += test_finite.cxx
TESTS += test_diag_matrix.cxx

#######################################
include ${IUEROOT}/config/top-params.mk
#######################################

ifeq "$(TJ_GCC30)" "1"
# gcc 3.0 needs too much virtual memory with optimisation
FLAGS_test_rational.cxx += -O1
FLAGS_test_bignum.cxx += -O1
FLAGS_test_finite.cxx += -O0
endif

#########################################
include ${IUEROOT}/config/static-tests.mk
#########################################
