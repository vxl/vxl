#include <testlib/testlib_register.h>

DECLARE( test_include );
DECLARE( test_user_info_io );

void
register_tests()
{
  REGISTER( test_include );
  REGISTER( test_user_info_io );
}

DEFINE_MAIN;
