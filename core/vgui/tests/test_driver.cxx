#include <testlib/testlib_register.h>

DECLARE( test_generic_image_view );
DECLARE( test_displaybase );
DECLARE( test_pixels );

void
register_tests()
{
  REGISTER( test_generic_image_view );
  REGISTER( test_displaybase );
  REGISTER( test_pixels );
}

DEFINE_MAIN;
