#include <testlib/testlib_register.h>


DECLARE( test_npower );
DECLARE( test_eigen_sym_matrix );
DECLARE( test_gaussian_sphere );
DECLARE( test_gaussian_indep );
DECLARE( test_gaussian );
DECLARE( test_update_gaussian );
DECLARE( test_mixture );

void
register_tests()
{
  REGISTER( test_npower );
  REGISTER( test_eigen_sym_matrix );
  REGISTER( test_gaussian_sphere );
  REGISTER( test_gaussian_indep );
  REGISTER( test_gaussian );
  REGISTER( test_update_gaussian );
  REGISTER( test_mixture );
}

DEFINE_MAIN;
