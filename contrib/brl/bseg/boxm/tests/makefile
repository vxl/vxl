USES += contrib/brl contrib/brl/bseg core
include ../config.mk
include $(VXLROOT)/core/testlib/config.mk

TESTMAIN := test_driver.cxx
SOURCES += test_utils.cxx
TESTS += test_binary_io.cxx
TESTS += test_block_iter.cxx
TESTS += test_block_vis_graph.cxx
TESTS += test_cell_vis_graph.cxx
TESTS += test_render_image.cxx
TESTS += test_quad_interpolate.cxx
TESTS += test_update.cxx
TESTS += test_save_raw.cxx

MINI_PROG_SOURCES += test_include.cxx

#########################################
include ${VXLROOT}/config/static-tests.mk
#########################################
