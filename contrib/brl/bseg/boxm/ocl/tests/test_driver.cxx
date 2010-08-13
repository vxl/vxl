#include <testlib/testlib_register.h>

DECLARE( test_octree );
DECLARE( test_octree_image );
DECLARE( test_backproject_ray );
DECLARE( test_expected_image );
DECLARE( test_ray_bundle );
DECLARE( test_statistics );
DECLARE( test_refine );
DECLARE( test_update );
DECLARE( test_file_io );
DECLARE( test_scene_io);
DECLARE( test_scene_convert);
DECLARE( test_camera);
DECLARE( test_bit_tree); 

void register_tests()
{
  REGISTER( test_octree );
  REGISTER( test_octree_image );
  REGISTER( test_backproject_ray );
  REGISTER( test_expected_image );
  REGISTER( test_ray_bundle );
  REGISTER( test_statistics );
  REGISTER( test_refine );
  REGISTER( test_update );
  REGISTER( test_file_io );
  REGISTER( test_scene_io );
  REGISTER( test_scene_convert );
  REGISTER( test_camera);
  REGISTER( test_bit_tree );
}


DEFINE_MAIN;
