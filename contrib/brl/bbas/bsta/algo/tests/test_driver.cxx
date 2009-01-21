#include <testlib/testlib_register.h>

DECLARE( test_fit_weibull );
DECLARE( test_gaussian_model );

void
register_tests()
{
  REGISTER( test_fit_weibull );
  REGISTER( test_gaussian_model );
}

DEFINE_MAIN;
