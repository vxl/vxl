USES += contrib/brl contrib/gel contrib/brl/bbas contrib/brl/bseg core
include ../config.mk
include ${IUEROOT}/contrib/gel/vtol/config.mk
include ${IUEROOT}/core/vil1/config.mk
include ${IUEROOT}/core/testlib/config.mk

TESTMAIN := test_driver.cxx
TESTS += test_detector.cxx
TESTS += test_region_proc.cxx

PARAMS_test_detector := jar-closeup.tif
PARAMS_test_region_proc := jar-closeup.tif

MINI_PROG_SOURCES += test_include.cxx

#########################################
include ${IUEROOT}/config/static-tests.mk
#########################################
