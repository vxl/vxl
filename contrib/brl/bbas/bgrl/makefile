USES += contrib/brl contrib/brl/bbas core

LIBRARY := bgrl
VERSION := 1.0

MANPAGE_SOURCES += bgrl_graph_sptr.h
SOURCES += bgrl_vertex.cxx
MANPAGE_SOURCES += bgrl_vertex_sptr.h
SOURCES += bgrl_edge.cxx
MANPAGE_SOURCES += bgrl_edge_sptr.h
SOURCES += bgrl_graph.cxx

#######################################
include ${IUEROOT}/config/top-params.mk
#######################################

ifndef NOTEST
 SUBDIRS += tests
endif

######################################
include ${IUEROOT}/config/top-rules.mk
######################################
