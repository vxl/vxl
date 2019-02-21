#include <testlib/testlib_register.h>

DECLARE( test_f_utils );
DECLARE( test_metadata );
DECLARE( test_match_utils );
DECLARE( test_match_tree );

void register_tests()
{
  REGISTER( test_f_utils );
  REGISTER( test_metadata );
  REGISTER( test_match_utils );
  REGISTER( test_match_tree );
}
DEFINE_MAIN;
