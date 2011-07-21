#include <testlib/testlib_register.h>

DECLARE( test_illum );
DECLARE( test_sun_pos );
DECLARE( test_sun_hist );
DECLARE( test_sun_dir_index );
DECLARE( test_phongs_model_est );


void register_tests()
{

  REGISTER( test_illum );
  REGISTER( test_sun_pos );
  REGISTER( test_sun_hist );
  REGISTER( test_sun_dir_index );
  REGISTER( test_phongs_model_est );
}

DEFINE_MAIN;
