#include <testlib/testlib_register.h>

DECLARE( test_create_tree );
DECLARE( test_locate_point );
DECLARE( test_locate_region );
DECLARE( test_find_neighbors );
DECLARE( test_binary_io );
DECLARE( test_clone_tree );

void register_tests()
{
  REGISTER( test_create_tree );
  REGISTER( test_locate_point );
  REGISTER( test_locate_region );
  REGISTER( test_find_neighbors );
  REGISTER( test_binary_io );
  REGISTER( test_clone_tree );
}


DEFINE_MAIN;
