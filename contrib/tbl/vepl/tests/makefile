USES += contrib/tbl core

include ../config.mk
include ${IUEROOT}/core/testlib/config.mk

# Do not instantiate templates in the library but implicitly, on use:
USE_IMPLICIT_TEMPLATES := 1

TESTMAIN := test_driver.cxx
TESTS += vepl_test_threshold.cxx
TESTS += vepl_test_erode_disk.cxx

#########################################
include ${IUEROOT}/config/static-tests.mk
#########################################
