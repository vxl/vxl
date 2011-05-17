#include <testlib/testlib_register.h>

DECLARE( test_illum );
DECLARE( test_sun_pos );
DECLARE( test_sun_hist );
DECLARE( test_sun_dir_index );


void register_tests()
{

  REGISTER( test_illum );
  REGISTER( test_sun_pos );
  REGISTER( test_sun_hist );
  REGISTER( test_sun_dir_index );
}

DEFINE_MAIN;
