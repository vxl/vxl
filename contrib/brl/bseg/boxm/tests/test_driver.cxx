#include <testlib/testlib_register.h>

DECLARE( test_binary_io );
DECLARE( test_block_iter );
DECLARE( test_block_vis_graph );
DECLARE( test_cell_vis_graph );

void register_tests()
{
  REGISTER( test_binary_io );
  REGISTER( test_block_iter );
  REGISTER( test_block_vis_graph );
    REGISTER( test_cell_vis_graph );

}


DEFINE_MAIN;
