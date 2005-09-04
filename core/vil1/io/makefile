USES += core
include ${VXLROOT}/core/vil1/config.mk

LIBRARY := vil1-io
VERSION := 1.0
IMMEDIATE_LIBS := -lvil1-io -lvil1 -lvsl 

SOURCES += vil1_io_image.cxx
SOURCES += vil1_io_image_impl.cxx
SOURCES += vil1_io_memory_image.cxx
SOURCES += vil1_io_memory_image_format.cxx
SOURCES += vil1_io_memory_image_impl.cxx

MANPAGE_SOURCES += vil1_io_rgb.txx
MANPAGE_SOURCES += vil1_io_rgba.txx
MANPAGE_SOURCES += vil1_io_memory_image_of.txx

#######################################
include ${VXLROOT}/config/top-params.mk
#######################################

ifndef NOTEST
SUBDIRS += tests
endif

######################################
include ${VXLROOT}/config/top-rules.mk
######################################
