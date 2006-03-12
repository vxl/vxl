USES += contrib/brl contrib/brl/bbas core
include ../config.mk
include ${VXLROOT}/core/testlib/config.mk

TESTMAIN := test_driver.cxx

TESTS += test_pixel_format.cxx
TESTS += test_convert.cxx
TESTS += test_pixel_iterator.cxx
TESTS += test_color.cxx

MINI_PROG_SOURCES += test_include.cxx

#########################################
include ${VXLROOT}/config/static-tests.mk
#########################################
