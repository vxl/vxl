USES += contrib/brl contrib/brl/bbas core

LIBRARY := bgrl2

SOURCES += bgrl2_hg_graph.cxx
SOURCES += bgrl2_hg_hypergraph.cxx
MANPAGE_SOURCES += bgrl2_edge.txx
MANPAGE_SOURCES += bgrl2_graph.txx
MANPAGE_SOURCES += bgrl2_hg_edge.h
MANPAGE_SOURCES += bgrl2_hg_hyperedge.h
MANPAGE_SOURCES += bgrl2_hg_vertex.h
MANPAGE_SOURCES += bgrl2_vertex.txx
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
