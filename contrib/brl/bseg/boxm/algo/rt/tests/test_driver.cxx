#include <testlib/testlib_register.h>

DECLARE( test_bayesian_optimizer );
DECLARE( test_edges );
DECLARE( test_shadow_appearance );

void register_tests()
{
  REGISTER( test_bayesian_optimizer );
  REGISTER( test_edges );
  REGISTER( test_shadow_appearance );
}


DEFINE_MAIN;
