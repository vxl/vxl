USES += contrib/brl contrib/brl/bbas contrib/gel core

LIBRARY := btol
VERSION := 1.0
DEFINES += -DBUILDING_BTOL_DLL

SOURCES += btol_edge_algs.cxx
SOURCES += btol_vertex_algs.cxx
SOURCES += btol_face_algs.cxx

#######################################
include ${VXLROOT}/config/top-params.mk
#######################################

ifndef NOTEST
SUBDIRS += tests
endif

######################################
include ${VXLROOT}/config/top-rules.mk
######################################
