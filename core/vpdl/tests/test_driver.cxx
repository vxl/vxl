#include <testlib/testlib_register.h>


DECLARE( test_eigen_sym_matrix );
DECLARE( test_gaussian_sphere );
DECLARE( test_gaussian_indep );
DECLARE( test_gaussian );
DECLARE( test_update_gaussian );
DECLARE( test_mixture );
DECLARE( test_mixture_of );
DECLARE( test_update_mog );
DECLARE( test_kernel_gaussian_sfbw );

void
register_tests()
{
  REGISTER( test_eigen_sym_matrix );
  REGISTER( test_gaussian_sphere );
  REGISTER( test_gaussian_indep );
  REGISTER( test_gaussian );
  REGISTER( test_update_gaussian );
  REGISTER( test_mixture );
  REGISTER( test_mixture_of );
  REGISTER( test_update_mog );
  REGISTER( test_kernel_gaussian_sfbw );
}

DEFINE_MAIN;
