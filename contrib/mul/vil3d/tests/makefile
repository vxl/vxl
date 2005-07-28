USES += contrib/mul core
include ../algo/config.mk
include ${IUEROOT}/core/testlib/config.mk
include ${IUEROOT}/core/vpl/config.mk

# This is where vil/vil_config.h resides
INCDIRS += ${IUEROOT}/core/vil/$(RELOBJDIR)/include

TESTMAIN = test_driver.cxx

TESTS += test_gauss_reduce.cxx
TESTS += test_image_view.cxx
TESTS += test_sample_profile_trilin.cxx
TESTS += test_trilin_interp.cxx
TESTS += test_algo_threshold.cxx
TESTS += test_algo_binary_dilate.cxx
TESTS += test_algo_binary_erode.cxx
TESTS += test_algo_structuring_element.cxx
TESTS += test_algo_exp_filter.cxx
TESTS += test_switch_axes.cxx
TESTS += test_algo_grad_1x3.cxx
TESTS += test_math.cxx
TESTS += test_image_resource.cxx
TESTS += test_algo_normalised_correlation_3d.cxx
TESTS += test_file_format_read.cxx
PARAMS_test_file_format_read := file_read_data
TESTS += test_convert.cxx
TESTS += test_algo_convolve_1d.cxx
TESTS += test_algo_histogram.cxx
TESTS += test_algo_histogram_equalise.cxx
TESTS += test_from_image_2d.cxx
TESTS += test_algo_anisotropic_filter.cxx
TESTS += test_resample.cxx
TESTS += test_analyze_format.cxx

MINI_PROG_SOURCES += test_include.cxx

#########################################
include ${IUEROOT}/config/static-tests.mk
#########################################
