USES := vxl
USES += mul
LIBRARY := vgl-io
VERSION := 1.0
IMMEDIATE_LIBS := -lvgl-io -lvgl -lvsl

SOURCES += vgl_io_polygon.cxx

MANPAGE_SOURCES += vgl_point_2d.txx

ifndef NOTEST
SUBDIRS += tests
endif

################################
include ${IUEROOT}/config/top.mk
################################
