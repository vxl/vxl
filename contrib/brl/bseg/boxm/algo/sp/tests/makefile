USES += contrib/brl contrib/brl/bseg core
include ../config.mk
include $(VXLROOT)/core/testlib/config.mk

TESTMAIN := test_driver.cxx

TESTS += test_render_image.cxx
TESTS += test_update_multi_bin.cxx
TESTS += test_compute_visibility.cxx
TESTS += test_update.cxx

MINI_PROG_SOURCES += test_include.cxx

#########################################
include ${VXLROOT}/config/static-tests.mk
#########################################
