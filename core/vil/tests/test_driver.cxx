#include <testlib/testlib_register.h>

DECLARE( test_vil1 );
DECLARE( test_convert );
DECLARE( test_file_format_read );
DECLARE( test_pixel_format );
DECLARE( test_save_load_image );
DECLARE( test_image_view );
DECLARE( test_image_resource );
DECLARE( test_bilin_interp );
DECLARE( test_sample_profile_bilin );
DECLARE( test_sample_grid_bilin );
DECLARE( test_algo_gauss_reduce );
DECLARE( test_algo_convolve_1d );
DECLARE( test_algo_convolve_2d );
DECLARE( test_algo_exp_filter_1d );
DECLARE( test_image_view_maths );
DECLARE( test_memory_chunk );
DECLARE( test_resample_bilin );
DECLARE( test_binary_dilate );
DECLARE( test_binary_erode );
DECLARE( test_greyscale_dilate );
DECLARE( test_greyscale_erode );
DECLARE( test_median );

void
register_tests()
{
  REGISTER( test_vil1 );
  REGISTER( test_convert );
  REGISTER( test_pixel_format );
  REGISTER( test_save_load_image );
  REGISTER( test_file_format_read );
  REGISTER( test_image_view );
  REGISTER( test_image_resource );
  REGISTER( test_bilin_interp );
  REGISTER( test_sample_profile_bilin );
  REGISTER( test_sample_grid_bilin );
  REGISTER( test_algo_gauss_reduce );
  REGISTER( test_algo_convolve_1d );
  REGISTER( test_algo_convolve_2d );
  REGISTER( test_algo_exp_filter_1d );
  REGISTER( test_image_view_maths );
  REGISTER( test_memory_chunk );
  REGISTER( test_resample_bilin );
  REGISTER( test_binary_dilate );
  REGISTER( test_binary_erode );
  REGISTER( test_greyscale_dilate );
  REGISTER( test_greyscale_erode );
  REGISTER( test_median );
}

DEFINE_MAIN;
