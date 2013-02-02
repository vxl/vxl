#include <testlib/testlib_register.h>

DECLARE( test_volm_wr3db_index );
DECLARE( test_volm_locations );
DECLARE( test_volm_matcher_p1 );

void register_tests()
{
  REGISTER( test_volm_locations );
  REGISTER( test_volm_wr3db_index );
  REGISTER( test_volm_matcher_p1 );
}


DEFINE_MAIN;
