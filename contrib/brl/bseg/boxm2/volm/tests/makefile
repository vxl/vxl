USES += contrib/brl contrib/brl/bseg core
include ../config.mk
include $(VXLROOT)/core/testlib/config.mk

TESTMAIN := test_driver.cxx
TESTS += test_volm_wr3db_ind.cxx
TESTS += test_volm_locations.cxx
TESTS += test_volm_matcher_p1.cxx
TESTS += test_volm_matcher_p0.cxx

MINI_PROG_SOURCES += test_include.cxx

#########################################
include ${VXLROOT}/config/static-tests.mk
#########################################
