#######################################
include ${IUEROOT}/config/top-params.mk
#######################################

USES := vxl
USES += mul

LIBRARY := vbl-io
VERSION := 1.0

SOURCES += vbl_io_user_info.cxx

# Additional files to document:
MANPAGE_SOURCES += vbl_io_array_1d.txx
MANPAGE_SOURCES += vbl_io_array_2d.txx
MANPAGE_SOURCES += vbl_io_array_3d.txx
MANPAGE_SOURCES += vbl_io_bounding_box.txx
MANPAGE_SOURCES += vbl_io_smart_ptr.txx
MANPAGE_SOURCES += vbl_io_sparse_array.txx
MANPAGE_SOURCES += vbl_io_sparse_array_base.txx

ifndef NOTEST
 SUBDIRS += tests
endif

######################################
include ${IUEROOT}/config/top-rules.mk
######################################
