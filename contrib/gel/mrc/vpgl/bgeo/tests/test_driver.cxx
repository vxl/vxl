#include <testlib/testlib_register.h>

DECLARE( test_lvcs );
DECLARE( test_sun_pos );

void
register_tests()
{
  REGISTER ( test_lvcs );
  REGISTER ( test_sun_pos );
}

DEFINE_MAIN;
