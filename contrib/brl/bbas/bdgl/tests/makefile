USES += contrib/brl contrib/gel contrib/brl/bbas core
include ../config.mk
include ${IUEROOT}/contrib/gel/vdgl/config.mk
include ${IUEROOT}/core/vnl/config.mk
include ${IUEROOT}/core/testlib/config.mk

TESTMAIN := test_driver.cxx

TESTS += test_curve_algs.cxx
TESTS += test_region_algs.cxx

MINI_PROG_SOURCES += test_include.cxx

#########################################
include ${IUEROOT}/config/static-tests.mk
#########################################
