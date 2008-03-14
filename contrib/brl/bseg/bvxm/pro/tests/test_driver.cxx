#include <testlib/testlib_register.h>

DECLARE( test_bvxm_roi_init_process );
DECLARE( test_bvxm_illum_index_process );
DECLARE( test_bvxm_create_voxel_world_process );
DECLARE( test_bvxm_normalize_image_process );
DECLARE( test_bvxm_generate_edge_map_process );
DECLARE( test_bvxm_rpc_registration_process );

void register_tests()
{
  REGISTER( test_bvxm_roi_init_process );
  REGISTER( test_bvxm_illum_index_process );
  REGISTER( test_bvxm_create_voxel_world_process );
  REGISTER( test_bvxm_normalize_image_process );
  REGISTER( test_bvxm_generate_edge_map_process );
  REGISTER( test_bvxm_rpc_registration_process );
}

DEFINE_MAIN;
