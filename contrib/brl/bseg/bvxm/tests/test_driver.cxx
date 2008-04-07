#include <testlib/testlib_register.h>

DECLARE( test_voxel_storage_mem );
DECLARE( test_voxel_storage_disk );
DECLARE( test_voxel_grid );
DECLARE( test_apm_processors );
DECLARE( test_lidar_processor );
DECLARE( test_voxel_world );
DECLARE( test_voxel_world_update );
DECLARE( test_voxel_world_mog_image );
DECLARE( test_gen_synthetic_world );
DECLARE( test_lidar_camera );

void register_tests()
{
  REGISTER( test_voxel_storage_mem );
  REGISTER( test_voxel_storage_disk );
  REGISTER( test_apm_processors );
  REGISTER( test_lidar_processor );
  REGISTER( test_voxel_grid );
  REGISTER( test_voxel_world );
  REGISTER( test_voxel_world_update );
  REGISTER( test_voxel_world_mog_image );
  REGISTER( test_gen_synthetic_world );
  REGISTER( test_lidar_camera );
}

DEFINE_MAIN;
