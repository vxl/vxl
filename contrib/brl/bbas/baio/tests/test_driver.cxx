#include <testlib/testlib_register.h>
DECLARE( test_read );
DECLARE( test_write );

void
register_tests()
{
  REGISTER( test_read );
  REGISTER( test_write );
}

DEFINE_MAIN;
