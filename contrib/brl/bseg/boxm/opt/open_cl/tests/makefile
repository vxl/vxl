USES += contrib/brl contrib/brl/bseg core
include ../config.mk
include $(VXLROOT)/core/testlib/config.mk

TESTMAIN := test_driver.cxx
SOURCES += octree_test_driver.cxx
SOURCES += open_cl_test_data.cxx
TESTS += test_octree.cxx
TESTS += test_ray_trace.cxx

MINI_PROG_SOURCES += test_include.cxx

#########################################
include ${VXLROOT}/config/static-tests.mk
#########################################
