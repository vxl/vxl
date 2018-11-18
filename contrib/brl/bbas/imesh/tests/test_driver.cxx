#include <testlib/testlib_register.h>


DECLARE( test_intersect );
DECLARE( test_detect );
DECLARE( test_kd_tree );
DECLARE( test_imls_surface );

void
register_tests()
{
  REGISTER( test_intersect );
  REGISTER( test_detect );
  REGISTER( test_kd_tree );
  REGISTER( test_imls_surface );
}

DEFINE_MAIN;
