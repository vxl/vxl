#   DESCRIPTION:
#
#   This is the vtol (Vision Topology) library.
#
#   DEPENDENCIES:  libvcl libvbl libvsol

USES += vxl gel
IMMEDIATE_LIBS += -lvdgl -lvsol -lvbl -lvul

LIBRARY := vtol
VERSION := 1.0

SOURCES += vtol_topology_object.cxx
SOURCES += vtol_topology_cache.cxx
SOURCES += vtol_list_functions.cxx
SOURCES += vtol_chain.cxx
SOURCES += vtol_two_chain.cxx
SOURCES += vtol_one_chain.cxx
SOURCES += vtol_zero_chain.cxx
SOURCES += vtol_edge.cxx
SOURCES += vtol_block.cxx
SOURCES += vtol_face.cxx
SOURCES += vtol_vertex.cxx

SOURCES += vtol_edge_2d.cxx
SOURCES += vtol_face_2d.cxx
SOURCES += vtol_vertex_2d.cxx

SOURCES += vtol_topology_io.cxx
SOURCES += vtol_cycle_processor.cxx

MANPAGE_SOURCES += vtol_block_sptr.h
MANPAGE_SOURCES += vtol_chain_sptr.h
MANPAGE_SOURCES += vtol_dtd.h
MANPAGE_SOURCES += vtol_edge_2d_sptr.h
MANPAGE_SOURCES += vtol_edge_sptr.h
MANPAGE_SOURCES += vtol_face_2d_sptr.h
MANPAGE_SOURCES += vtol_face_sptr.h
MANPAGE_SOURCES += vtol_macros.h
MANPAGE_SOURCES += vtol_one_chain_sptr.h
MANPAGE_SOURCES += vtol_topology_cache_sptr.h
MANPAGE_SOURCES += vtol_topology_object_sptr.h
MANPAGE_SOURCES += vtol_two_chain_sptr.h
MANPAGE_SOURCES += vtol_vertex_2d_sptr.h
MANPAGE_SOURCES += vtol_vertex_sptr.h
MANPAGE_SOURCES += vtol_zero_chain_sptr.h

#######################################
include ${IUEROOT}/config/top-params.mk
#######################################

ifeq "$(TJ_GCC30)" "1"
FLAGS_vtol_two_chain.cxx += -O1 # otherwise gcc 3.0 needs too much virtual memory
endif

ifndef NOTEST
SUBDIRS += tests
SUBDIRS += examples
endif

######################################
include ${IUEROOT}/config/top-rules.mk
######################################

# on HP unix, this library has become too big, so it needs -fPIC
ifeq "HP-UX" "$(findstring HP-UX,$(OS))"
pic = $(PIC)
endif
