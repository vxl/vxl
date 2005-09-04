USES += core
LIBRARY := vnl
VERSION := 1.0
DEFINES += -DBUILDING_VNL_DLL

MANPAGE_SOURCES += vnl_fwd.h
MANPAGE_SOURCES += vnl_config.h.in vnl_config.h
MANPAGE_SOURCES += vnl_tag.h

# vector and matrix
MANPAGE_SOURCES += vnl_c_vector.txx
MANPAGE_SOURCES += vnl_vector.txx
MANPAGE_SOURCES += vnl_vector_ref.h
MANPAGE_SOURCES += vnl_vector_fixed.txx
MANPAGE_SOURCES += vnl_vector_fixed_ref.txx
MANPAGE_SOURCES += vnl_file_vector.txx
MANPAGE_SOURCES += vnl_matrix.txx
MANPAGE_SOURCES += vnl_matrix_ref.h
MANPAGE_SOURCES += vnl_matrix_fixed.txx
MANPAGE_SOURCES += vnl_matrix_fixed_ref.txx
MANPAGE_SOURCES += vnl_diag_matrix.txx
MANPAGE_SOURCES += vnl_sparse_matrix.txx
MANPAGE_SOURCES += vnl_matrix_exp.txx
MANPAGE_SOURCES += vnl_file_matrix.txx
MANPAGE_SOURCES += vnl_sym_matrix.txx
# matrix operators
MANPAGE_SOURCES += vnl_fortran_copy.txx
MANPAGE_SOURCES += vnl_det.txx
MANPAGE_SOURCES += vnl_transpose.h
MANPAGE_SOURCES += vnl_inverse.h
MANPAGE_SOURCES += vnl_trace.h
MANPAGE_SOURCES += vnl_rank.txx
MANPAGE_SOURCES += vnl_scalar_join_iterator.txx

#
SOURCES += vnl_alloc.cxx
SOURCES += vnl_block.cxx
SOURCES += vnl_math.cxx
SOURCES += vnl_copy.cxx
MANPAGE_SOURCES += vnl_complex.h
SOURCES += vnl_error.cxx
MANPAGE_SOURCES += vnl_matlab_print.txx
SOURCES += vnl_matlab_print_format.cxx
SOURCES += vnl_matlab_print_scalar.cxx
MANPAGE_SOURCES += vnl_matlab_print2.h
MANPAGE_SOURCES += vnl_matlab_header.h
SOURCES += vnl_matlab_write.cxx
SOURCES += vnl_matlab_read.cxx
SOURCES += vnl_matlab_filewrite.cxx
SOURCES += vnl_matops.cxx
SOURCES += vnl_real_polynomial.cxx
SOURCES += vnl_real_npolynomial.cxx

# alternative number representations

# Quaternions
MANPAGE_SOURCES += vnl_quaternion.txx
# Rational number arithmetic; represented by two long integers
SOURCES += vnl_rational.cxx
# Arbitrary precision integers
SOURCES += vnl_bignum.cxx
# Finite field and finite ring numbers and arithmetic
MANPAGE_SOURCES += vnl_finite.h
# p-adic numbers are actually a limit case in n of GF(p^n): cannot implement
#MANPAGE_SOURCES += vnl_padic_field_number.txx

# ops
SOURCES += vnl_fastops.cxx
MANPAGE_SOURCES += vnl_operators.h
MANPAGE_SOURCES += vnl_linear_operators_3.h
MANPAGE_SOURCES += vnl_complex_ops.txx vnl_complexify.h vnl_imag.h vnl_real.h

# traits
SOURCES += vnl_numeric_traits.cxx
MANPAGE_SOURCES += vnl_complex_traits.h
SOURCES += vnl_bignum_traits.cxx
SOURCES += vnl_rational_traits.cxx

# matrix and vector specialisations
SOURCES += vnl_int_matrix.cxx     # ?
MANPAGE_SOURCES += vnl_T_n.h
MANPAGE_SOURCES += vnl_int_2.h
MANPAGE_SOURCES += vnl_int_3.h
MANPAGE_SOURCES += vnl_int_4.h
MANPAGE_SOURCES += vnl_float_2.h
MANPAGE_SOURCES += vnl_float_3.h
MANPAGE_SOURCES += vnl_float_4.h
MANPAGE_SOURCES += vnl_double_2.h
MANPAGE_SOURCES += vnl_double_3.h
MANPAGE_SOURCES += vnl_double_4.h

