USES = vxl mul
include ${IUEROOT}/mul/vil2/config.mk
include ${IUEROOT}/mul/vil2/algo/config.mk
include ${IUEROOT}/mul/vil2/io/config.mk
include ${IUEROOT}/vxl/vul/config.mk
include ${IUEROOT}/vxl/vpl/config.mk
include ${IUEROOT}/vxl/testlib/config.mk

TESTMAIN = test_driver.cxx

TESTS += test_file_format_read.cxx
PARAMS_test_file_format_read := file_read_data
TESTS += test_save_load_image.cxx
TESTS += test_bilin_interp.cxx
TESTS += test_image_view.cxx
TESTS += test_image_resource.cxx
TESTS += test_algo_gauss_reduce.cxx
TESTS += test_sample_profile_bilin.cxx
TESTS += test_algo_convolve_1d.cxx
TESTS += test_algo_convolve_2d.cxx
TESTS += test_sample_grid_bilin.cxx
TESTS += test_algo_exp_filter_1d.cxx
TESTS += test_pixel_format.cxx
TESTS += test_image_view_maths.cxx
TESTS += test_memory_chunk.cxx
TESTS += test_convert.cxx
TESTS += test_binary_dilate.cxx
TESTS += test_binary_erode.cxx
TESTS += test_resample_bilin.cxx
TESTS += test_vil1.cxx
PARAMS_test_vil1 := file_read_data
TESTS += test_greyscale_dilate.cxx
TESTS += test_greyscale_erode.cxx
TESTS += test_median.cxx
TESTS += test_algo_exp_grad_filter_1d.cxx

include ${IUEROOT}/config/identify-os.mk
ifeq ($(OS),OSF1V4) # with optimisation, gcc 3.x runs out of memory
FLAGS_test_algo_convolve_1d.cxx := -O0
endif

#########################################
include ${IUEROOT}/config/static-tests.mk
#########################################
