USES += core
include ${IUEROOT}/core/vil/config.mk

LIBRARY := vil-io
VERSION := 1.0
IMMEDIATE_LIBS := -lvil-io -lvil -lvsl 

SOURCES += vil_io_image.cxx
SOURCES += vil_io_image_impl.cxx
SOURCES += vil_io_memory_image.cxx
SOURCES += vil_io_memory_image_format.cxx
SOURCES += vil_io_memory_image_impl.cxx

MANPAGE_SOURCES += vil_io_rgb.txx
MANPAGE_SOURCES += vil_io_rgba.txx
MANPAGE_SOURCES += vil_io_memory_image_of.txx

#######################################
include ${IUEROOT}/config/top-params.mk
#######################################

ifndef NOTEST
SUBDIRS += tests
endif

######################################
include ${IUEROOT}/config/top-rules.mk
######################################
