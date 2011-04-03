USES += contrib/brl contrib/gel contrib/brl/bbas contrib/brl/bseg core
include ../config.mk
include ${VXLROOT}/contrib/gel/vtol/config.mk
include ${VXLROOT}/core/vil1/config.mk
include ${VXLROOT}/core/testlib/config.mk

TESTMAIN := test_driver.cxx
TESTS += test_detector.cxx
TESTS += test_region_proc.cxx
TESTS += test_gauss_fit.cxx
TESTS += test_image_mesh.cxx
TESTS += test_denoise_mrf.cxx
TESTS += test_denoise_mrf_bp.cxx

PARAMS_test_detector := jar-closeup.tif
PARAMS_test_region_proc := jar-closeup.tif

MINI_PROG_SOURCES += test_include.cxx

#########################################
include ${VXLROOT}/config/static-tests.mk
#########################################
