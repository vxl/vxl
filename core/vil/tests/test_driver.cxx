#include <testlib/testlib_register.h>

DECLARE( test_file_format_read );
DECLARE( test_pixel_format );
DECLARE( test_save_load_image );
DECLARE( test_image_view );
DECLARE( test_bilin_interp );
DECLARE( test_sample_profile_bilin );
DECLARE( test_sample_grid_bilin );
DECLARE( test_algo_gauss_reduce );
DECLARE( test_algo_convolve_1d );
DECLARE( test_algo_exp_filter_1d );

void
register_tests()
{
  REGISTER( test_pixel_format );
  REGISTER( test_save_load_image );
  REGISTER( test_file_format_read );
  REGISTER( test_image_view );
  REGISTER( test_bilin_interp );
  REGISTER( test_sample_profile_bilin );
  REGISTER( test_sample_grid_bilin );
  REGISTER( test_algo_gauss_reduce );
  REGISTER( test_algo_convolve_1d );
  REGISTER( test_algo_exp_filter_1d );
}

DEFINE_MAIN;
