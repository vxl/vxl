USES += contrib/gel core
include ../config.mk
include $(IUEROOT)/core/testlib/config.mk
include $(IUEROOT)/core/vpl/config.mk

TESTMAIN := test_driver.cxx
TESTS += test_gevd_bufferxy.cxx
TESTS += test_gevd_memory_mixin.cxx
TESTS += test_gevd_param_mixin.cxx
TESTS += test_gevd_noise.cxx
TESTS += test_gevd_float_operators.cxx

MINI_PROG_SOURCES += test_include.cxx

#########################################
include ${IUEROOT}/config/static-tests.mk
#########################################

