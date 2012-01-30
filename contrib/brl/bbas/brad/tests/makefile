USES += contrib/brl contrib/brl/bbas core
include ../config.mk
include ${VXLROOT}/core/testlib/config.mk

TESTMAIN := test_driver.cxx
TESTS += test_illum.cxx
TESTS += test_sun_dir_index.cxx
TESTS += test_sun_hist.cxx
TESTS += test_sun_pos.cxx
TESTS += test_feature_pca.cxx
TESTS += test_phongs_model_est.cxx
TESTS += test_synoptic_function.cxx
TESTS += test_image_metadata.cxx
TESTS += test_atmospheric_parameters.cxx

MINI_PROG_SOURCES += test_include.cxx
MINI_PROG_SOURCES += test_template_include.cxx

#########################################
include ${VXLROOT}/config/static-tests.mk
#########################################
