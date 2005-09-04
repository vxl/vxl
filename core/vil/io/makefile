USES += core

LIBRARY := vil-io
VERSION := 1.0

SOURCES += vil_io_memory_chunk.cxx
MANPAGE_SOURCES += vil_io_smart_ptr.txx
MANPAGE_SOURCES += vil_io_image_view.h

#######################################
include ${VXLROOT}/config/top-params.mk
#######################################

ifndef NOTEST
SUBDIRS += tests
endif

######################################
include ${VXLROOT}/config/top-rules.mk
######################################
