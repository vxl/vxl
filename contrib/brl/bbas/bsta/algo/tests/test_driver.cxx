#include <testlib/testlib_register.h>

DECLARE( test_fit_weibull );
DECLARE( test_gaussian_model );
DECLARE( test_mean_shift );
DECLARE( test_von_mises_update );
DECLARE( test_beta_updater );


void
register_tests()
{
  REGISTER( test_fit_weibull );
  REGISTER( test_gaussian_model );
  REGISTER( test_mean_shift );
  REGISTER( test_von_mises_update );
  REGISTER( test_beta_updater );

}

DEFINE_MAIN;
