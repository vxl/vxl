#include <testlib/testlib_register.h>

DECLARE( test_marked_images_from_files );
DECLARE( test_reflected_marked_images );

void register_tests()
{
  REGISTER( test_marked_images_from_files );
  REGISTER( test_reflected_marked_images );
}


DEFINE_MAIN;
