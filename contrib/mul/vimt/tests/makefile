USES += contrib/mul core
include ../config.mk
include $(IUEROOT)/core/vpl/config.mk
include ${IUEROOT}/core/testlib/config.mk

TESTMAIN := test_driver.cxx
TESTS += test_scale_pyramid_builder_2d.cxx
TESTS += test_gaussian_pyramid_builder_2d.cxx
TESTS += test_gaussian_pyramid_builder_2d_general.cxx
TESTS += test_image_2d_of.cxx
TESTS += test_sample_grid_bilin.cxx
TESTS += test_sample_profile_bilin.cxx
TESTS += test_transform_2d.cxx
TESTS += test_convolve_2d.cxx
TESTS += test_find_troughs.cxx
TESTS += test_find_peaks.cxx

#########################################
include ${IUEROOT}/config/static-tests.mk
#########################################
