#include <testlib/testlib_register.h>


DECLARE( test_bins_2d );
DECLARE( test_borgefors );
DECLARE( test_bounding_box );
DECLARE( test_dist );
DECLARE( test_kd_tree );
DECLARE( test_point );
DECLARE( test_bins );

void
register_tests()
{
  REGISTER( test_bins_2d );
  REGISTER( test_borgefors );
  REGISTER( test_bounding_box );
  REGISTER( test_dist );
  REGISTER( test_kd_tree );
  REGISTER( test_point );
  REGISTER( test_bins );
}

DEFINE_MAIN;
