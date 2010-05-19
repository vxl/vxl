#include <testlib/testlib_register.h>

DECLARE( test_octree );
DECLARE( test_backproject_ray );
DECLARE( test_expected_image );

void register_tests()
{
  REGISTER( test_octree );
  REGISTER( test_backproject_ray );
  REGISTER( test_expected_image );

}


DEFINE_MAIN;
