USES += contrib/gel core
include ../config.mk
include ../algo/config.mk
include ${VXLROOT}/core/testlib/config.mk

TESTMAIN := test_driver.cxx

TESTS += test_vertex_2d.cxx
TESTS += test_zero_chain.cxx
TESTS += test_edge_2d.cxx
TESTS += test_one_chain.cxx
TESTS += test_face_2d.cxx
TESTS += test_two_chain.cxx
TESTS += test_block.cxx
TESTS += vtol_test_refcounting.cxx
TESTS += test_cycle_processor.cxx
FLAGS_test_cycle_processor.cxx := -O1
TESTS += vtol_test_timing.cxx
TESTS += test_extract_topology.cxx

MINI_PROG_SOURCES += test_include.cxx

#########################################
include ${VXLROOT}/config/static-tests.mk
#########################################
