#include <testlib/testlib_register.h>

DECLARE( test_image_view );
DECLARE( test_trilin_interp );
DECLARE( test_sample_profile_trilin );
DECLARE( test_gauss_reduce );
DECLARE( test_algo_threshold );

void
register_tests()
{
  REGISTER( test_image_view );
  REGISTER( test_trilin_interp );
  REGISTER( test_sample_profile_trilin );
  REGISTER( test_gauss_reduce );
  REGISTER( test_algo_threshold );
}

DEFINE_MAIN;
