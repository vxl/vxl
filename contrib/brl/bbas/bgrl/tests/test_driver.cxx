#include <testlib/testlib_register.h>

DECLARE( test_graph );
DECLARE( test_search );

void
register_tests()
{
  REGISTER ( test_graph );
  REGISTER ( test_search );
}

DEFINE_MAIN;
