USES += contrib/brl contrib/brl/bbas contrib/brl/bseg core
include ../config.mk
include ${IUEROOT}/core/testlib/config.mk

TESTMAIN := test_driver.cxx
TESTS += test_histogram.cxx
TESTS += test_mutual_info.cxx
TESTS += test_watershed.cxx
TESTS += test_fourier.cxx
TESTS += test_homography.cxx
TESTS += test_lucas_kanade.cxx

MINI_PROG_SOURCES += test_include.cxx

#########################################
include ${IUEROOT}/config/static-tests.mk
#########################################
