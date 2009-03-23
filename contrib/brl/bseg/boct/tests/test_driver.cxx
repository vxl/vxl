#include <testlib/testlib_register.h>

DECLARE( test_create_tree );
DECLARE( test_locate_point );


void register_tests()
{
  REGISTER( test_create_tree );
  REGISTER( test_locate_point );
}

DEFINE_MAIN;
