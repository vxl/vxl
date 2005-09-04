USES += contrib/mul core
include ../config.mk
include $(VXLROOT)/core/vpl/config.mk
include $(VXLROOT)/core/testlib/config.mk

TESTMAIN := test_driver.cxx

TESTS += test_axis_gaussian.cxx
TESTS += test_gaussian.cxx
TESTS += test_mixture.cxx
TESTS += test_pc_gaussian.cxx

MINI_PROG_SOURCES += test_include.cxx

#########################################
include ${VXLROOT}/config/static-tests.mk
#########################################
