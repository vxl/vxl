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

ADD_TEST( NAME vil_algo_test_gauss_filter COMMAND $<TARGET_FILE:vil_algo_test_all> test_algo_gauss_filter)
ADD_TEST( NAME vil_algo_test_gauss_reduce COMMAND $<TARGET_FILE:vil_algo_test_all> test_algo_gauss_reduce)
ADD_TEST( NAME vil_algo_test_colour_space COMMAND $<TARGET_FILE:vil_algo_test_all> test_algo_colour_space)
ADD_TEST( NAME vil_algo_test_convolve_1d COMMAND $<TARGET_FILE:vil_algo_test_all> test_algo_convolve_1d)
ADD_TEST( NAME vil_algo_test_convolve_2d COMMAND $<TARGET_FILE:vil_algo_test_all> test_algo_convolve_2d)
ADD_TEST( NAME vil_algo_test_correlate_1d COMMAND $<TARGET_FILE:vil_algo_test_all> test_algo_correlate_1d)
ADD_TEST( NAME vil_algo_test_correlate_2d COMMAND $<TARGET_FILE:vil_algo_test_all> test_algo_correlate_2d)
ADD_TEST( NAME vil_algo_test_exp_filter_1d COMMAND $<TARGET_FILE:vil_algo_test_all> test_algo_exp_filter_1d)
ADD_TEST( NAME vil_algo_test_exp_grad_filter_1d COMMAND $<TARGET_FILE:vil_algo_test_all> test_algo_exp_grad_filter_1d)
ADD_TEST( NAME vil_algo_test_line_filter COMMAND $<TARGET_FILE:vil_algo_test_all> test_algo_line_filter)
ADD_TEST( NAME vil_algo_test_threshold COMMAND $<TARGET_FILE:vil_algo_test_all> test_algo_threshold)
ADD_TEST( NAME vil_algo_test_grid_merge COMMAND $<TARGET_FILE:vil_algo_test_all> test_algo_grid_merge )
ADD_TEST( NAME vil_algo_test_find_4con_boundary COMMAND $<TARGET_FILE:vil_algo_test_all> test_algo_find_4con_boundary )
ADD_TEST( NAME vil_algo_test_fft COMMAND $<TARGET_FILE:vil_algo_test_all> test_algo_fft )
ADD_TEST( NAME vil_algo_test_histogram COMMAND $<TARGET_FILE:vil_algo_test_all> test_algo_histogram )
ADD_TEST( NAME vil_algo_test_histogram_equalise COMMAND $<TARGET_FILE:vil_algo_test_all> test_algo_histogram_equalise )
ADD_TEST( NAME vil_algo_test_distance_transform COMMAND $<TARGET_FILE:vil_algo_test_all> test_algo_distance_transform )
ADD_TEST( NAME vil_algo_test_blob COMMAND $<TARGET_FILE:vil_algo_test_all> test_algo_blob )
ADD_TEST( NAME vil_algo_test_find_peaks COMMAND $<TARGET_FILE:vil_algo_test_all> test_algo_find_peaks )
ADD_TEST( NAME vil_algo_test_find_plateaus COMMAND $<TARGET_FILE:vil_algo_test_all> test_algo_find_plateaus )
ADD_TEST( NAME vil_algo_test_region_finder COMMAND $<TARGET_FILE:vil_algo_test_all> test_algo_region_finder )
ADD_TEST( NAME vil_algo_test_corners COMMAND $<TARGET_FILE:vil_algo_test_all> test_algo_corners )
ADD_TEST( NAME vil_algo_test_cartesian_differential_invariants COMMAND $<TARGET_FILE:vil_algo_test_all> test_algo_cartesian_differential_invariants )
ADD_TEST( NAME vil_algo_test_binary_erode COMMAND $<TARGET_FILE:vil_algo_test_all> test_binary_erode)
ADD_TEST( NAME vil_algo_test_binary_dilate COMMAND $<TARGET_FILE:vil_algo_test_all> test_binary_dilate)
ADD_TEST( NAME vil_algo_test_greyscale_dilate COMMAND $<TARGET_FILE:vil_algo_test_all> test_greyscale_dilate)
ADD_TEST( NAME vil_algo_test_greyscale_erode COMMAND $<TARGET_FILE:vil_algo_test_all> test_greyscale_erode)
ADD_TEST( NAME vil_algo_test_median COMMAND $<TARGET_FILE:vil_algo_test_all> test_median)
ADD_TEST( NAME vil_algo_test_suppress_non_max COMMAND $<TARGET_FILE:vil_algo_test_all> test_suppress_non_max )
ADD_TEST( NAME vil_algo_test_suppress_non_plateau COMMAND $<TARGET_FILE:vil_algo_test_all> test_algo_suppress_non_plateau )
ADD_TEST( NAME vil_algo_test_algo_sobel COMMAND $<TARGET_FILE:vil_algo_test_all> test_algo_sobel)
ADD_TEST( NAME vil_algo_test_abs_shuffle_distance COMMAND $<TARGET_FILE:vil_algo_test_all> test_algo_abs_shuffle_distance)
ADD_TEST( NAME vil_algo_test_suppress_non_max_edges COMMAND $<TARGET_FILE:vil_algo_test_all> test_algo_suppress_non_max_edges)
ADD_TEST( NAME vil_algo_test_checker_board COMMAND $<TARGET_FILE:vil_algo_test_all> test_algo_checker_board)
ADD_TEST( NAME vil_algo_test_quad_distance_function COMMAND $<TARGET_FILE:vil_algo_test_all> test_algo_quad_distance_function)
ADD_TEST( NAME vil_algo_test_flood_fill COMMAND $<TARGET_FILE:vil_algo_test_all> test_algo_flood_fill)

ADD_EXECUTABLE( vil_algo_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( vil_algo_test_include vil_algo )
ADD_EXECUTABLE( vil_algo_test_template_include test_template_include.cxx )
TARGET_LINK_LIBRARIES( vil_algo_test_template_include vil_algo )
