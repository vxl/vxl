#include <testlib/testlib_register.h>

DECLARE( test_config );
DECLARE( test_build_info );

void
register_tests()
{
  REGISTER( test_config );
  REGISTER( test_build_info );
}

DEFINE_MAIN;
