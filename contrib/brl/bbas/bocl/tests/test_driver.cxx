#include <testlib/testlib_register.h>

DECLARE( test_global_io_bandwidth );
DECLARE( test_command_queue );

void
register_tests()
{
  REGISTER( test_global_io_bandwidth );
  REGISTER( test_command_queue );

}

DEFINE_MAIN;
