USES += contrib/brl contrib/brl/bseg core
include ../config.mk
include ${VXLROOT}/core/testlib/config.mk

TESTMAIN := test_driver.cxx
TESTS += test_bvxm_densities.cxx
TESTS += test_bvxm_hierarchy_builder.cxx
TESTS += test_bvxm_parts.cxx
TESTS += test_param_estimation.cxx
TESTS += test_bvxm_hierarchy_detector.cxx

MINI_PROG_SOURCES += test_include.cxx

#########################################
include ${VXLROOT}/config/static-tests.mk
#########################################
