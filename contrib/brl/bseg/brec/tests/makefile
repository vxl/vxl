USES += contrib/brl contrib/brl/bbas contrib/brl/bseg core
include ../config.mk
include ${VXLROOT}/core/testlib/config.mk

TESTMAIN := test_driver.cxx
TESTS += test_param_estimation.cxx
TESTS += test_brec_hierarchy_detector.cxx
TESTS += test_brec_hierarchy_builder.cxx
TESTS += test_brec_parts.cxx
TESTS += test_brec_densities.cxx
TESTS += test_brec_part_hierarchy_learner.cxx
TESTS += test_brec_hierarchy_detector_w_models.cxx

MINI_PROG_SOURCES += test_include.cxx

#########################################
include ${VXLROOT}/config/static-tests.mk
#########################################
