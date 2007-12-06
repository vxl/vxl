#include <testlib/testlib_register.h>

DECLARE( test_pca );
DECLARE( test_trivial_ca );
DECLARE( test_general_ca );

void register_tests()
{
  REGISTER( test_pca );
  REGISTER( test_trivial_ca );
  REGISTER( test_general_ca );
}


DEFINE_MAIN;
