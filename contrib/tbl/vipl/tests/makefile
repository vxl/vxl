USES += contrib/tbl contrib/mul core

include ${IUEROOT}/contrib/mul/mil/config.mk
include ../config.mk
include ${IUEROOT}/core/vnl/algo/config.mk
include ${IUEROOT}/core/vnl/io/config.mk
include ${IUEROOT}/core/vgl/io/config.mk
include ${IUEROOT}/core/testlib/config.mk

# Do not instantiate templates in the library but implicitly, on use:
USE_IMPLICIT_TEMPLATES := 1

TESTMAIN := test_driver.cxx
TESTS += vipl_test_threshold.cxx
TESTS += vipl_test_erode_disk.cxx
TESTS += vipl_test_gaussian_convolution.cxx
TESTS += vipl_test_histogram.cxx

#########################################
include ${IUEROOT}/config/static-tests.mk
#########################################
