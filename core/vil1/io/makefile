USES := vxl
USES += mul

TRY_TIFF := 1
TRY_JPEG := 1
TRY_PNG  := 1 
USE_SOCKETS := 1

LIBRARY := vil-io
VERSION := 1.0
IMMEDIATE_LIBS := -lvil-io -lvil -lvsl 

SOURCES += vil_io_image.cxx vil_io_image_impl.cxx vil_io_memory_image.cxx vil_io_memory_image_format.cxx vil_io_memory_image_impl.cxx

MANPAGE_SOURCES += 

ifndef NOTEST
SUBDIRS += tests
endif

################################
include ${IUEROOT}/config/top.mk
################################
