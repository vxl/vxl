#include <testlib/testlib_register.h>

DECLARE( test_graph_rep1 );
DECLARE( test_dp_solver );

void
register_tests()
{
  REGISTER( test_graph_rep1 );
  REGISTER( test_dp_solver );
}

DEFINE_MAIN;
