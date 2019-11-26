#include "testlib/testlib_register.h"

DECLARE( test_algo_gauss_reduce );
DECLARE( test_algo_colour_space );
DECLARE( test_algo_correlate_1d );
DECLARE( test_algo_convolve_1d );
DECLARE( test_algo_correlate_2d );
DECLARE( test_algo_convolve_2d );
DECLARE( test_algo_exp_filter_1d );
DECLARE( test_algo_gauss_filter );
DECLARE( test_algo_exp_grad_filter_1d );
DECLARE( test_algo_line_filter );
DECLARE( test_algo_threshold );
DECLARE( test_algo_grid_merge );
DECLARE( test_algo_find_4con_boundary );
DECLARE( test_algo_fft );
DECLARE( test_algo_histogram );
DECLARE( test_algo_histogram_equalise );
DECLARE( test_algo_distance_transform );
DECLARE( test_algo_blob );
DECLARE( test_algo_find_peaks );
DECLARE( test_algo_find_plateaus );
DECLARE( test_algo_region_finder );
DECLARE( test_algo_cartesian_differential_invariants );
DECLARE( test_algo_corners );
DECLARE( test_binary_dilate );
DECLARE( test_binary_erode );
DECLARE( test_greyscale_dilate );
DECLARE( test_greyscale_erode );
DECLARE( test_median );
DECLARE( test_suppress_non_max );
DECLARE( test_algo_suppress_non_plateau );
DECLARE( test_algo_sobel );
DECLARE( test_algo_2nd_deriv_3x3 );
DECLARE( test_algo_abs_shuffle_distance );
DECLARE( test_algo_suppress_non_max_edges );
DECLARE( test_algo_checker_board );
DECLARE( test_algo_quad_distance_function );
DECLARE( test_algo_flood_fill );

void
register_tests()
{
  REGISTER( test_algo_gauss_reduce );
  REGISTER( test_algo_colour_space );
  REGISTER( test_algo_correlate_1d );
  REGISTER( test_algo_convolve_1d );
  REGISTER( test_algo_correlate_2d );
  REGISTER( test_algo_convolve_2d );
  REGISTER( test_algo_exp_filter_1d );
  REGISTER( test_algo_gauss_filter );
  REGISTER( test_algo_exp_grad_filter_1d );
  REGISTER( test_algo_line_filter );
  REGISTER( test_algo_threshold );
  REGISTER( test_algo_grid_merge );
  REGISTER( test_algo_find_4con_boundary );
  REGISTER( test_algo_fft );
  REGISTER( test_algo_histogram );
  REGISTER( test_algo_histogram_equalise );
  REGISTER( test_algo_distance_transform );
  REGISTER( test_algo_blob );
  REGISTER( test_algo_find_peaks );
  REGISTER( test_algo_find_plateaus );
  REGISTER( test_algo_region_finder );
  REGISTER( test_algo_cartesian_differential_invariants );
  REGISTER( test_algo_corners );
  REGISTER( test_binary_dilate );
  REGISTER( test_binary_erode );
  REGISTER( test_greyscale_dilate );
  REGISTER( test_greyscale_erode );
  REGISTER( test_median );
  REGISTER( test_suppress_non_max );
  REGISTER( test_algo_suppress_non_plateau );
  REGISTER( test_algo_sobel );
  REGISTER( test_algo_2nd_deriv_3x3 );
  REGISTER( test_algo_abs_shuffle_distance );
  REGISTER( test_algo_suppress_non_max_edges );
  REGISTER( test_algo_checker_board );
  REGISTER( test_algo_quad_distance_function );
  REGISTER( test_algo_flood_fill );
}

DEFINE_MAIN;
