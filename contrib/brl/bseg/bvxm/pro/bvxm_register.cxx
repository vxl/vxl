#include "bvxm_register.h"

#include <bprb/bprb_macros.h>
#include <bprb/bprb_batch_process_manager.h>

#include "bvxm_create_voxel_world_process.h"
#include "bvxm_detect_changes_process.h"
#include "bvxm_generate_edge_map_process.h"
#include "bvxm_render_virtual_view_process.h"
#include "bvxm_render_expected_image_process.h"
#include "bvxm_change_detection_display_process.h"
#include "bvxm_update_process.h"
#include "bvxm_update_lidar_process.h"
#include "bvxm_normalize_image_process.h"
#include "bvxm_illum_index_process.h"
#include "bvxm_roi_init_process.h"
#include "bvxm_rpc_registration_process.h"
#include "bvxm_rpc_prob_registration_process.h"
#include "bvxm_save_edges_raw_process.h"
#include "bvxm_save_occupancy_raw_process.h"
#include "bvxm_save_occupancy_vff_process.h"
#include "bvxm_clean_world_process.h"
#include "bvxm_create_local_rpc_process.h"
#include "bvxm_create_normalized_image_process.h"
#include "bvxm_lidar_init_process.h"
#include "bvxm_gen_synthetic_world_process.h"
#include "bvxm_create_synth_lidar_data_process.h"
#include "bvxm_heightmap_process.h"
#include "bvxm_compare_rpc_process.h"
#include "bvxm_ocp_compare_process.h"
#include "bvxm_ocp_hist_process.h"
#include "bvxm_pmap_hist_process.h"
#include "bvxm_pmap_ratio_process.h"
#include "bvxm_roc_process.h"
#include "bvxm_update_edges_lidar_process.h"
#include "bvxm_lidar_edge_detection_process.h"
#include "multiscale_pro/bvxm_detect_scale_process.h"
#include "multiscale_pro/bvxm_compare_3d_voxels_process.h"

#include <bvxm/bvxm_voxel_world.h>

void bvxm_register::register_datatype()
{
  REGISTER_DATATYPE( bvxm_voxel_world_sptr );
}

void bvxm_register::register_process()
{
  REG_PROCESS(bvxm_create_voxel_world_process, bprb_batch_process_manager); 
  REG_PROCESS(bvxm_detect_changes_process, bprb_batch_process_manager);
  REG_PROCESS(bvxm_generate_edge_map_process, bprb_batch_process_manager);
  REG_PROCESS(bvxm_render_virtual_view_process,bprb_batch_process_manager);
  REG_PROCESS(bvxm_render_expected_image_process,bprb_batch_process_manager);
  REG_PROCESS(bvxm_change_detection_display_process,bprb_batch_process_manager);
  REG_PROCESS(bvxm_update_process,bprb_batch_process_manager);
  REG_PROCESS(bvxm_update_lidar_process,bprb_batch_process_manager);
  REG_PROCESS(bvxm_normalize_image_process,bprb_batch_process_manager);
  REG_PROCESS(bvxm_illum_index_process,bprb_batch_process_manager);
  REG_PROCESS(bvxm_roi_init_process,bprb_batch_process_manager);
  REG_PROCESS(bvxm_rpc_registration_process,bprb_batch_process_manager);
  REG_PROCESS(bvxm_rpc_prob_registration_process,bprb_batch_process_manager);
  REG_PROCESS(bvxm_save_edges_raw_process,bprb_batch_process_manager);
  REG_PROCESS(bvxm_save_occupancy_raw_process,bprb_batch_process_manager);
  REG_PROCESS(bvxm_save_occupancy_vff_process,bprb_batch_process_manager);
  REG_PROCESS(bvxm_clean_world_process,bprb_batch_process_manager);
  REG_PROCESS(bvxm_create_local_rpc_process,bprb_batch_process_manager);
  REG_PROCESS(bvxm_create_normalized_image_process,bprb_batch_process_manager);
  REG_PROCESS(bvxm_lidar_init_process,bprb_batch_process_manager);
  REG_PROCESS(bvxm_gen_synthetic_world_process,bprb_batch_process_manager);
  REG_PROCESS(bvxm_create_synth_lidar_data_process,bprb_batch_process_manager);
  REG_PROCESS(bvxm_heightmap_process,bprb_batch_process_manager);
  REG_PROCESS(bvxm_compare_rpc_process,bprb_batch_process_manager);
  REG_PROCESS(bvxm_ocp_compare_process,bprb_batch_process_manager);
  REG_PROCESS(bvxm_ocp_hist_process,bprb_batch_process_manager);
  REG_PROCESS(bvxm_pmap_hist_process,bprb_batch_process_manager);
  REG_PROCESS(bvxm_pmap_ratio_process,bprb_batch_process_manager);
  REG_PROCESS(bvxm_roc_process,bprb_batch_process_manager);
  REG_PROCESS(bvxm_update_edges_lidar_process,bprb_batch_process_manager);
  REG_PROCESS(bvxm_lidar_edge_detection_process,bprb_batch_process_manager);
  REG_PROCESS(bvxm_detect_scale_process,bprb_batch_process_manager);
  REG_PROCESS(bvxm_compare_3d_voxels_process,bprb_batch_process_manager);

}