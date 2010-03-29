#include <testlib/testlib_register.h>

DECLARE( test_bayesian_optimizer );
DECLARE( test_edges );

void register_tests()
{
  REGISTER( test_bayesian_optimizer );\
  REGISTER( test_edges );
}


DEFINE_MAIN;
