USES += core

LIBRARY := vbl-io
VERSION := 1.0

# Additional files to document:
MANPAGE_SOURCES += vbl_io_array_1d.txx
MANPAGE_SOURCES += vbl_io_array_2d.txx
MANPAGE_SOURCES += vbl_io_array_3d.txx
MANPAGE_SOURCES += vbl_io_bounding_box.txx
MANPAGE_SOURCES += vbl_io_smart_ptr.txx
MANPAGE_SOURCES += vbl_io_sparse_array_base.txx
MANPAGE_SOURCES += vbl_io_quadruple.txx
MANPAGE_SOURCES += vbl_io_sparse_array_1d.h
MANPAGE_SOURCES += vbl_io_sparse_array_2d.h
MANPAGE_SOURCES += vbl_io_sparse_array_3d.h
MANPAGE_SOURCES += vbl_io_triple.txx

#######################################
include ${IUEROOT}/config/top-params.mk
#######################################

ifndef NOTEST
 SUBDIRS += tests
endif

######################################
include ${IUEROOT}/config/top-rules.mk
######################################
