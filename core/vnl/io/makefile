USES += core
LIBRARY := vnl-io
VERSION := 1.0
IMMEDIATE_LIBS := -lvnl-io -lvnl -lvsl

SOURCES += vnl_io_nonlinear_minimizer.cxx
SOURCES += vnl_io_real_polynomial.cxx
SOURCES += vnl_io_real_npolynomial.cxx
SOURCES += vnl_io_rational.cxx
SOURCES += vnl_io_bignum.cxx

MANPAGE_SOURCES += vnl_io_matrix.txx
MANPAGE_SOURCES += vnl_io_diag_matrix.txx
MANPAGE_SOURCES += vnl_io_sparse_matrix.txx
MANPAGE_SOURCES += vnl_io_sym_matrix.txx
MANPAGE_SOURCES += vnl_io_vector.txx

MANPAGE_SOURCES += vnl_io_matrix_fixed.txx
MANPAGE_SOURCES += vnl_io_vector_fixed.txx

#######################################
include ${IUEROOT}/config/top-params.mk
#######################################

ifndef NOTEST
SUBDIRS += tests
endif

######################################
include ${IUEROOT}/config/top-rules.mk
######################################

ifeq ($(COMPILER),gcc)
FLAGS_Templates/vsl_vector_io+vcl_vector+vcl_vector+vnl_vector+double----.cxx := -ftemplate-depth-35
endif
