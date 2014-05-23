#include <testlib/testlib_register.h>


DECLARE(test_legendre_polynomial );
DECLARE( test_fresnel );

void
register_tests()
{
  REGISTER( test_legendre_polynomial );
  REGISTER( test_fresnel );
}

DEFINE_MAIN;
