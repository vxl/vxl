USES := vxl mul

LIBRARY := vil2-io
VERSION := 1.0

SOURCES += vil2_io_memory_chunk.cxx
MANPAGE_SOURCES += vil2_io_smart_ptr.txx
MANPAGE_SOURCES += vil2_io_image_view.h

#######################################
include ${IUEROOT}/config/top-params.mk
#######################################

ifndef NOTEST
SUBDIRS += tests
endif

######################################
include ${IUEROOT}/config/top-rules.mk
######################################
