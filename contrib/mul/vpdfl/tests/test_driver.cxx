#include <testlib/testlib_register.h>

DECLARE( test_mixture );
DECLARE( test_axis_gaussian );
DECLARE( test_gaussian );
DECLARE( test_pc_gaussian );

void register_tests()
{
  REGISTER( test_mixture );
  REGISTER( test_axis_gaussian );
  REGISTER( test_gaussian );
  REGISTER( test_pc_gaussian );
}


DEFINE_MAIN;
