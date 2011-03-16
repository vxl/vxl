USES += contrib/brl contrib/brl/bbas core

LIBRARY := imesh
VERSION := 1.0

SOURCES += imesh_detection.cxx
SOURCES += imesh_face.cxx
SOURCES += imesh_fileio.cxx
SOURCES += imesh_half_edge.cxx
SOURCES += imesh_mesh.cxx
SOURCES += imesh_operations.cxx
SOURCES += imesh_vertex.cxx

SUBDIRS += algo

#######################################
include ${VXLROOT}/config/top-params.mk
#######################################

ifndef NOTEST
 SUBDIRS += tests
endif

######################################
include ${VXLROOT}/config/top-rules.mk
######################################
