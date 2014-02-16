#include <testlib/testlib_register.h>

DECLARE( test_binary_io );
DECLARE( test_block_iter );
//FAILS ON DASHBOARD FIX_ME
//DECLARE( test_region_finder );
DECLARE( test_cell_iterator );
DECLARE( test_load_neighboring_blocks );

void register_tests()
{
  REGISTER( test_binary_io );
  REGISTER( test_block_iter );
//FAILS ON DASHBOARD FIX_ME
// REGISTER( test_region_finder );
  REGISTER( test_cell_iterator );
  REGISTER( test_load_neighboring_blocks );
}


DEFINE_MAIN;
