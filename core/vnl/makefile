################################
include ${IUEROOT}/config/usr_params.mk
################################

USES := vxl
LIBRARY := vnl
VERSION := 1.0
IULIBS := -lvcl
DEFINES += -DBUILDING_VNL_DLL

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
SOURCES += $(wildcard vnl_block.cxx)
FLAGS_vnl_block.cxx += $(optimize)

#
SOURCES += vnl_alloc.cxx
SOURCES += vnl_math.cxx
SOURCES += vnl_copy.cxx
SOURCES += vnl_complex.cxx
SOURCES += vnl_error.cxx
SOURCES += vnl_test.cxx
SOURCES += vnl_matlab_print.cxx
SOURCES += vnl_matlab_write.cxx
SOURCES += vnl_matlab_read.cxx
SOURCES += vnl_matlab_filewrite.cxx
SOURCES += vnl_matops.cxx
SOURCES += vnl_real_polynomial.cxx
SOURCES += vnl_real_npolynomial.cxx

# ops
SOURCES += vnl_fastops.cxx
SOURCES += vnl_linear_operators_3.cxx
#MANPAGE_SOURCES += vnl_complex_ops.txx
FLAGS_vnl_fastops.cxx = $(optimize)

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

# stuff
SOURCES += vnl_trace.cxx
SOURCES += vnl_sample.cxx
SOURCES += vnl_unary_function.cxx
SOURCES += vnl_transpose.cxx

# algo
SUBDIRS += algo

# tests
ifndef NOTEST
SUBDIRS += tests
endif


ifndef NOTEST
SUBDIRS += examples
endif

##  Only needed for Templates/vnl_matrix+* and Templates/vnl_c_vector+* :
ifeq ($(COMPILER),CC-n32)
PRAGMA_INSTANTIATE_TEMPLATES = 1
endif
#optimize := -O3
FLAGS_Templates/vnl_c_vector+float-.cxx += $(optimize)
FLAGS_Templates/vnl_c_vector+double-.cxx += $(optimize)
FLAGS_Templates/vnl_c_vector+float_complex-.cxx += $(optimize)
FLAGS_Templates/vnl_c_vector+double_complex-.cxx += $(optimize)
FLAGS_Templates/vnl_sparse_matrix_instances.cxx += $(optimize)

################################
include ${IUEROOT}/config/usr_rules.mk
################################

# on HP unix, this library has become too big, so it needs -fPIC
ifeq "hpux" "$(findstring hpux,$(OS))"
pic = $(PIC)
endif
