#include <testlib/testlib_register.h>

DECLARE( test_volm_wr3db_index );
DECLARE( test_volm_locations );
DECLARE( test_volm_obj_based_matcher );

void register_tests()
{
  REGISTER( test_volm_locations );
  REGISTER( test_volm_wr3db_index );
  REGISTER( test_volm_obj_based_matcher );
}


DEFINE_MAIN;
