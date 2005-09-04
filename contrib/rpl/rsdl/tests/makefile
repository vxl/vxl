USES += contrib/rpl core
include ../config.mk
include $(VXLROOT)/core/vnl/config.mk
include $(VXLROOT)/core/testlib/config.mk

TESTMAIN := test_driver.cxx
TESTS += test_borgefors.cxx
TESTS += test_bins_2d.cxx
TESTS += test_bounding_box.cxx
TESTS += test_dist.cxx
TESTS += test_kd_tree.cxx
TESTS += test_point.cxx
TESTS += test_bins.cxx

MINI_PROG_SOURCES += test_include.cxx

#########################################
include ${VXLROOT}/config/static-tests.mk
#########################################
