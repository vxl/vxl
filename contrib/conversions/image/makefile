# This is the image conversions directory makefile

USES := vxl
IMMEDIATE_LIBS := -lvil -lvbl

SUBDIRS := tests

MANPAGE_SOURCES += vil_to_vbl_array.h
MANPAGE_SOURCES += vbl_array_to_vil.h
ifneq (,$(wildcard $(IUEROOT)/Image))
USES += Image
IMMEDIATE_LIBS += -lImageClasses
MANPAGE_SOURCES += Image_to_vil.h
MANPAGE_SOURCES += vil_to_Image.h
endif

################################
include ${IUEROOT}/config/top.mk
################################
