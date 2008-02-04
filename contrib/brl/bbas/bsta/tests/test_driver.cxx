#include <testlib/testlib_register.h>

DECLARE( test_gaussian_full );
DECLARE( test_gaussian_indep );
DECLARE( test_gaussian_sphere );
DECLARE( test_mixture );
DECLARE( test_bsta_histogram );
DECLARE( test_k_medoid );
DECLARE( test_otsu_threshold );

void
register_tests()
{
  REGISTER( test_gaussian_full );
  REGISTER( test_gaussian_indep );
  REGISTER( test_gaussian_sphere );
  REGISTER( test_mixture );
  REGISTER( test_bsta_histogram );
  REGISTER( test_k_medoid );
  REGISTER( test_otsu_threshold );
}

DEFINE_MAIN;
