#######################################
include ${IUEROOT}/config/top-params.mk
#######################################

USES += vxl

LIBRARY := vbl
VERSION := 1.0

MANPAGE_SOURCES += vbl_fwd.h

SOURCES += vbl_bool_ostream.cxx
SOURCES += vbl_ref_count.cxx
SOURCES += vbl_sort.cxx
MANPAGE_SOURCES += vbl_sort.txx
SOURCES += vbl_qsort.cxx
SOURCES += vbl_shared_pointer.cxx
MANPAGE_SOURCES += vbl_shared_pointer.txx
MANPAGE_SOURCES += vbl_protection_traits.txx
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

#MANPAGE_SOURCES += vbl_basic_relation.txx # FIXME - vcl_hash_map does not exist
MANPAGE_SOURCES += vbl_basic_optional.h
MANPAGE_SOURCES += vbl_basic_relation_type.h
MANPAGE_SOURCES += vbl_basic_relation_where.h
MANPAGE_SOURCES += vbl_basic_tuple.h
#MANPAGE_SOURCES += vbl_br_default.txx
#MANPAGE_SOURCES += vbl_br_impl.txx
#MANPAGE_SOURCES += vbl_hash_map.txx

# debugging

DEFINES += -DBUILDING_VBL_DLL

# stream I/O
SUBDIRS += io

ifndef NOTEST
SUBDIRS += tests
SUBDIRS += examples
endif

######################################
include ${IUEROOT}/config/top-rules.mk
######################################
