USES += core

LIBRARY := vbl
VERSION := 1.0

MANPAGE_SOURCES += vbl_fwd.h

SOURCES += vbl_bool_ostream.cxx
MANPAGE_SOURCES += vbl_ref_count.h
SOURCES += vbl_sort.cxx
MANPAGE_SOURCES += vbl_sort.txx
SOURCES += vbl_qsort.cxx
MANPAGE_SOURCES += vbl_shared_pointer.h
MANPAGE_SOURCES += vbl_smart_ptr.txx

MANPAGE_SOURCES += vbl_array_1d.txx
MANPAGE_SOURCES += vbl_array_2d.txx
MANPAGE_SOURCES += vbl_array_3d.txx
SOURCES += vbl_bit_array_2d.cxx
SOURCES += vbl_bit_array_3d.cxx
MANPAGE_SOURCES += vbl_sparse_array_base.txx
MANPAGE_SOURCES += vbl_sparse_array_1d.txx
MANPAGE_SOURCES += vbl_sparse_array_2d.txx
MANPAGE_SOURCES += vbl_sparse_array_3d.txx
MANPAGE_SOURCES += vbl_big_sparse_array_3d.txx

MANPAGE_SOURCES += vbl_bounding_box.txx

MANPAGE_SOURCES += vbl_triple.txx
MANPAGE_SOURCES += vbl_quadruple.txx

MANPAGE_SOURCES += vbl_checked_delete.h
MANPAGE_SOURCES += vbl_scoped_ptr.h

# debugging

DEFINES += -DBUILDING_VBL_DLL

#######################################
include ${VXLROOT}/config/top-params.mk
#######################################

ifndef NOTEST
SUBDIRS += tests
SUBDIRS += examples
endif

######################################
include ${VXLROOT}/config/top-rules.mk
######################################
