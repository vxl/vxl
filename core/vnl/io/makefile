USES := vxl
USES += mul
LIBRARY := vnl-io
VERSION := 1.0
IMMEDIATE_LIBS := -lvnl-io -lvnl -lvsl

SOURCES += vnl_io_nonlinear_minimizer.cxx
SOURCES += vnl_io_real_polynomial.cxx
SOURCES += vnl_io_real_npolynomial.cxx

MANPAGE_SOURCES += vnl_io_diag_matrix.txx
MANPAGE_SOURCES += vnl_io_matrix.txx
MANPAGE_SOURCES += vnl_io_matrix_fixed.txx
MANPAGE_SOURCES += vnl_io_sparse_matrix.txx
MANPAGE_SOURCES += vnl_io_vector.txx
MANPAGE_SOURCES += vnl_io_vector_fixed.txx

ifndef NOTEST
SUBDIRS += tests
endif

################################
include ${IUEROOT}/config/top.mk
################################
