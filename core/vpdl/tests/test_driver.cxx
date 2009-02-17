#include <testlib/testlib_register.h>

DECLARE( test_eigen_sym_matrix );
DECLARE( test_gaussian_sphere );
DECLARE( test_gaussian_indep );
DECLARE( test_gaussian );

void
register_tests()
{
  
  REGISTER( test_eigen_sym_matrix );
  REGISTER( test_gaussian_sphere );
  REGISTER( test_gaussian_indep );
  REGISTER( test_gaussian );
}

DEFINE_MAIN;
