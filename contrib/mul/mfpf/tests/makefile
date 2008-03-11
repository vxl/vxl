USES += contrib/mul core
include ../config.mk
include $(VXLROOT)/core/testlib/config.mk

TESTMAIN := test_driver.cxx
TESTS += test_edge_finder.cxx
TESTS += test_norm_corr1d.cxx
TESTS += test_norm_corr2d.cxx
TESTS += test_profile_pdf.cxx
TESTS += test_region_pdf.cxx
TESTS += test_pose.cxx

MINI_PROG_SOURCES += test_include.cxx

#########################################
include ${VXLROOT}/config/static-tests.mk
#########################################
