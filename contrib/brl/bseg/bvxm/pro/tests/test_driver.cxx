#include <testlib/testlib_register.h>

DECLARE( test_bvxm_roi_init_process );
DECLARE( test_bvxm_illum_index_process );
DECLARE( test_bvxm_create_voxel_world_process );
DECLARE( test_bvxm_normalize_image_process );
DECLARE( test_bvxm_create_normalized_image_process );
DECLARE( test_bvxm_rpc_registration_process );
DECLARE( test_bvxm_create_local_rpc_process );
DECLARE( test_bvxm_update_lidar_process );
DECLARE( test_bvxm_change_detection_display_process);
DECLARE( test_bvxm_create_synth_lidar_data_process);

void register_tests()
{
  REGISTER( test_bvxm_roi_init_process );
  REGISTER( test_bvxm_illum_index_process );
  REGISTER( test_bvxm_create_voxel_world_process );
  REGISTER( test_bvxm_normalize_image_process );
  REGISTER( test_bvxm_create_normalized_image_process );
  REGISTER( test_bvxm_rpc_registration_process );
  REGISTER( test_bvxm_create_local_rpc_process );
  REGISTER( test_bvxm_change_detection_display_process);
  REGISTER( test_bvxm_create_synth_lidar_data_process);
  REGISTER( test_bvxm_update_lidar_process );
}

DEFINE_MAIN;
