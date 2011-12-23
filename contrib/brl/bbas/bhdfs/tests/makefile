USES += contrib/brl contrib/brl/bbas core
include ../config.mk
include ${VXLROOT}/core/testlib/config.mk

TESTMAIN := test_driver.cxx
TESTS += test_manager.cxx
TESTS += test_fstream.cxx
TESTS += test_vil_stream.cxx
TESTS += test_vil_save_load_image.cxx
TESTS += test_hadoop.cxx

MINI_PROG_SOURCES += test_include.cxx

#########################################
include ${VXLROOT}/config/static-tests.mk
#########################################
