# This is core/vil/tests/CMakeLists.txt

ADD_EXECUTABLE( vil_test_all
  test_driver.cxx

  # basic things
  test_image_resource.cxx
  test_image_view.cxx
  test_memory_chunk.cxx

  # file format readers/writers
  test_file_format_read.cxx
  test_save_load_image.cxx
  test_image_loader_robustness.cxx
  test_stream.cxx

  # image operations
  test_deep_copy_3_plane.cxx
  test_image_view_maths.cxx
  test_convert.cxx
  test_rotate_image.cxx
  test_warp.cxx

  # Sampling Operations
  test_bilin_interp.cxx
  test_sample_profile_bilin.cxx
  test_sample_grid_bilin.cxx
  test_resample_bilin.cxx
  test_bicub_interp.cxx
  test_sample_profile_bicub.cxx
  test_sample_grid_bicub.cxx
  test_resample_bicub.cxx

  # vil/algo
  test_algo_gauss_filter.cxx
  test_algo_gauss_reduce.cxx
  test_algo_convolve_1d.cxx
  test_algo_convolve_2d.cxx
  test_algo_correlate_1d.cxx
  test_algo_correlate_2d.cxx
  test_algo_exp_filter_1d.cxx
  test_pixel_format.cxx
  test_binary_erode.cxx
  test_binary_dilate.cxx
  test_greyscale_dilate.cxx
  test_greyscale_erode.cxx
  test_median.cxx
  test_algo_exp_grad_filter_1d.cxx
  test_algo_line_filter.cxx
  test_algo_threshold.cxx
  test_algo_grid_merge.cxx
  test_algo_find_4con_boundary.cxx
  test_algo_trace_4con_boundary.cxx
  test_algo_trace_8con_boundary.cxx
  test_algo_fft.cxx
  test_suppress_non_max.cxx
  test_algo_histogram.cxx
  test_algo_histogram_equalise.cxx
  test_algo_distance_transform.cxx
  test_algo_blob_finder.cxx
  test_algo_find_peaks.cxx
  test_algo_region_finder.cxx
  test_algo_cartesian_differential_invariants.cxx
  test_algo_corners.cxx
)

IF(CMAKE_COMPILER_IS_GNUCXX)
  SET_SOURCE_FILES_PROPERTIES(test_algo_convolve_1d.cxx PROPERTIES COMPILE_FLAGS -O0)
ENDIF(CMAKE_COMPILER_IS_GNUCXX)

TARGET_LINK_LIBRARIES( vil_test_all vil_algo vil vpl vul testlib vcl )

# basic things
ADD_TEST( vil_test_image_resource           vil_test_all test_image_resource)
ADD_TEST( vil_test_image_view               vil_test_all test_image_view)
ADD_TEST( vil_test_memory_chunk             vil_test_all test_memory_chunk)

# file format readers/writers
ADD_TEST( vil_test_file_format_read         vil_test_all test_file_format_read ${CMAKE_CURRENT_SOURCE_DIR}/file_read_data)
ADD_TEST( vil_test_save_load_image          vil_test_all test_save_load_image)
ADD_TEST( vil_test_image_loader_robustness  vil_test_all test_image_loader_robustness)
ADD_TEST( vil_test_stream                   vil_test_all test_stream)

# image operations
ADD_TEST( vil_test_deep_copy_3_plane        vil_test_all test_deep_copy_3_plane )
ADD_TEST( vil_test_image_view_maths         vil_test_all test_image_view_maths)
ADD_TEST( vil_test_convert                  vil_test_all test_convert ${CMAKE_CURRENT_SOURCE_DIR}/file_read_data)
ADD_TEST( vil_test_rotate_image             vil_test_all test_rotate_image)
ADD_TEST( vil_test_warp                     vil_test_all test_warp)

# Sampling Operations
ADD_TEST( vil_test_bilin_interp             vil_test_all test_bilin_interp)
ADD_TEST( vil_test_sample_profile_bilin     vil_test_all test_sample_profile_bilin)
ADD_TEST( vil_test_sample_grid_bilin        vil_test_all test_sample_grid_bilin)
ADD_TEST( vil_test_resample_bilin           vil_test_all test_resample_bilin)
ADD_TEST( vil_test_bicub_interp             vil_test_all test_bicub_interp)
ADD_TEST( vil_test_sample_profile_bicub     vil_test_all test_sample_profile_bicub)
ADD_TEST( vil_test_sample_grid_bicub        vil_test_all test_sample_grid_bicub)
ADD_TEST( vil_test_resample_bicub           vil_test_all test_resample_bicub)

