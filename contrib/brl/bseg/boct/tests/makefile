USES += contrib/brl contrib/brl/bbas contrib/brl/bseg core
include ../config.mk
include ${VXLROOT}/core/testlib/config.mk

TESTMAIN := test_driver.cxx

TESTS += test_find_neighbors.cxx
TESTS += test_locate_point.cxx
TESTS += test_create_tree.cxx
TESTS += test_locate_region.cxx
TESTS += test_binary_io.cxx
TESTS += test_clone_tree.cxx
TESTS += test_tree_cell_reader.cxx

MINI_PROG_SOURCES += test_include.cxx

#########################################
include ${VXLROOT}/config/static-tests.mk
#########################################
