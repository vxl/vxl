USES = vxl mul
include ${IUEROOT}/mul/vil2/algo/config.mk
include ${IUEROOT}/vxl/vul/config.mk
include ${IUEROOT}/vxl/vpl/config.mk
include ${IUEROOT}/vxl/testlib/config.mk

TESTMAIN = test_driver.cxx

TESTS += test_file_format_read.cxx
PARAMS_test_file_format_read := file_read_data
TESTS += test_save_load_image.cxx
TESTS += test_bilin_interp.cxx
TESTS += test_image_view.cxx
TESTS += test_algo_gauss_reduce.cxx
TESTS += test_sample_profile_bilin.cxx
TESTS += test_algo_convolve_1d.cxx
TESTS += test_sample_grid_bilin.cxx
TESTS += test_algo_exp_filter_1d.cxx

#########################################
include ${IUEROOT}/config/static-tests.mk
#########################################
