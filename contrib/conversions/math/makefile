# This is the matrix/vector conversions directory makefile

USES += core
IMMEDIATE_LIBS := -lvbl -lvnl

MANPAGE_SOURCES += vnl_matrix_to_vbl_array.h
MANPAGE_SOURCES += vbl_array_to_vnl_matrix.h
MANPAGE_SOURCES += vnl_vector_to_vbl_array.h
MANPAGE_SOURCES += vbl_array_to_vnl_vector.h
ifneq (,$(wildcard $(IUEROOT)/COOL))
USES += COOL
MANPAGE_SOURCES += vnl_matrix_to_CoolMatrix.h
MANPAGE_SOURCES += CoolMatrix_to_vnl_matrix.h
MANPAGE_SOURCES += vnl_vector_to_CoolVector.h
MANPAGE_SOURCES += CoolVector_to_vnl_vector.h
endif
ifneq (,$(wildcard $(IUEROOT)/Numerics))
USES += Numerics
IMMEDIATE_LIBS += -liue-math
MANPAGE_SOURCES += vnl_matrix_to_IUE_matrix.h
MANPAGE_SOURCES += IUE_matrix_to_vnl_matrix.h
MANPAGE_SOURCES += vnl_vector_to_IUE_vector.h
MANPAGE_SOURCES += IUE_vector_to_vnl_vector.h
endif

#######################################
include ${IUEROOT}/config/top-params.mk
#######################################

ifndef NOTEST
SUBDIRS := tests
endif

######################################
include ${IUEROOT}/config/top-rules.mk
######################################
