#include <testlib/testlib_register.h>

DECLARE( test_2d_geo_index );
DECLARE( test_biarc );
DECLARE( test_eulerspiral );

void
register_tests()
{
  REGISTER( test_2d_geo_index );
  REGISTER( test_biarc );
  REGISTER( test_eulerspiral );
}

DEFINE_MAIN;
