USES += contrib/brl contrib/brl/bbas contrib/brl/bseg core
include ../config.mk
include ${IUEROOT}/core/vpl/config.mk
include ${IUEROOT}/core/testlib/config.mk

TESTMAIN := test_driver.cxx
TESTS += test_epi_point.cxx
TESTS += test_epi_seg.cxx
TESTS += test_node.cxx
TESTS += test_network.cxx

MINI_PROG_SOURCES += test_include.cxx

#########################################
include ${IUEROOT}/config/static-tests.mk
#########################################
