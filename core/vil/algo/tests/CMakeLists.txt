# This is core/vil/algo/tests/CMakeLists.txt

add_executable( vil_algo_test_all
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

if(CMAKE_COMPILER_IS_GNUCXX)
  set_source_files_properties(test_algo_convolve_1d.cxx PROPERTIES COMPILE_FLAGS -O0)
endif()

target_link_libraries( vil_algo_test_all ${VXL_LIB_PREFIX}vil_algo ${VXL_LIB_PREFIX}testlib ${VXL_LIB_PREFIX}vcl )


# vil/algo

add_test( NAME vil_algo_test_gauss_filter COMMAND $<TARGET_FILE:vil_algo_test_all> test_algo_gauss_filter)
add_test( NAME vil_algo_test_gauss_reduce COMMAND $<TARGET_FILE:vil_algo_test_all> test_algo_gauss_reduce)
add_test( NAME vil_algo_test_colour_space COMMAND $<TARGET_FILE:vil_algo_test_all> test_algo_colour_space)
add_test( NAME vil_algo_test_convolve_1d COMMAND $<TARGET_FILE:vil_algo_test_all> test_algo_convolve_1d)
add_test( NAME vil_algo_test_convolve_2d COMMAND $<TARGET_FILE:vil_algo_test_all> test_algo_convolve_2d)
add_test( NAME vil_algo_test_correlate_1d COMMAND $<TARGET_FILE:vil_algo_test_all> test_algo_correlate_1d)
add_test( NAME vil_algo_test_correlate_2d COMMAND $<TARGET_FILE:vil_algo_test_all> test_algo_correlate_2d)
add_test( NAME vil_algo_test_exp_filter_1d COMMAND $<TARGET_FILE:vil_algo_test_all> test_algo_exp_filter_1d)
add_test( NAME vil_algo_test_exp_grad_filter_1d COMMAND $<TARGET_FILE:vil_algo_test_all> test_algo_exp_grad_filter_1d)
add_test( NAME vil_algo_test_line_filter COMMAND $<TARGET_FILE:vil_algo_test_all> test_algo_line_filter)
add_test( NAME vil_algo_test_threshold COMMAND $<TARGET_FILE:vil_algo_test_all> test_algo_threshold)
add_test( NAME vil_algo_test_grid_merge COMMAND $<TARGET_FILE:vil_algo_test_all> test_algo_grid_merge )
add_test( NAME vil_algo_test_find_4con_boundary COMMAND $<TARGET_FILE:vil_algo_test_all> test_algo_find_4con_boundary )
add_test( NAME vil_algo_test_fft COMMAND $<TARGET_FILE:vil_algo_test_all> test_algo_fft )
add_test( NAME vil_algo_test_histogram COMMAND $<TARGET_FILE:vil_algo_test_all> test_algo_histogram )
add_test( NAME vil_algo_test_histogram_equalise COMMAND $<TARGET_FILE:vil_algo_test_all> test_algo_histogram_equalise )
add_test( NAME vil_algo_test_distance_transform COMMAND $<TARGET_FILE:vil_algo_test_all> test_algo_distance_transform )
add_test( NAME vil_algo_test_blob COMMAND $<TARGET_FILE:vil_algo_test_all> test_algo_blob )
add_test( NAME vil_algo_test_find_peaks COMMAND $<TARGET_FILE:vil_algo_test_all> test_algo_find_peaks )
add_test( NAME vil_algo_test_find_plateaus COMMAND $<TARGET_FILE:vil_algo_test_all> test_algo_find_plateaus )
add_test( NAME vil_algo_test_region_finder COMMAND $<TARGET_FILE:vil_algo_test_all> test_algo_region_finder )
add_test( NAME vil_algo_test_corners COMMAND $<TARGET_FILE:vil_algo_test_all> test_algo_corners )
add_test( NAME vil_algo_test_cartesian_differential_invariants COMMAND $<TARGET_FILE:vil_algo_test_all> test_algo_cartesian_differential_invariants )
add_test( NAME vil_algo_test_binary_erode COMMAND $<TARGET_FILE:vil_algo_test_all> test_binary_erode)
add_test( NAME vil_algo_test_binary_dilate COMMAND $<TARGET_FILE:vil_algo_test_all> test_binary_dilate)
add_test( NAME vil_algo_test_greyscale_dilate COMMAND $<TARGET_FILE:vil_algo_test_all> test_greyscale_dilate)
add_test( NAME vil_algo_test_greyscale_erode COMMAND $<TARGET_FILE:vil_algo_test_all> test_greyscale_erode)
add_test( NAME vil_algo_test_median COMMAND $<TARGET_FILE:vil_algo_test_all> test_median)
add_test( NAME vil_algo_test_suppress_non_max COMMAND $<TARGET_FILE:vil_algo_test_all> test_suppress_non_max )
add_test( NAME vil_algo_test_suppress_non_plateau COMMAND $<TARGET_FILE:vil_algo_test_all> test_algo_suppress_non_plateau )
add_test( NAME vil_algo_test_algo_sobel COMMAND $<TARGET_FILE:vil_algo_test_all> test_algo_sobel)
add_test( NAME vil_algo_test_abs_shuffle_distance COMMAND $<TARGET_FILE:vil_algo_test_all> test_algo_abs_shuffle_distance)
add_test( NAME vil_algo_test_suppress_non_max_edges COMMAND $<TARGET_FILE:vil_algo_test_all> test_algo_suppress_non_max_edges)
add_test( NAME vil_algo_test_checker_board COMMAND $<TARGET_FILE:vil_algo_test_all> test_algo_checker_board)
add_test( NAME vil_algo_test_quad_distance_function COMMAND $<TARGET_FILE:vil_algo_test_all> test_algo_quad_distance_function)
add_test( NAME vil_algo_test_flood_fill COMMAND $<TARGET_FILE:vil_algo_test_all> test_algo_flood_fill)

add_executable( vil_algo_test_include test_include.cxx )
target_link_libraries( vil_algo_test_include ${VXL_LIB_PREFIX}vil_algo )
add_executable( vil_algo_test_template_include test_template_include.cxx )
target_link_libraries( vil_algo_test_template_include ${VXL_LIB_PREFIX}vil_algo )
