#include <testlib/testlib_register.h>

DECLARE( test_2d_geo_index );
DECLARE( test_biarc );
DECLARE( test_eulerspiral );
DECLARE( test_register );

void
register_tests()
{
  REGISTER( test_2d_geo_index );
  REGISTER( test_biarc );
  REGISTER( test_eulerspiral );
  REGISTER( test_register );
}

DEFINE_MAIN;
