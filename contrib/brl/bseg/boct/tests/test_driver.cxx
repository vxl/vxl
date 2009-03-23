#include <testlib/testlib_register.h>

DECLARE( test_create_tree );
DECLARE( test_locate_point );
DECLARE( test_locate_region );


void register_tests()
{
  REGISTER( test_create_tree );
  REGISTER( test_locate_point );
  REGISTER( test_locate_region );

}


DEFINE_MAIN;
