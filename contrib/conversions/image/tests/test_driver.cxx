#include <testlib/testlib_register.h>

DECLARE( test_vil1_vbl_conversions );
DECLARE( test_image_conversions );

void
register_tests()
{
  REGISTER( test_vil1_vbl_conversions );
  REGISTER( test_image_conversions );
}

DEFINE_MAIN;
