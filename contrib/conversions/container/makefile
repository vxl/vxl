# This is the container conversions directory makefile

USES += core
IMMEDIATE_LIBS += -lvnl

MANPAGE_SOURCES += vcl_vector_to_vcl_list.h
MANPAGE_SOURCES += vcl_list_to_vcl_vector.h
MANPAGE_SOURCES += vcl_vector_to_vnl_vector.h
MANPAGE_SOURCES += vnl_vector_to_vcl_vector.h
ifneq (,$(wildcard $(IUEROOT)/COOL))
USES += COOL
IMMEDIATE_LIBS += -lCOOL
MANPAGE_SOURCES += CoolList_to_vcl_list.h
MANPAGE_SOURCES += vcl_list_to_CoolList.h
MANPAGE_SOURCES += CoolList_to_vcl_vector.h
MANPAGE_SOURCES += vcl_vector_to_CoolList.h
MANPAGE_SOURCES += CoolArray_to_vcl_vector.h
MANPAGE_SOURCES += vcl_vector_to_CoolArray.h
MANPAGE_SOURCES += CoolArray_to_vcl_list.h
MANPAGE_SOURCES += vcl_list_to_CoolArray.h
MANPAGE_SOURCES += CoolArray_to_CoolList.h
MANPAGE_SOURCES += CoolList_to_CoolArray.h
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
