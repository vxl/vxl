# This is core/vil/algo/tests/CMakeLists.txt

ADD_EXECUTABLE( vil_algo_test_all
  test_driver.cxx
  test_algo_gauss_filter.cxx
  test_algo_gauss_reduce.cxx
  test_algo_convolve_1d.cxx
  test_algo_convolve_2d.cxx
  test_algo_correlate_1d.cxx
  test_algo_correlate_2d.cxx
  test_algo_exp_filter_1d.cxx
  test_algo_exp_grad_filter_1d.cxx
  test_algo_line_filter.cxx
  test_algo_threshold.cxx
  test_algo_grid_merge.cxx
  test_algo_find_4con_boundary.cxx
  test_algo_trace_4con_boundary.cxx
  test_algo_trace_8con_boundary.cxx
  test_algo_fft.cxx
  test_algo_histogram.cxx
  test_algo_histogram_equalise.cxx
  test_algo_distance_transform.cxx
  test_algo_blob_finder.cxx
  test_algo_find_peaks.cxx
  test_algo_region_finder.cxx
  test_algo_cartesian_differential_invariants.cxx
  test_algo_corners.cxx
  test_binary_erode.cxx
  test_binary_dilate.cxx
  test_greyscale_dilate.cxx
  test_greyscale_erode.cxx
  test_median.cxx
  test_suppress_non_max.cxx
)

IF(CMAKE_COMPILER_IS_GNUCXX)
  SET_SOURCE_FILES_PROPERTIES(test_algo_convolve_1d.cxx PROPERTIES COMPILE_FLAGS -O0)
ENDIF(CMAKE_COMPILER_IS_GNUCXX)

TARGET_LINK_LIBRARIES( vil_algo_test_all vil_algo vpl vul testlib vcl )


# vil/algo
ADD_TEST( vil_algo_test_gauss_filter        vil_algo_test_all test_algo_gauss_filter)
ADD_TEST( vil_algo_test_gauss_reduce        vil_algo_test_all test_algo_gauss_reduce)
ADD_TEST( vil_algo_test_convolve_1d         vil_algo_test_all test_algo_convolve_1d)
ADD_TEST( vil_algo_test_convolve_2d         vil_algo_test_all test_algo_convolve_2d)
ADD_TEST( vil_algo_test_correlate_1d        vil_algo_test_all test_algo_correlate_1d)
ADD_TEST( vil_algo_test_correlate_2d        vil_algo_test_all test_algo_correlate_2d)
ADD_TEST( vil_algo_test_exp_filter_1d       vil_algo_test_all test_algo_exp_filter_1d)
ADD_TEST( vil_algo_test_exp_grad_filter_1d  vil_algo_test_all test_algo_exp_grad_filter_1d)
ADD_TEST( vil_algo_test_line_filter         vil_algo_test_all test_algo_line_filter)
ADD_TEST( vil_algo_test_threshold           vil_algo_test_all test_algo_threshold)
ADD_TEST( vil_algo_test_grid_merge          vil_algo_test_all test_algo_grid_merge )
ADD_TEST( vil_algo_test_find_4con_boundary  vil_algo_test_all test_algo_find_4con_boundary )
ADD_TEST( vil_algo_test_trace_4con_boundary vil_algo_test_all test_algo_trace_4con_boundary )
ADD_TEST( vil_algo_test_trace_8con_boundary vil_algo_test_all test_algo_trace_8con_boundary )
ADD_TEST( vil_algo_test_fft                 vil_algo_test_all test_algo_fft )
ADD_TEST( vil_algo_test_histogram           vil_algo_test_all test_algo_histogram )
ADD_TEST( vil_algo_test_histogram_equalise  vil_algo_test_all test_algo_histogram_equalise )
ADD_TEST( vil_algo_test_distance_transform  vil_algo_test_all test_algo_distance_transform )
ADD_TEST( vil_algo_test_blob_finder         vil_algo_test_all test_algo_blob_finder )
ADD_TEST( vil_algo_test_find_peaks          vil_algo_test_all test_algo_find_peaks )
ADD_TEST( vil_algo_test_region_finder       vil_algo_test_all test_algo_region_finder )
ADD_TEST( vil_algo_test_corners             vil_algo_test_all test_algo_corners )
ADD_TEST( vil_algo_test_cartesian_differential_invariants vil_algo_test_all test_algo_cartesian_differential_invariants )
ADD_TEST( vil_algo_test_binary_erode        vil_algo_test_all test_binary_erode)
ADD_TEST( vil_algo_test_binary_dilate       vil_algo_test_all test_binary_dilate)
ADD_TEST( vil_algo_test_greyscale_dilate    vil_algo_test_all test_greyscale_dilate)
ADD_TEST( vil_algo_test_greyscale_erode     vil_algo_test_all test_greyscale_erode)
ADD_TEST( vil_algo_test_median              vil_algo_test_all test_median)
ADD_TEST( vil_algo_test_suppress_non_max    vil_algo_test_all test_suppress_non_max )



ADD_EXECUTABLE( vil_algo_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( vil_algo_test_include vil_algo )
