# This is the image conversions directory makefile
ifneq (,$(wildcard $(IUEROOT)/Image))

USES := Image vxl
IMMEDIATE_LIBS := -lImageClasses -lvil -lvbl

SUBDIRS := tests

MANPAGE_SOURCES += Image_to_vil.h
MANPAGE_SOURCES += vil_to_Image.h
MANPAGE_SOURCES += vil_to_vbl_array.h
MANPAGE_SOURCES += vbl_array_to_vil.h
endif

################################
include ${IUEROOT}/config/top.mk
################################
