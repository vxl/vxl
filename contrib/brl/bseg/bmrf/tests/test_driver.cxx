#include <testlib/testlib_register.h>

DECLARE( test_epi_point );
DECLARE( test_epi_seg );
DECLARE( test_node );
DECLARE( test_network );

void
register_tests()
{
  REGISTER ( test_epi_point );
  REGISTER ( test_epi_seg );
  REGISTER ( test_node );
  REGISTER ( test_network );
}

DEFINE_MAIN;
