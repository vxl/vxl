USES += contrib/brl contrib/brl/bbas contrib/brl/bseg core
include ../config.mk
include ${VXLROOT}/core/vpl/config.mk
include ${VXLROOT}/contrib/brl/bseg/brip/config.mk
include ${VXLROOT}/core/testlib/config.mk

TESTMAIN := test_driver.cxx
TESTS += test_epi_point.cxx
TESTS += test_epi_seg.cxx
TESTS += test_node.cxx
TESTS += test_network.cxx
TESTS += test_network_builder.cxx

MINI_PROG_SOURCES += test_include.cxx

#########################################
include ${VXLROOT}/config/static-tests.mk
#########################################
