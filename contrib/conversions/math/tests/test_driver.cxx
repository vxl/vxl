#include <testlib/testlib_register.h>

DECLARE( test_matrix_conversions );
DECLARE( test_CoolMatrix_conversions );
DECLARE( test_IUE_matrix_conversions );

void
register_tests()
{
  REGISTER( test_matrix_conversions );
  REGISTER( test_CoolMatrix_conversions );
  REGISTER( test_IUE_matrix_conversions );
}

DEFINE_MAIN;
