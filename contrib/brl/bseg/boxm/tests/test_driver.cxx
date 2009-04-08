#include <testlib/testlib_register.h>

DECLARE( test_binary_io );
DECLARE( test_block_iter );

void register_tests()
{
  REGISTER( test_binary_io );
  REGISTER( test_block_iter );
}


DEFINE_MAIN;
