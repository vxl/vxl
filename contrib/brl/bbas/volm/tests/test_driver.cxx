#include <testlib/testlib_register.h>


DECLARE( test_spherical_container );
DECLARE( test_spherical_shell_container );
DECLARE( test_volm_query );

void
register_tests()
{
  REGISTER( test_spherical_container );
  REGISTER( test_spherical_shell_container );
}

DEFINE_MAIN;

