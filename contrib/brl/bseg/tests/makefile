USES += contrib/brl contrib/gel contrib/brl/bbas contrib/brl/bseg core
include ../sdet/config.mk
include ../brip/config.mk

TESTS += sdet_test_detector.cxx
TESTS += sdet_test_region_proc.cxx
TESTS += brip_test_lucas_kanade.cxx

PARAMS_sdet_test_detector := jar-closeup.tif
PARAMS_sdet_test_region_proc := jar-closeup.tif

MINI_PROG_SOURCES += test_include.cxx

#########################################
include ${IUEROOT}/config/static-tests.mk
#########################################
