USES += contrib/brl contrib/brl/bbas core
include ../config.mk
include ${VXLROOT}/core/testlib/config.mk

TESTMAIN := test_driver.cxx
TESTS += test_bil_cedt.cxx
TESTS += test_bil_detect_ridges.cxx
TESTS += test_bil_edt.cxx
TESTS += test_bil_finite_differences.cxx
TESTS += test_bil_finite_second_differences.cxx
TESTS += test_bil_scale_image.cxx
TESTS += test_bil_wshed2d.cxx

MINI_PROG_SOURCES += test_include.cxx

#########################################
include ${VXLROOT}/config/static-tests.mk
#########################################
