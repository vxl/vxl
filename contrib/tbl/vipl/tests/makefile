USES += contrib/tbl contrib/mul core

include ../config.mk
include ${VXLROOT}/core/vnl/algo/config.mk
include ${VXLROOT}/core/vnl/io/config.mk
include ${VXLROOT}/core/vgl/io/config.mk
include ${VXLROOT}/core/testlib/config.mk

# Do not instantiate templates in the library but implicitly, on use:
USE_IMPLICIT_TEMPLATES := 1

TESTMAIN := test_driver.cxx
TESTS += vipl_test_threshold.cxx
TESTS += vipl_test_erode_disk.cxx
TESTS += vipl_test_gaussian_convolution.cxx
TESTS += vipl_test_histogram.cxx

MINI_PROG_SOURCES += test_include.cxx

#########################################
include ${VXLROOT}/config/static-tests.mk
#########################################