# vil/algo
ADD_TEST( vil_test_algo_gauss_filter        vil_test_all test_algo_gauss_filter)
ADD_TEST( vil_test_algo_gauss_reduce        vil_test_all test_algo_gauss_reduce)
ADD_TEST( vil_test_algo_convolve_1d         vil_test_all test_algo_convolve_1d)
ADD_TEST( vil_test_algo_convolve_2d         vil_test_all test_algo_convolve_2d)
ADD_TEST( vil_test_algo_correlate_1d        vil_test_all test_algo_correlate_1d)
ADD_TEST( vil_test_algo_correlate_2d        vil_test_all test_algo_correlate_2d)
ADD_TEST( vil_test_algo_exp_filter_1d       vil_test_all test_algo_exp_filter_1d)
ADD_TEST( vil_test_pixel_format             vil_test_all test_pixel_format)
ADD_TEST( vil_test_binary_erode             vil_test_all test_binary_erode)
ADD_TEST( vil_test_binary_dilate            vil_test_all test_binary_dilate)
ADD_TEST( vil_test_greyscale_dilate         vil_test_all test_greyscale_dilate)
ADD_TEST( vil_test_greyscale_erode          vil_test_all test_greyscale_erode)
ADD_TEST( vil_test_median                   vil_test_all test_median)
ADD_TEST( vil_test_algo_exp_grad_filter_1d  vil_test_all test_algo_exp_grad_filter_1d)
ADD_TEST( vil_test_algo_line_filter         vil_test_all test_algo_line_filter)
ADD_TEST( vil_test_algo_threshold           vil_test_all test_algo_threshold)
ADD_TEST( vil_test_algo_grid_merge          vil_test_all test_algo_grid_merge )
ADD_TEST( vil_test_algo_find_4con_boundary  vil_test_all test_algo_find_4con_boundary )
ADD_TEST( vil_test_algo_trace_4con_boundary vil_test_all test_algo_trace_4con_boundary )
ADD_TEST( vil_test_algo_trace_8con_boundary vil_test_all test_algo_trace_8con_boundary )
ADD_TEST( vil_test_algo_fft                 vil_test_all test_algo_fft )
ADD_TEST( vil_test_suppress_non_max         vil_test_all test_suppress_non_max )
ADD_TEST( vil_test_algo_histogram           vil_test_all test_algo_histogram )
ADD_TEST( vil_test_algo_histogram_equalise  vil_test_all test_algo_histogram_equalise )
ADD_TEST( vil_test_algo_distance_transform  vil_test_all test_algo_distance_transform )
ADD_TEST( vil_test_algo_blob_finder         vil_test_all test_algo_blob_finder )
ADD_TEST( vil_test_algo_find_peaks          vil_test_all test_algo_find_peaks )
ADD_TEST( vil_test_algo_region_finder       vil_test_all test_algo_region_finder )
ADD_TEST( vil_test_algo_corners             vil_test_all test_algo_corners )
ADD_TEST( vil_test_algo_cartesian_differential_invariants vil_test_all test_algo_cartesian_differential_invariants )

# To run timing tests
#ADD_EXECUTABLE( vil_test_access_timings vil_test_access_timings.cxx)
#TARGET_LINK_LIBRARIES( vil_test_access_timings vil mbl vcl )
#ADD_TEST( vil_test_access_timings vil_test_access_timings )

#ADD_EXECUTABLE( vil_test_access_timings_noplanes vil_test_access_timings_noplanes.cxx)
#TARGET_LINK_LIBRARIES( vil_test_access_timings_noplanes vil mbl vcl )
#ADD_TEST( vil_test_access_timings_noplanes vil_test_access_timings_noplanes )

#ADD_EXECUTABLE( vil_vil_compare_access_timings vil_vil_compare_access_timings.cxx)
#TARGET_LINK_LIBRARIES( vil_vil_compare_access_timings vil mbl vcl )
#ADD_TEST( vil_vil_compare_access_timings vil_vil_compare_access_timings )

#ADD_EXECUTABLE( vil_test_pair_access_timings vil_test_pair_access_timings.cxx)
#TARGET_LINK_LIBRARIES( vil_test_pair_access_timings vil mbl vcl )
#ADD_TEST( vil_test_pair_access_timings vil_test_pair_access_timings )

#ADD_EXECUTABLE( vil_test_random_access_timings vil_test_random_access_timings.cxx)
#TARGET_LINK_LIBRARIES( vil_test_random_access_timings vil mbl vcl )
#ADD_TEST( vil_test_random_access_timings vil_test_random_access_timings )

#ADD_EXECUTABLE( vil_mil_gauss_general_timings vil_mil_gauss_general_timings.cxx)
#TARGET_LINK_LIBRARIES( vil_mil_gauss_general_timings vil_algo mil vpl vul mbl testlib vcl )
#ADD_TEST( vil_mil_gauss_general_timings vil_mil_gauss_general_timings )

#ADD_EXECUTABLE( vil_vil_compare_function_timings vil_vil_compare_function_timings.cxx)
#TARGET_LINK_LIBRARIES( vil_vil_compare_function_timings vil mbl vcl )
#ADD_TEST( vil_vil_compare_function_timings vil_vil_compare_function_timings )

ADD_EXECUTABLE( vil_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( vil_test_include vil_algo vil )