MANPAGE_SOURCES += vnl_int_1x1.h
MANPAGE_SOURCES += vnl_int_2x2.h
MANPAGE_SOURCES += vnl_float_1x1.h
MANPAGE_SOURCES += vnl_float_1x2.h
MANPAGE_SOURCES += vnl_float_2x1.h
MANPAGE_SOURCES += vnl_float_2x2.h
MANPAGE_SOURCES += vnl_float_1x3.h
MANPAGE_SOURCES += vnl_float_3x1.h
MANPAGE_SOURCES += vnl_float_3x3.h
MANPAGE_SOURCES += vnl_float_3x4.h
MANPAGE_SOURCES += vnl_float_4x3.h
MANPAGE_SOURCES += vnl_float_4x4.h
MANPAGE_SOURCES += vnl_double_1x1.h
MANPAGE_SOURCES += vnl_double_1x2.h
MANPAGE_SOURCES += vnl_double_2x1.h
MANPAGE_SOURCES += vnl_double_2x2.h
MANPAGE_SOURCES += vnl_double_1x3.h
MANPAGE_SOURCES += vnl_double_3x1.h
MANPAGE_SOURCES += vnl_double_2x3.h
MANPAGE_SOURCES += vnl_double_3x2.h
MANPAGE_SOURCES += vnl_double_3x3.h
MANPAGE_SOURCES += vnl_double_3x4.h
MANPAGE_SOURCES += vnl_double_4x3.h
MANPAGE_SOURCES += vnl_double_4x4.h

# optimization
SOURCES += vnl_cost_function.cxx
SOURCES += vnl_least_squares_function.cxx
SOURCES += vnl_least_squares_cost_function.cxx
SOURCES += vnl_nonlinear_minimizer.cxx
SOURCES += vnl_hungarian_algorithm.cxx

# linear systems
SOURCES += vnl_linear_system.cxx
SOURCES += vnl_sparse_matrix_linear_system.cxx

# special matrices
SOURCES += vnl_rotation_matrix.cxx
MANPAGE_SOURCES += vnl_cross_product_matrix.h
MANPAGE_SOURCES += vnl_identity_3x3.h

# special functions
SOURCES += vnl_bessel.cxx
MANPAGE_SOURCES += vnl_cross.h
SOURCES += vnl_erf.cxx
SOURCES += vnl_gamma.cxx
SOURCES += vnl_sample.cxx
SOURCES += vnl_random.cxx
MANPAGE_SOURCES += vnl_unary_function.txx
MANPAGE_SOURCES += vnl_identity.h

# numerical integration
MANPAGE_SOURCES += vnl_analytic_integrant.h
MANPAGE_SOURCES += vnl_integrant_fnct.h
SOURCES += vnl_definite_integral.cxx

# algo
SUBDIRS += algo

#######################################
include ${VXLROOT}/config/top-params.mk
#######################################

# tests
ifndef NOTEST
SUBDIRS += tests
endif

# examples
ifndef NOTEST
SUBDIRS += examples
endif

######################################
include ${VXLROOT}/config/top-rules.mk
######################################

debug=$(debug_for_optimize) # needed for using $(optimize) on some files

# on HP unix and Solaris, this library has become too big, so it needs -fPIC
pic = $(PIC)

FLAGS_vnl_fastops.cxx := $(optimize)
FLAGS_Templates/vnl_c_vector+float-.cxx := $(optimize)
FLAGS_Templates/vnl_c_vector+double-.cxx := $(optimize)
FLAGS_Templates/vnl_c_vector+float_complex-.cxx := $(optimize)
FLAGS_Templates/vnl_c_vector+double_complex-.cxx := $(optimize)
FLAGS_Templates/vnl_sparse_matrix_instances.cxx := $(optimize)

ifeq ($(OS),OSF1V5) # with optimisation, the is_finite etc functions fail for long double
FLAGS_vnl_math.cxx := -O0
FLAGS_vnl_bignum.cxx := -O0
                    # and these need too much memory with gcc 3.0
FLAGS_Templates/vnl_matrix+long-.cxx := -O0
FLAGS_Templates/vnl_matrix+ulong-.cxx := -O0
FLAGS_Templates/vnl_matrix+vcl_complex+double--.cxx := -O0
FLAGS_Templates/vnl_matrix+vcl_complex+long_double--.cxx := -O0
FLAGS_Templates/vnl_matrix_fixed+vnl_bignum.3.3-.cxx := -O0
FLAGS_Templates/vnl_vector+vnl_rational-.cxx := -O0
FLAGS_Templates/vnl_vector_fixed+vnl_rational.3-.cxx := -O0
endif

$(VXLROOT)/core/vnl/vnl_config.h: vnl_config.h.in
	perl -p -e 's/\@VNL_CONFIG_CHECK_BOUNDS\@/1/;s/\@VNL_CONFIG_LEGACY_METHODS\@/0/;s/\@VNL_CONFIG_THREAD_SAFE\@/1/;' $^ > $@
