#   DESCRIPTION:
#
#   This is the vbl (Vision Basics) library.
#
#   DEPENDENCIES:  libvcl

VXL := 1 

USES += vxl
IULIBS := -lvcl
IULIBS += -lvbl
IULIBS += -lvsol

LIBRARY := vtol
VERSION := 1.0



SOURCES += vtol_topology_object_2d.cxx
SOURCES += vtol_topology_cache_2d.cxx
SOURCES += vtol_topology_hierarchy_node_2d.cxx
SOURCES += vtol_list_functions_2d.cxx
SOURCES += vtol_hierarchy_node_2d.cxx
SOURCES += vtol_two_chain_2d.cxx
SOURCES += vtol_one_chain_2d.cxx
SOURCES += vtol_zero_chain_2d.cxx
SOURCES += vtol_edge_2d.cxx
SOURCES += vtol_block_2d.cxx
SOURCES += vtol_face_2d.cxx
SOURCES += vtol_vertex_2d.cxx


SOURCES += vtol_topology_object_3d.cxx
SOURCES += vtol_topology_cache_3d.cxx
SOURCES += vtol_topology_hierarchy_node_3d.cxx
SOURCES += vtol_list_functions_3d.cxx
SOURCES += vtol_hierarchy_node_3d.cxx
SOURCES += vtol_two_chain_3d.cxx
SOURCES += vtol_one_chain_3d.cxx
SOURCES += vtol_zero_chain_3d.cxx
SOURCES += vtol_edge_3d.cxx
SOURCES += vtol_block_3d.cxx
SOURCES += vtol_face_3d.cxx
SOURCES += vtol_vertex_3d.cxx


# DEFINES += -DBUILDING_VTOL_DLL

##########################################
include ${IUEROOT}/config/top-params.mk
##########################################

ifndef NOTEST
SUBDIRS += tests
SUBDIRS += examples
endif

##########################################
include ${IUEROOT}/config/top-rules.mk
##########################################
