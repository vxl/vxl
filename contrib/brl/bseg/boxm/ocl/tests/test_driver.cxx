#include <testlib/testlib_register.h>

DECLARE( test_octree );
DECLARE( test_octree_stack );
DECLARE( test_octree_image );
DECLARE( test_backproject_ray );
DECLARE( test_expected_image );
DECLARE( test_ray_bundle );
DECLARE( test_statistics );
DECLARE( test_refine );

void register_tests()
{
  REGISTER( test_octree );
  REGISTER( test_octree_stack );
  REGISTER( test_octree_image );
  REGISTER( test_backproject_ray );
  REGISTER( test_expected_image );
  REGISTER( test_ray_bundle );
  REGISTER( test_statistics );
  REGISTER( test_refine );
}


DEFINE_MAIN;
