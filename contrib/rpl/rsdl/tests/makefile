USES += contrib/rpl contrib/mul core
include ../config.mk
include $(IUEROOT)/contrib/mul/mbl/config.mk
include $(IUEROOT)/core/testlib/config.mk

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
include ${IUEROOT}/config/static-tests.mk
#########################################
