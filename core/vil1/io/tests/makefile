USES += core
include ../config.mk
include ${IUEROOT}/core/vpl/config.mk
include ${IUEROOT}/core/testlib/config.mk

TESTMAIN = test_driver.cxx

TESTS += test_memory_image_format_io.cxx
TESTS += test_memory_image_impl_io.cxx
TESTS += test_memory_image_of_io.cxx
TESTS += test_rgb_io.cxx
TESTS += test_rgba_io.cxx
TESTS += golden_test_vil_io.cxx

MINI_PROG_SOURCES += test_include.cxx

#########################################
include ${IUEROOT}/config/static-tests.mk
#########################################
