#   DESCRIPTION:
#
#   This is the vtol (Vision Topology) library.
#
#   DEPENDENCIES:  libvcl libvbl libvsol

USES += vxl gel
IMMEDIATE_LIBS += -lvsol -lvbl

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

#######################################
include ${IUEROOT}/config/top-params.mk
#######################################

ifndef NOTEST
#SUBDIRS += tests
SUBDIRS += examples
endif

######################################
include ${IUEROOT}/config/top-rules.mk
######################################

# on HP unix, this library has become too big, so it needs -fPIC
ifeq "HP-UX" "$(findstring HP-UX,$(OS))"
pic = $(PIC)
endif
