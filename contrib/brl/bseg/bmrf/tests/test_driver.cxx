#include <testlib/testlib_register.h>

DECLARE( test_node );
DECLARE( test_epi_point );

void
register_tests()
{
  REGISTER ( test_node );
  REGISTER ( test_epi_point );
}

DEFINE_MAIN;
