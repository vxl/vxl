#include <testlib/testlib_register.h>

DECLARE( test_appearance );
DECLARE( test_dem_appear );
void
register_tests()
{
  REGISTER( test_appearance );
  REGISTER( test_dem_appear );
}

DEFINE_MAIN;
