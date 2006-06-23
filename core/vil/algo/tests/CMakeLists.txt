# This is core/vil/algo/tests/CMakeLists.txt

ADD_EXECUTABLE( vil_algo_test_all
  test_driver.cxx
  test_algo_gauss_filter.cxx
  test_algo_gauss_reduce.cxx
  test_algo_colour_space.cxx
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
  test_algo_sobel.cxx
  test_algo_abs_shuffle_distance.cxx
  test_algo_suppress_non_max_edges.cxx
  test_algo_checker_board.cxx
  test_algo_quad_distance_function.cxx
)

IF(CMAKE_COMPILER_IS_GNUCXX)
  SET_SOURCE_FILES_PROPERTIES(test_algo_convolve_1d.cxx PROPERTIES COMPILE_FLAGS -O0)
ENDIF(CMAKE_COMPILER_IS_GNUCXX)

TARGET_LINK_LIBRARIES( vil_algo_test_all vil_algo testlib vcl )


# vil/algo

ADD_TEST( vil_algo_test_gauss_filter        ${EXECUTABLE_OUTPUT_PATH}/vil_algo_test_all test_algo_gauss_filter)
ADD_TEST( vil_algo_test_gauss_reduce        ${EXECUTABLE_OUTPUT_PATH}/vil_algo_test_all test_algo_gauss_reduce)
ADD_TEST( vil_algo_test_colour_space        ${EXECUTABLE_OUTPUT_PATH}/vil_algo_test_all test_algo_colour_space)
ADD_TEST( vil_algo_test_convolve_1d         ${EXECUTABLE_OUTPUT_PATH}/vil_algo_test_all test_algo_convolve_1d)
ADD_TEST( vil_algo_test_convolve_2d         ${EXECUTABLE_OUTPUT_PATH}/vil_algo_test_all test_algo_convolve_2d)
ADD_TEST( vil_algo_test_correlate_1d        ${EXECUTABLE_OUTPUT_PATH}/vil_algo_test_all test_algo_correlate_1d)
ADD_TEST( vil_algo_test_correlate_2d        ${EXECUTABLE_OUTPUT_PATH}/vil_algo_test_all test_algo_correlate_2d)
ADD_TEST( vil_algo_test_exp_filter_1d       ${EXECUTABLE_OUTPUT_PATH}/vil_algo_test_all test_algo_exp_filter_1d)
ADD_TEST( vil_algo_test_exp_grad_filter_1d  ${EXECUTABLE_OUTPUT_PATH}/vil_algo_test_all test_algo_exp_grad_filter_1d)
ADD_TEST( vil_algo_test_line_filter         ${EXECUTABLE_OUTPUT_PATH}/vil_algo_test_all test_algo_line_filter)
ADD_TEST( vil_algo_test_threshold           ${EXECUTABLE_OUTPUT_PATH}/vil_algo_test_all test_algo_threshold)
ADD_TEST( vil_algo_test_grid_merge          ${EXECUTABLE_OUTPUT_PATH}/vil_algo_test_all test_algo_grid_merge )
ADD_TEST( vil_algo_test_find_4con_boundary  ${EXECUTABLE_OUTPUT_PATH}/vil_algo_test_all test_algo_find_4con_boundary )
ADD_TEST( vil_algo_test_trace_4con_boundary ${EXECUTABLE_OUTPUT_PATH}/vil_algo_test_all test_algo_trace_4con_boundary )
ADD_TEST( vil_algo_test_trace_8con_boundary ${EXECUTABLE_OUTPUT_PATH}/vil_algo_test_all test_algo_trace_8con_boundary )
ADD_TEST( vil_algo_test_fft                 ${EXECUTABLE_OUTPUT_PATH}/vil_algo_test_all test_algo_fft )
ADD_TEST( vil_algo_test_histogram           ${EXECUTABLE_OUTPUT_PATH}/vil_algo_test_all test_algo_histogram )
ADD_TEST( vil_algo_test_histogram_equalise  ${EXECUTABLE_OUTPUT_PATH}/vil_algo_test_all test_algo_histogram_equalise )
ADD_TEST( vil_algo_test_distance_transform  ${EXECUTABLE_OUTPUT_PATH}/vil_algo_test_all test_algo_distance_transform )
ADD_TEST( vil_algo_test_blob_finder         ${EXECUTABLE_OUTPUT_PATH}/vil_algo_test_all test_algo_blob_finder )
ADD_TEST( vil_algo_test_find_peaks          ${EXECUTABLE_OUTPUT_PATH}/vil_algo_test_all test_algo_find_peaks )
ADD_TEST( vil_algo_test_region_finder       ${EXECUTABLE_OUTPUT_PATH}/vil_algo_test_all test_algo_region_finder )
ADD_TEST( vil_algo_test_corners             ${EXECUTABLE_OUTPUT_PATH}/vil_algo_test_all test_algo_corners )
ADD_TEST( vil_algo_test_cartesian_differential_invariants ${EXECUTABLE_OUTPUT_PATH}/vil_algo_test_all test_algo_cartesian_differential_invariants )
ADD_TEST( vil_algo_test_binary_erode        ${EXECUTABLE_OUTPUT_PATH}/vil_algo_test_all test_binary_erode)
ADD_TEST( vil_algo_test_binary_dilate       ${EXECUTABLE_OUTPUT_PATH}/vil_algo_test_all test_binary_dilate)
ADD_TEST( vil_algo_test_greyscale_dilate    ${EXECUTABLE_OUTPUT_PATH}/vil_algo_test_all test_greyscale_dilate)
ADD_TEST( vil_algo_test_greyscale_erode     ${EXECUTABLE_OUTPUT_PATH}/vil_algo_test_all test_greyscale_erode)
ADD_TEST( vil_algo_test_median              ${EXECUTABLE_OUTPUT_PATH}/vil_algo_test_all test_median)
ADD_TEST( vil_algo_test_suppress_non_max    ${EXECUTABLE_OUTPUT_PATH}/vil_algo_test_all test_suppress_non_max )
ADD_TEST( vil_algo_test_algo_sobel          ${EXECUTABLE_OUTPUT_PATH}/vil_algo_test_all test_algo_sobel)
ADD_TEST( vil_algo_abs_shuffle_distance     ${EXECUTABLE_OUTPUT_PATH}/vil_algo_test_all test_algo_abs_shuffle_distance)
ADD_TEST( vil_algo_suppress_non_max_edges   ${EXECUTABLE_OUTPUT_PATH}/vil_algo_test_all test_algo_suppress_non_max_edges)
ADD_TEST( vil_algo_test_checker_board       ${EXECUTABLE_OUTPUT_PATH}/vil_algo_test_all test_algo_checker_board)
ADD_TEST( vil_algo_test_quad_distance_function       ${EXECUTABLE_OUTPUT_PATH}/vil_algo_test_all test_algo_quad_distance_function)

#ADD_EXECUTABLE( vil_mil_gauss_general_timings vil_mil_gauss_general_timings.cxx)
#TARGET_LINK_LIBRARIES( vil_mil_gauss_general_timings vil_algo mil vpl vul mbl testlib vcl )
#ADD_TEST( vil_mil_gauss_general_timings ${EXECUTABLE_OUTPUT_PATH}/vil_mil_gauss_general_timings )

ADD_EXECUTABLE( vil_algo_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( vil_algo_test_include vil_algo )
