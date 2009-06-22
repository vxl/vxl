#include <testlib/testlib_register.h>

DECLARE( test_voxel_storage_mem );
DECLARE( test_voxel_storage_disk );
DECLARE( test_voxel_storage_disk_cached );
DECLARE( test_voxel_grid );
DECLARE( test_basic_ops );

void register_tests()
{
  REGISTER( test_voxel_storage_mem );
  REGISTER( test_voxel_storage_disk );
  REGISTER( test_voxel_storage_disk_cached );
  REGISTER( test_voxel_grid );
  REGISTER( test_basic_ops );
}

DEFINE_MAIN;
