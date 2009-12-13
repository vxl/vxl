#include <testlib/testlib_register.h>

DECLARE( test_apm_processors );
DECLARE( test_lidar_processor );
DECLARE( test_voxel_world );
DECLARE( test_voxel_world_update );
DECLARE( test_voxel_world_mog_image );
DECLARE( test_platform_computations );
DECLARE( test_tangent_update );
DECLARE( test_illum );
void register_tests()
{
  REGISTER( test_apm_processors );
  REGISTER( test_lidar_processor );
  REGISTER( test_voxel_world );
  REGISTER( test_voxel_world_update );
  REGISTER( test_voxel_world_mog_image );
  REGISTER( test_platform_computations );
  REGISTER( test_tangent_update );
  REGISTER( test_illum );
}

DEFINE_MAIN;
