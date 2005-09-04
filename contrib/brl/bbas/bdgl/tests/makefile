USES += contrib/brl contrib/gel contrib/brl/bbas core
include ../config.mk
include ${VXLROOT}/contrib/gel/vdgl/config.mk
include ${VXLROOT}/core/vnl/config.mk
include ${VXLROOT}/core/testlib/config.mk

TESTMAIN := test_driver.cxx

TESTS += test_curve_algs.cxx
TESTS += test_region_algs.cxx

MINI_PROG_SOURCES += test_include.cxx

#########################################
include ${VXLROOT}/config/static-tests.mk
#########################################
