USES += contrib/mul core

LIBRARY := mmn
VERSION := 1.0

MANPAGE_SOURCES += mmn_analyze_graph.h
MANPAGE_SOURCES += mmn_arc.h
MANPAGE_SOURCES += mmn_dependancy.h
SOURCES += mmn_dp_solver.cxx
SOURCES += mmn_lbp_solver.cxx
SOURCES += mmn_graph_rep1.cxx
SOURCES += mmn_make_tri_tree.cxx
MANPAGE_SOURCES += mmn_solve.h
SOURCES += mmn_order_cost.cxx
MANPAGE_SOURCES += mmn_triplet.h
SOURCES += mmn_csp_solver.cxx
SOURCES += mmn_diffusion_solver.cxx
SOURCES += mmn_solver.cxx
SOURCES += mmn_parse_arcs.cxx
SOURCES += mmn_add_all_loaders.cxx

#######################################
include ${VXLROOT}/config/top-params.mk
#######################################

ifndef NOTEST
SUBDIRS += tests
endif

######################################
include ${VXLROOT}/config/top-rules.mk
######################################
