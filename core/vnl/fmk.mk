include ${fmk_ROOT}/fmk_params.mk

fmk_USES := vxl

fmk_LIBRARY := vnl
fmk_VERSION := 1.0

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
#MANPAGE_SOURCES += vnl_resize.txx

#
fmk_LIB_SOURCES += vnl_math.cxx
fmk_LIB_SOURCES += vnl_copy.cxx
fmk_LIB_SOURCES += vnl_complex.cxx
fmk_LIB_SOURCES += vnl_error.cxx
fmk_LIB_SOURCES += vnl_test.cxx
fmk_LIB_SOURCES += vnl_matlab_print.cxx
fmk_LIB_SOURCES += vnl_matlab_write.cxx
fmk_LIB_SOURCES += vnl_matlab_read.cxx
fmk_LIB_SOURCES += vnl_matlab_filewrite.cxx
fmk_LIB_SOURCES += vnl_matops.cxx
fmk_LIB_SOURCES += vnl_real_polynomial.cxx
fmk_LIB_SOURCES += vnl_real_npolynomial.cxx

# ops
fmk_LIB_SOURCES += vnl_fastops.cxx
fmk_LIB_SOURCES += vnl_linear_operators_3.cxx
#MANPAGE_SOURCES += vnl_complex_ops.txx

# traits
fmk_LIB_SOURCES += vnl_numeric_limits.cxx
fmk_LIB_SOURCES += vnl_numeric_traits.cxx
fmk_LIB_SOURCES += vnl_complex_traits.cxx

#
fmk_LIB_SOURCES += vnl_int_matrix.cxx
fmk_LIB_SOURCES += vnl_int_2.cxx
fmk_LIB_SOURCES += vnl_int_3.cxx
fmk_LIB_SOURCES += vnl_int_4.cxx
fmk_LIB_SOURCES += vnl_float_2.cxx
fmk_LIB_SOURCES += vnl_float_3.cxx
fmk_LIB_SOURCES += vnl_double_2.cxx
fmk_LIB_SOURCES += vnl_double_3.cxx
fmk_LIB_SOURCES += vnl_double_4.cxx
fmk_LIB_SOURCES += vnl_double_2x3.cxx

# optimization
fmk_LIB_SOURCES += vnl_cost_function.cxx
fmk_LIB_SOURCES += vnl_least_squares_function.cxx
fmk_LIB_SOURCES += vnl_least_squares_cost_function.cxx
fmk_LIB_SOURCES += vnl_nonlinear_minimizer.cxx

# special matrices
fmk_LIB_SOURCES += vnl_rotation_matrix.cxx
fmk_LIB_SOURCES += vnl_cross_product_matrix.cxx
fmk_LIB_SOURCES += vnl_identity_3x3.cxx

# stuff
fmk_LIB_SOURCES += vnl_trace.cxx
fmk_LIB_SOURCES += vnl_sample.cxx
fmk_LIB_SOURCES += vnl_unary_function.cxx
fmk_LIB_SOURCES += vnl_transpose.cxx

# algo
fmk_SUBDIRS += algo

# tests
ifndef NOTEST
#fmk_SUBDIRS += tests
endif


#FIXME DEFINES += -DBUILDING_VNL_DLL
ifndef NOTEST
#fmk_SUBDIRS += examples
endif

#FIXME
# ##  Only needed for Templates/vnl_matrix+* and Templates/vnl_c_vector+* :
# ifeq ($(COMPILER),CC-n32)
# PRAGMA_INSTANTIATE_TEMPLATES = 1
# endif

fmk_FLAGS_vnl_fastops.cxx += $(fmk_optimize)
fmk_FLAGS_Templates/vnl_c_vector+float-.cxx += $(fmk_optimize)
fmk_FLAGS_Templates/vnl_c_vector+double-.cxx += $(fmk_optimize)
fmk_FLAGS_Templates/vnl_c_vector+float_complex-.cxx += $(fmk_optimize)
fmk_FLAGS_Templates/vnl_c_vector+double_complex-.cxx += $(fmk_optimize)

include ${fmk_ROOT}/fmk_rules.mk
