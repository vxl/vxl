# This is the image conversions directory makefile

USES += core
IMMEDIATE_LIBS := -lvil1 -lvbl

MANPAGE_SOURCES += vil1_to_vbl_array.h
MANPAGE_SOURCES += vbl_array_to_vil1.h

ifneq (,$(wildcard $(VXLROOT)/Image))
USES += Image
IMMEDIATE_LIBS += -lImageClasses
MANPAGE_SOURCES += Image_to_vil1.h
MANPAGE_SOURCES += vil1_to_Image.h
endif

#######################################
include ${VXLROOT}/config/top-params.mk
#######################################

ifndef NOTEST
SUBDIRS := tests
endif

######################################
include ${VXLROOT}/config/top-rules.mk
######################################
