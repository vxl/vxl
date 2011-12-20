#include <testlib/testlib_register.h>

DECLARE( test_bounded_image_view );
DECLARE( test_raw_image_istream );


void register_tests()
{
  REGISTER( test_bounded_image_view );
  REGISTER( test_raw_image_istream );
}

DEFINE_MAIN;
