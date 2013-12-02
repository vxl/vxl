#include <testlib/testlib_register.h>

DECLARE( test_voxel_storage_mem );
DECLARE( test_voxel_storage_slab_mem );
DECLARE( test_voxel_storage_disk );
DECLARE( test_voxel_storage_disk_cached );
DECLARE( test_voxel_grid );
DECLARE( test_basic_ops );
DECLARE( test_grid_to_image_stack );
DECLARE( test_bvxm_vrml );

void register_tests()
{
  REGISTER( test_voxel_storage_mem );
  REGISTER( test_voxel_storage_slab_mem );
  REGISTER( test_voxel_storage_disk );
  REGISTER( test_voxel_storage_disk_cached );
  REGISTER( test_voxel_grid );
  REGISTER( test_basic_ops );
  REGISTER( test_grid_to_image_stack );
  REGISTER( test_bvxm_vrml);
}

DEFINE_MAIN;
