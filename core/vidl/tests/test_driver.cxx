#include <testlib/testlib_register.h>

DECLARE( test_pixel_format );
DECLARE( test_pixel_iterator );
DECLARE( test_color);
DECLARE( test_convert);

void
register_tests()
{
  REGISTER( test_pixel_format );
  REGISTER( test_pixel_iterator );
  REGISTER( test_color );
  REGISTER( test_convert );
}

DEFINE_MAIN;
