#include <testlib/testlib_register.h>

DECLARE( test_transform_3d );
DECLARE( test_image_3d_of );
DECLARE( test_load );
void
register_tests()
{
  REGISTER( test_transform_3d );
  REGISTER( test_image_3d_of );
  REGISTER( test_load );
}

DEFINE_MAIN;
