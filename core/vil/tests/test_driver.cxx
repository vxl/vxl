#include <testlib/testlib_register.h>

DECLARE( test_file_format_read );
DECLARE( test_save_load_image );
DECLARE( test_image_view );
DECLARE( test_bilin_interp );
DECLARE( test_sample_profile_bilin );
DECLARE( test_algo_gauss_reduce );

void
register_tests()
{
  REGISTER( test_save_load_image );
  REGISTER( test_file_format_read );
  REGISTER( test_image_view );
  REGISTER( test_bilin_interp );
  REGISTER( test_sample_profile_bilin );
  REGISTER( test_algo_gauss_reduce );
}

DEFINE_MAIN;
