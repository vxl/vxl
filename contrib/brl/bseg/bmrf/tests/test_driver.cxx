#include <testlib/testlib_register.h>

DECLARE( test_node );
DECLARE( test_epi_point );
DECLARE( test_epi_seg );

void
register_tests()
{
  REGISTER ( test_node );
  REGISTER ( test_epi_point );
  REGISTER ( test_epi_seg );
}

DEFINE_MAIN;
