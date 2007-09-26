USES += contrib/mul core

LIBRARY := mmn
VERSION := 1.0

MANPAGE_SOURCES += mmn_analyze_graph.h
MANPAGE_SOURCES += mmn_arc.h
MANPAGE_SOURCES += mmn_dependancy.h
SOURCES += mmn_dp_solver.cxx
SOURCES += mmn_graph_rep1.cxx
SOURCES += mmn_make_tri_tree.cxx
MANPAGE_SOURCES += mmn_solve.h

#######################################
include ${VXLROOT}/config/top-params.mk
#######################################

ifndef NOTEST
SUBDIRS += tests
endif

######################################
include ${VXLROOT}/config/top-rules.mk
######################################
