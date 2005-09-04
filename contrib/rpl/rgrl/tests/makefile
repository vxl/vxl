USES += contrib/rpl contrib/mul core
include ../config.mk
include $(VXLROOT)/core/testlib/config.mk

TESTMAIN := test_driver.cxx
SOURCES += test_util.cxx
TESTS += test_convergence.cxx
TESTS += test_estimator.cxx
TESTS += test_feature.cxx
TESTS += test_feature_set.cxx
TESTS += test_initializer.cxx
TESTS += test_initializer_ran_sam.cxx
TESTS += test_match_set.cxx
TESTS += test_matcher.cxx
TESTS += test_rgrl_util.cxx
TESTS += test_scale_est.cxx
TESTS += test_set_of.cxx
TESTS += test_spline.cxx
TESTS += test_trans_reader.cxx
PARAMS_test_trans_reader := affine_xform.txt
TESTS += test_transformation.cxx
TESTS += test_feature_region.cxx

MINI_PROG_SOURCES += test_include.cxx

#########################################
include ${VXLROOT}/config/static-tests.mk
#########################################
