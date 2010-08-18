USES += contrib/brl contrib/brl/bseg core
include ../config.mk
include $(VXLROOT)/core/testlib/config.mk

TESTMAIN := test_driver.cxx
SOURCES += test_utils.cxx

TESTS += test_binary_io.cxx
TESTS += test_block_iter.cxx
TESTS += test_cell_iterator.cxx
TESTS += test_region_finder.cxx
TESTS += test_load_neighboring_blocks.cxx

MINI_PROG_SOURCES += test_include.cxx

#########################################
include ${VXLROOT}/config/static-tests.mk
#########################################
