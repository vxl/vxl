USES += contrib/brl contrib/brl/bseg core
include ../config.mk
include $(VXLROOT)/core/testlib/config.mk

TESTMAIN := test_driver.cxx
SOURCES += test_utils.cxx

TESTS += test_scene.cxx
TESTS += test_wrappers.cxx
TESTS += test_cache.cxx
TESTS += test_io.cxx
TESTS += test_block.cxx
TESTS += test_data.cxx
TESTS += test_cpp_render_process.cxx
TESTS += test_cpp_update_process.cxx

MINI_PROG_SOURCES += test_include.cxx

#########################################
include ${VXLROOT}/config/static-tests.mk
#########################################
