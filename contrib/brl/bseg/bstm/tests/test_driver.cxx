#include <testlib/testlib_register.h>

DECLARE( test_block );
DECLARE( test_time_tree );

void register_tests()
{
  REGISTER( test_block );
  REGISTER( test_time_tree );
}


DEFINE_MAIN;
