#include <testlib/testlib_register.h>

DECLARE( test_image_view );
DECLARE( test_trilin_interp );
DECLARE( test_sample_profile_trilin );
DECLARE( test_gauss_reduce );
DECLARE( test_algo_threshold );
DECLARE( test_algo_structuring_element );
DECLARE( test_algo_binary_dilate );
DECLARE( test_algo_binary_erode );

void
register_tests()
{
  REGISTER( test_image_view );
  REGISTER( test_trilin_interp );
  REGISTER( test_sample_profile_trilin );
  REGISTER( test_gauss_reduce );
  REGISTER( test_algo_threshold );
  REGISTER( test_algo_structuring_element );
  REGISTER( test_algo_binary_dilate );
  REGISTER( test_algo_binary_erode );
}

DEFINE_MAIN;
