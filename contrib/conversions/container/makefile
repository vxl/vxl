# This is the container conversions directory makefile
ifneq (,$(wilcard $(IUEROOT)/cool))

USES := COOL
IMMEDIATE_LIBS := -lCOOL

SUBDIRS := tests

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
MANPAGE_SOURCES += vcl_vector_to_vcl_list.h
MANPAGE_SOURCES += vcl_list_to_vcl_vector.h
endif

################################
include ${IUEROOT}/config/top.mk
################################
