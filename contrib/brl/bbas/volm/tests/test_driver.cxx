#include <testlib/testlib_register.h>


DECLARE( test_spherical_container );
DECLARE( test_tile );
DECLARE( test_spherical_shell_container );
DECLARE( test_loc_hyp );

void
register_tests()
{
  REGISTER( test_spherical_container );
  REGISTER( test_tile );
  REGISTER( test_spherical_shell_container );
  REGISTER( test_loc_hyp );
}

DEFINE_MAIN;

