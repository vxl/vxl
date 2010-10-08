#include <testlib/testlib_register.h>

DECLARE( test_icam_transform );
DECLARE( test_minimizer );

void
register_tests()
{
  REGISTER( test_minimizer );
  REGISTER( test_icam_transform );
}

DEFINE_MAIN;


