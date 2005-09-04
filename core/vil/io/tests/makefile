USES += core
include ../config.mk
include $(VXLROOT)/core/vpl/config.mk
include ${VXLROOT}/core/testlib/config.mk

TESTMAIN := test_driver.cxx
TESTS += test_image_view_io.cxx
TESTS += test_memory_chunk_io.cxx

MINI_PROG_SOURCES += test_include.cxx

#########################################
include ${VXLROOT}/config/static-tests.mk
#########################################
