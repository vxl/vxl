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
  test_algo_fft.cxx
  test_algo_histogram.cxx
  test_algo_histogram_equalise.cxx
  test_algo_distance_transform.cxx
  test_algo_blob.cxx
  test_algo_find_peaks.cxx
  test_algo_find_plateaus.cxx
  test_algo_region_finder.cxx
  test_algo_cartesian_differential_invariants.cxx
  test_algo_corners.cxx
  test_binary_erode.cxx
  test_binary_dilate.cxx
  test_greyscale_dilate.cxx
  test_greyscale_erode.cxx
  test_median.cxx
  test_suppress_non_max.cxx
  test_algo_suppress_non_plateau.cxx
  test_algo_sobel.cxx
  test_algo_abs_shuffle_distance.cxx
  test_algo_suppress_non_max_edges.cxx
  test_algo_checker_board.cxx
  test_algo_quad_distance_function.cxx
  test_algo_flood_fill.cxx
)

IF(CMAKE_COMPILER_IS_GNUCXX)
  SET_SOURCE_FILES_PROPERTIES(test_algo_convolve_1d.cxx PROPERTIES COMPILE_FLAGS -O0)
ENDIF(CMAKE_COMPILER_IS_GNUCXX)

TARGET_LINK_LIBRARIES( vil_algo_test_all vil_algo testlib vcl )


# vil/algo

ADD_TEST( vil_algo_test_gauss_filter        ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/vil_algo_test_all test_algo_gauss_filter)
ADD_TEST( vil_algo_test_gauss_reduce        ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/vil_algo_test_all test_algo_gauss_reduce)
ADD_TEST( vil_algo_test_colour_space        ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/vil_algo_test_all test_algo_colour_space)
ADD_TEST( vil_algo_test_convolve_1d         ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/vil_algo_test_all test_algo_convolve_1d)
ADD_TEST( vil_algo_test_convolve_2d         ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/vil_algo_test_all test_algo_convolve_2d)
ADD_TEST( vil_algo_test_correlate_1d        ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/vil_algo_test_all test_algo_correlate_1d)
ADD_TEST( vil_algo_test_correlate_2d        ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/vil_algo_test_all test_algo_correlate_2d)
ADD_TEST( vil_algo_test_exp_filter_1d       ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/vil_algo_test_all test_algo_exp_filter_1d)
ADD_TEST( vil_algo_test_exp_grad_filter_1d  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/vil_algo_test_all test_algo_exp_grad_filter_1d)
ADD_TEST( vil_algo_test_line_filter         ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/vil_algo_test_all test_algo_line_filter)
ADD_TEST( vil_algo_test_threshold           ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/vil_algo_test_all test_algo_threshold)
ADD_TEST( vil_algo_test_grid_merge          ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/vil_algo_test_all test_algo_grid_merge )
ADD_TEST( vil_algo_test_find_4con_boundary  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/vil_algo_test_all test_algo_find_4con_boundary )
ADD_TEST( vil_algo_test_fft                 ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/vil_algo_test_all test_algo_fft )
ADD_TEST( vil_algo_test_histogram           ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/vil_algo_test_all test_algo_histogram )
ADD_TEST( vil_algo_test_histogram_equalise  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/vil_algo_test_all test_algo_histogram_equalise )
ADD_TEST( vil_algo_test_distance_transform  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/vil_algo_test_all test_algo_distance_transform )
ADD_TEST( vil_algo_test_blob                ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/vil_algo_test_all test_algo_blob )
ADD_TEST( vil_algo_test_find_peaks          ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/vil_algo_test_all test_algo_find_peaks )
ADD_TEST( vil_algo_test_find_plateaus       ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/vil_algo_test_all test_algo_find_plateaus )
ADD_TEST( vil_algo_test_region_finder       ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/vil_algo_test_all test_algo_region_finder )
ADD_TEST( vil_algo_test_corners             ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/vil_algo_test_all test_algo_corners )
ADD_TEST( vil_algo_test_cartesian_differential_invariants ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/vil_algo_test_all test_algo_cartesian_differential_invariants )
ADD_TEST( vil_algo_test_binary_erode        ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/vil_algo_test_all test_binary_erode)
ADD_TEST( vil_algo_test_binary_dilate       ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/vil_algo_test_all test_binary_dilate)
ADD_TEST( vil_algo_test_greyscale_dilate    ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/vil_algo_test_all test_greyscale_dilate)
ADD_TEST( vil_algo_test_greyscale_erode     ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/vil_algo_test_all test_greyscale_erode)
ADD_TEST( vil_algo_test_median              ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/vil_algo_test_all test_median)
ADD_TEST( vil_algo_test_suppress_non_max    ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/vil_algo_test_all test_suppress_non_max )
ADD_TEST( vil_algo_test_suppress_non_plateau ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/vil_algo_test_all test_algo_suppress_non_plateau )
ADD_TEST( vil_algo_test_algo_sobel          ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/vil_algo_test_all test_algo_sobel)
ADD_TEST( vil_algo_test_abs_shuffle_distance     ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/vil_algo_test_all test_algo_abs_shuffle_distance)
ADD_TEST( vil_algo_test_suppress_non_max_edges   ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/vil_algo_test_all test_algo_suppress_non_max_edges)
ADD_TEST( vil_algo_test_checker_board       ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/vil_algo_test_all test_algo_checker_board)
ADD_TEST( vil_algo_test_quad_distance_function   ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/vil_algo_test_all test_algo_quad_distance_function)
ADD_TEST( vil_algo_test_flood_fill          ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/vil_algo_test_all test_algo_flood_fill)

ADD_EXECUTABLE( vil_algo_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( vil_algo_test_include vil_algo )
ADD_EXECUTABLE( vil_algo_test_template_include test_template_include.cxx )
TARGET_LINK_LIBRARIES( vil_algo_test_template_include vil_algo )
