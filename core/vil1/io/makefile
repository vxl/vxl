USES := vxl
USES += mul

TRY_TIFF := 1
TRY_JPEG := 1
TRY_PNG  := 1 
USE_SOCKETS := 1

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

ifndef NOTEST
SUBDIRS += tests
endif

################################
include ${IUEROOT}/config/top.mk
################################
