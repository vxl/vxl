USES += contrib/brl contrib/gel contrib/brl/bbas contrib/brl/bseg core
include ../sdet/config.mk
include ../brip/config.mk
include ${IUEROOT}/core/testlib/config.mk

TESTMAIN := test_driver.cxx
TESTS += sdet_test_detector.cxx
TESTS += sdet_test_region_proc.cxx
TESTS += brip_test_lucas_kanade.cxx
TESTS += brip_test_fourier.cxx
TESTS += brip_test_homography.cxx

PARAMS_sdet_test_detector := jar-closeup.tif
PARAMS_sdet_test_region_proc := jar-closeup.tif

MINI_PROG_SOURCES += test_include.cxx

#########################################
include ${IUEROOT}/config/static-tests.mk
#########################################
