#######################################
include ${IUEROOT}/config/top-params.mk
#######################################

USES += vxl

LIBRARY := vbl
VERSION := 1.0

SOURCES += vbl_bool_ostream.cxx
SOURCES += vbl_ref_count.cxx
SOURCES += vbl_sort.cxx
SOURCES += vbl_qsort.cxx
SOURCES += vbl_shared_pointer.cxx

MANPAGE_SOURCES += vbl_array.txx # deprecated
MANPAGE_SOURCES += vbl_array_1d.txx
MANPAGE_SOURCES += vbl_array_2d.txx
MANPAGE_SOURCES += vbl_array_3d.txx
MANPAGE_SOURCES += vbl_binary.txx
MANPAGE_SOURCES += vbl_bounding_box.txx
#MANPAGE_SOURCES += vbl_basic_relation.txx # FIXME - vcl_hash_map does not exist
#MANPAGE_SOURCES += vbl_br_default.txx
#MANPAGE_SOURCES += vbl_br_impl.txx
#MANPAGE_SOURCES += vbl_hash_map.txx
MANPAGE_SOURCES += vbl_protection_traits.txx
MANPAGE_SOURCES += vbl_smart_ptr.txx
MANPAGE_SOURCES += vbl_sort.txx
MANPAGE_SOURCES += vbl_sparse_array.txx # deprecated
MANPAGE_SOURCES += vbl_sparse_array_base.txx
MANPAGE_SOURCES += vbl_sparse_array_1d.txx
MANPAGE_SOURCES += vbl_sparse_array_2d.txx
MANPAGE_SOURCES += vbl_sparse_array_3d.txx

# debugging

DEFINES += -DBUILDING_VBL_DLL

ifndef NOTEST
SUBDIRS += tests
SUBDIRS += examples
endif

######################################
include ${IUEROOT}/config/top-rules.mk
######################################
