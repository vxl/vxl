USES += core
include ../config.mk
include ../algo/config.mk
include ../io/config.mk
include ${IUEROOT}/core/vul/config.mk
include ${IUEROOT}/core/vpl/config.mk
include ${IUEROOT}/core/testlib/config.mk

TESTMAIN = test_driver.cxx

# basic things
TESTS += test_image_resource.cxx
TESTS += test_image_view.cxx
TESTS += test_memory_chunk.cxx

# file format readers/writers
TESTS += test_file_format_read.cxx
PARAMS_test_file_format_read := file_read_data
TESTS += test_save_load_image.cxx

# image operations
TESTS += test_deep_copy_3_plane.cxx
TESTS += test_image_view_maths.cxx
TESTS += test_convert.cxx
PARAMS_test_convert := file_read_data
TESTS += test_rotate_image.cxx
TESTS += test_warp.cxx

# Sampling Operations
TESTS += test_bilin_interp.cxx
TESTS += test_sample_profile_bilin.cxx
TESTS += test_sample_grid_bilin.cxx
TESTS += test_resample_bilin.cxx
TESTS += test_bicub_interp.cxx
TESTS += test_sample_profile_bicub.cxx
TESTS += test_sample_grid_bicub.cxx
TESTS += test_resample_bicub.cxx

# vil/algo
TESTS += test_algo_gauss_filter.cxx
TESTS += test_algo_gauss_reduce.cxx
TESTS += test_algo_convolve_1d.cxx
TESTS += test_algo_convolve_2d.cxx
TESTS += test_algo_correlate_1d.cxx
TESTS += test_algo_correlate_2d.cxx
TESTS += test_algo_exp_filter_1d.cxx
TESTS += test_pixel_format.cxx
TESTS += test_binary_erode.cxx
TESTS += test_binary_dilate.cxx
TESTS += test_greyscale_dilate.cxx
TESTS += test_greyscale_erode.cxx
TESTS += test_median.cxx
TESTS += test_algo_exp_grad_filter_1d.cxx
TESTS += test_algo_line_filter.cxx
TESTS += test_algo_threshold.cxx
TESTS += test_algo_grid_merge.cxx
TESTS += test_algo_find_4con_boundary.cxx
TESTS += test_algo_fft.cxx
TESTS += test_suppress_non_max.cxx
TESTS += test_algo_histogram.cxx
TESTS += test_algo_histogram_equalise.cxx
TESTS += test_algo_distance_transform.cxx
TESTS += test_algo_trace_4con_boundary.cxx
TESTS += test_algo_blob_finder.cxx
TESTS += test_algo_trace_8con_boundary.cxx
TESTS += test_algo_find_peaks.cxx
TESTS += test_algo_region_finder.cxx
TESTS += test_algo_cartesian_differential_invariants.cxx
TESTS += test_algo_corners.cxx

#MINI_PROG_SOURCES += vil_mil_gauss_general_timings.cxx
#MINI_PROG_SOURCES += vil_vil_compare_access_timings.cxx
#MINI_PROG_SOURCES += vil_test_access_timings.cxx
#MINI_PROG_SOURCES += vil_test_random_access_timings.cxx
#MINI_PROG_SOURCES += vil_test_access_timings_noplanes.cxx
#MINI_PROG_SOURCES += vil_test_pair_access_timings.cxx
#MINI_PROG_SOURCES += vil_vil_compare_function_timings.cxx

MINI_PROG_SOURCES += test_include.cxx

#########################################
include ${IUEROOT}/config/static-tests.mk
#########################################

ifeq ($(OS),OSF1V4) # with optimisation, gcc 3.x runs out of memory
 FLAGS_test_algo_convolve_1d.cxx := -O0
endif
