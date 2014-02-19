#include <testlib/testlib_register.h>

DECLARE( test_volm_wr3db_ind );
DECLARE( test_volm_locations );
DECLARE( test_volm_matcher_p1 );
DECLARE( test_volm_matcher_p0 );

void register_tests()
{
  REGISTER( test_volm_locations );
  REGISTER( test_volm_wr3db_ind );
  REGISTER( test_volm_matcher_p1 );
  REGISTER( test_volm_matcher_p0 );
}


DEFINE_MAIN;
