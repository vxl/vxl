#include <testlib/testlib_register.h>

DECLARE( test_osl_to_vdgl );
DECLARE( test_DigitalCurve_to_vdgl );

void
register_tests()
{
  REGISTER( test_osl_to_vdgl );
  REGISTER( test_DigitalCurve_to_vdgl );
}

DEFINE_MAIN;
