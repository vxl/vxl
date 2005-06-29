#include <testlib/testlib_register.h>

DECLARE( test_bsta_histogram );
DECLARE( test_k_medoid );
DECLARE( test_otsu_threshold );

void
register_tests()
{
  REGISTER ( test_bsta_histogram );
  REGISTER ( test_k_medoid );
  REGISTER ( test_otsu_threshold );
}

DEFINE_MAIN;
