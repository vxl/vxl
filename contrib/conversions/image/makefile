# This is the image conversions directory makefile

USES := vxl
IMMEDIATE_LIBS := -lvil -lvbl

MANPAGE_SOURCES += vil_to_vbl_array.h
MANPAGE_SOURCES += vbl_array_to_vil.h

ifneq (,$(wildcard $(IUEROOT)/mul))
USES += mul
IMMEDIATE_LIBS := -lmil
MANPAGE_SOURCES += vil_to_mil.h
MANPAGE_SOURCES += mil_to_vil.h
endif
ifneq (,$(wildcard $(IUEROOT)/Image))
USES += Image
IMMEDIATE_LIBS += -lImageClasses
MANPAGE_SOURCES += Image_to_vil.h
MANPAGE_SOURCES += vil_to_Image.h
endif

ifndef NOTEST
SUBDIRS := tests
endif

################################
include ${IUEROOT}/config/top.mk
################################
