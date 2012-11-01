#include <testlib/testlib_register.h>

DECLARE( test_volm_wr3db_index );
DECLARE( test_volm_locations );

void register_tests()
{
  REGISTER( test_volm_locations );
  REGISTER( test_volm_wr3db_index );
}


DEFINE_MAIN;
