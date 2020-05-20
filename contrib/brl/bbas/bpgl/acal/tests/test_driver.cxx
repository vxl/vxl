#include "testlib/testlib_register.h"

DECLARE( test_f_utils );
DECLARE( test_match_graph );
DECLARE( test_match_tree );
DECLARE( test_match_utils );
DECLARE( test_metadata );

void
register_tests()
{
  REGISTER( test_f_utils );
  REGISTER( test_match_graph );
  REGISTER( test_match_tree );
  REGISTER( test_match_utils );
  REGISTER( test_metadata );
}

DEFINE_MAIN;
