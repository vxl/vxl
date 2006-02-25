USES += core
include ../config.mk
include ${VXLROOT}/core/vul/config.mk # vul_timer used
include ${VXLROOT}/core/vpl/config.mk # vpl_unlink used in test_matlab
include ${VXLROOT}/core/testlib/config.mk

TESTMAIN = test_driver.cxx
TESTS += test_matlab.cxx
TESTS += test_math.cxx
TESTS += test_vector.cxx
TESTS += test_resize.cxx
TESTS += test_matrix.cxx
TESTS += test_matrix_fixed.cxx
TESTS += test_vector_fixed_ref.cxx
TESTS += test_matrix_fixed_ref.cxx
TESTS += test_file_matrix.cxx
TESTS += test_sample.cxx
TESTS += test_random.cxx
TESTS += test_numeric_traits.cxx
TESTS += test_complex.cxx
TESTS += test_inverse.cxx
TESTS += test_matrix_exp.cxx
TESTS += test_transpose.cxx
TESTS += test_fastops.cxx
TESTS += test_rational.cxx
TESTS += test_real_polynomial.cxx
TESTS += test_sym_matrix.cxx
TESTS += test_bignum.cxx
TESTS += test_finite.cxx
TESTS += test_diag_matrix.cxx
TESTS += test_gamma.cxx
TESTS += test_arithmetic.cxx
TESTS += test_hungarian_algorithm.cxx
TESTS += test_integrant.cxx
TESTS += test_bessel.cxx
TESTS += test_quaternion.cxx

MANPAGE_SOURCES += test_arithmetic_body.h

MINI_PROG_SOURCES += basic_operation_timings.cxx

MINI_PROG_SOURCES += test_include.cxx

#########################################
include ${VXLROOT}/config/static-tests.mk
#########################################

ifeq "$(TJ_GCC30)" "1"
# gcc 3.0 needs too much virtual memory with optimisation
FLAGS_test_rational.cxx := -O1
FLAGS_test_bignum.cxx := -O3
FLAGS_test_finite.cxx := -O0
endif
