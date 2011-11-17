#include <testlib/testlib_register.h>

DECLARE( test_icam_transform );
DECLARE( test_minimizer );
DECLARE( test_rotation_only );

void
register_tests()
{
  REGISTER( test_minimizer );
  REGISTER( test_icam_transform );
  REGISTER( test_rotation_only );
}

DEFINE_MAIN;


