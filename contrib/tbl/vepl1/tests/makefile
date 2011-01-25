USES += contrib/tbl core
include ../config.mk
include ${VXLROOT}/core/vil1/config.mk
include ${VXLROOT}/core/testlib/config.mk

# Do not instantiate templates in the library but implicitly, on use:
USE_IMPLICIT_TEMPLATES := 1

TESTMAIN := test_driver.cxx
TESTS += vepl1_test_threshold.cxx
TESTS += vepl1_test_erode_disk.cxx

MINI_PROG_SOURCES += test_include.cxx
MINI_PROG_SOURCES += test_template_include.cxx

#########################################
include ${VXLROOT}/config/static-tests.mk
#########################################
