#include <testlib/testlib_register.h>

DECLARE( test_global_io_bandwidth );
DECLARE( test_command_queue );
DECLARE( test_kernel );
DECLARE( test_local_mem_access );
DECLARE( test_mem );

void register_tests()
{
  REGISTER( test_global_io_bandwidth );
  REGISTER( test_command_queue );
  REGISTER( test_kernel );
  REGISTER( test_local_mem_access );
  REGISTER( test_mem );
}

DEFINE_MAIN;
