#include <testlib/testlib_register.h>

DECLARE( test_icam_transform );
DECLARE( test_minimizer );
DECLARE( test_cylinder_map );
DECLARE( test_spherical_map );

void
register_tests()
{
  REGISTER( test_minimizer );
  REGISTER( test_icam_transform );
  REGISTER( test_cylinder_map );
  REGISTER( test_spherical_map );
}

DEFINE_MAIN;
