#include <testlib/testlib_register.h>

DECLARE( test_container );
DECLARE( test_COOL_container );

void
register_tests()
{
  REGISTER( test_container );
  REGISTER( test_COOL_container );
}

DEFINE_MAIN;
