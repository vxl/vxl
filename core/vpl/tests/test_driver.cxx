#include <testlib/testlib_register.h>

DECLARE( test_include );
DECLARE( test_unistd );

void
register_tests()
{
  REGISTER( test_include );
  REGISTER( test_unistd );
}

DEFINE_MAIN;
