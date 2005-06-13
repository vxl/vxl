#include <testlib/testlib_register.h>

DECLARE( test_bsta_histogram );
DECLARE( test_k_medoid );

void
register_tests()
{
  REGISTER ( test_bsta_histogram );
  REGISTER ( test_k_medoid );
}

DEFINE_MAIN;
