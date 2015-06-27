#include <testlib/testlib_register.h>


DECLARE( test_eye );
DECLARE( test_orbit );
DECLARE( test_fit_orbit );
DECLARE( test_fit_margin );

void register_tests()
{

  REGISTER( test_eye );
  REGISTER( test_orbit );
  REGISTER( test_fit_orbit );
  REGISTER( test_fit_margin );
}

DEFINE_MAIN;
