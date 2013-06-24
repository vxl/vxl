#include <testlib/testlib_register.h>

DECLARE( test_gaussian_full );
DECLARE( test_gaussian_indep );
DECLARE( test_gaussian_sphere );
DECLARE( test_parzen_sphere );
DECLARE( test_weibull );
DECLARE( test_von_mises );
DECLARE( test_mixture );
DECLARE( test_bsta_histogram );
DECLARE( test_k_medoid );
DECLARE( test_k_means );
DECLARE( test_otsu_threshold );
DECLARE( test_sampler );
DECLARE( test_beta );
DECLARE( test_kent );
DECLARE( test_spherical_histogram );
void
register_tests()
{
  REGISTER( test_von_mises );
  REGISTER( test_gaussian_full );
  REGISTER( test_gaussian_indep );
  REGISTER( test_gaussian_sphere );
  REGISTER( test_parzen_sphere );
  REGISTER( test_weibull );
  REGISTER( test_mixture );
  REGISTER( test_bsta_histogram );
  REGISTER( test_k_medoid );
  REGISTER( test_k_means );
  REGISTER( test_otsu_threshold );
  REGISTER( test_sampler );
  REGISTER( test_beta );
  REGISTER( test_kent );
  REGISTER( test_spherical_histogram );
}

DEFINE_MAIN;
