#include <testlib/testlib_register.h>


DECLARE( test_eye );
DECLARE( test_orbit );
DECLARE( test_fit_orbit );
DECLARE( test_fit_margin );
DECLARE( test_pc_viewer );
DECLARE( test_mandible);
DECLARE( test_cranium);

void register_tests()
{

  REGISTER( test_eye );
  REGISTER( test_orbit );
  REGISTER( test_fit_orbit );
  REGISTER( test_fit_margin );
  REGISTER( test_pc_viewer );
  REGISTER( test_mandible );
  REGISTER( test_cranium );
}

DEFINE_MAIN;
