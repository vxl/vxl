#include <testlib/testlib_register.h>

DECLARE( test_displaybase );
DECLARE( test_pixels );
DECLARE( test_image_tableau );
DECLARE( test_range_map );

void
register_tests()
{
  REGISTER( test_displaybase );
  REGISTER( test_pixels );
  REGISTER( test_image_tableau );
  REGISTER( test_range_map );
}

DEFINE_MAIN;
