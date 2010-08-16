#include <testlib/testlib_register.h>

DECLARE( test_binary_io );
DECLARE( test_block_iter );
DECLARE( test_region_finder );
DECLARE( test_cell_iterator );

void register_tests()
{
  REGISTER( test_binary_io );
  REGISTER( test_block_iter );
  REGISTER( test_region_finder );
  REGISTER( test_cell_iterator );
}


DEFINE_MAIN;
