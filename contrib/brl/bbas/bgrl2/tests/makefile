USES += contrib/brl contrib/brl/bbas core
include ../config.mk
include ${VXLROOT}/core/testlib/config.mk

TESTMAIN := test_driver.cxx
MANPAGE_SOURCES += test_edge.h
MANPAGE_SOURCES += test_edge_sptr.h
MANPAGE_SOURCES += test_graph.h
MANPAGE_SOURCES += test_graph_sptr.h
MANPAGE_SOURCES += test_node.h
MANPAGE_SOURCES += test_node_sptr.h

TESTS += graph_test.cxx

MINI_PROG_SOURCES += test_include.cxx
MINI_PROG_SOURCES += test_template_include.cxx

#########################################
include ${VXLROOT}/config/static-tests.mk
#########################################
