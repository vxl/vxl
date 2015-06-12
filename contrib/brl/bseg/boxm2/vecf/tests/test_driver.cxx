#include <testlib/testlib_register.h>


DECLARE( test_eye );
DECLARE( test_orbit );
DECLARE( test_fit_orbit );

void register_tests()
{

  REGISTER( test_eye );
  REGISTER( test_orbit );
  REGISTER( test_fit_orbit );
}

DEFINE_MAIN;
