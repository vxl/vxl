VXL := 1

################################
include ${IUEROOT}/config/top-params.mk
################################

USES := vxl

LIBRARY := vnl
VERSION := 1.0

# template sources
#MANPAGE_SOURCES += vnl_scalar_join_iterator.txx
#MANPAGE_SOURCES += vnl_c_vector.txx
#MANPAGE_SOURCES += vnl_vector.txx
#MANPAGE_SOURCES += vnl_matrix.txx
#MANPAGE_SOURCES += vnl_diag_matrix.txx
#MANPAGE_SOURCES += vnl_fortran_copy.txx
#MANPAGE_SOURCES += vnl_vector_fixed.txx
#MANPAGE_SOURCES += vnl_matrix_fixed.txx
#MANPAGE_SOURCES += vnl_file_vector.txx
#MANPAGE_SOURCES += vnl_file_matrix.txx
#MANPAGE_SOURCES += vnl_scatter_3x3.txx

SOURCES += ftraits.cxx

#
SOURCES += vnl_math.cxx
SOURCES += vnl_error.cxx
SOURCES += vnl_test.cxx
SOURCES += vnl_matops.cxx
SOURCES += vnl_real_polynomial.cxx
SOURCES += vnl_real_npolynomial.cxx

# ops
SOURCES += vnl_fastops.cxx
SOURCES += vnl_linear_operators_3.cxx
SOURCES += vnl_complex_ops.cxx

# traits
SOURCES += vnl_numeric_limits.cxx
SOURCES += vnl_numeric_traits.cxx
SOURCES += vnl_complex_traits.cxx

#
SOURCES += vnl_int_matrix.cxx     # ?
SOURCES += vnl_int_2.cxx
SOURCES += vnl_int_3.cxx
SOURCES += vnl_int_4.cxx
SOURCES += vnl_float_2.cxx
SOURCES += vnl_float_3.cxx
SOURCES += vnl_double_2.cxx
SOURCES += vnl_double_3.cxx
SOURCES += vnl_double_4.cxx
SOURCES += vnl_double_2x3.cxx

# optimization
SOURCES += vnl_cost_function.cxx
SOURCES += vnl_least_squares_function.cxx
SOURCES += vnl_least_squares_cost_function.cxx
SOURCES += vnl_nonlinear_minimizer.cxx

# special matrices
SOURCES += vnl_rotation_matrix.cxx
SOURCES += vnl_cross_product_matrix.cxx
SOURCES += vnl_identity_3x3.cxx

# algo
SUBDIRS += algo

#more
# stuff
SOURCES += vnl_unary_function.cxx
SOURCES += vnl_transpose.cxx
SOURCES += vnl_resize.cxx

# tests
ifneq (1, $(NOTEST))
SUBDIRS += test
endif

IULIBS := -lvcl

DEFINES += -DBUILDING_VNL_DLL

################################
include ${IUEROOT}/config/top-rules.mk
################################
