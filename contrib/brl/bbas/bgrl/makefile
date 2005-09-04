USES += contrib/brl contrib/brl/bbas core

LIBRARY := bgrl
VERSION := 1.0

MANPAGE_SOURCES += bgrl_graph_sptr.h
SOURCES += bgrl_vertex.cxx
MANPAGE_SOURCES += bgrl_vertex_sptr.h
SOURCES += bgrl_edge.cxx
MANPAGE_SOURCES += bgrl_edge_sptr.h
SOURCES += bgrl_graph.cxx
SOURCES += bgrl_search_func.cxx
MANPAGE_SOURCES += bgrl_search_func_sptr.h


#######################################
include ${VXLROOT}/config/top-params.mk
#######################################

ifndef NOTEST
 SUBDIRS += tests
endif

######################################
include ${VXLROOT}/config/top-rules.mk
######################################
