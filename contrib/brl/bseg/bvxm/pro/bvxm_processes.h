#ifndef bvxm_processes_h_
#define bvxm_processes_h_

#include <bprb/bprb_func_process.h>

bool bvxm_change_detection_display_process_init(bprb_func_process& pro);
bool bvxm_change_detection_display_process(bprb_func_process& pro);

bool bvxm_clean_world_process_init(bprb_func_process& pro);
bool bvxm_clean_world_process(bprb_func_process& pro);

bool bvxm_compare_rpc_process_init(bprb_func_process& pro);
bool bvxm_compare_rpc_process(bprb_func_process& pro);

bool bvxm_create_local_rpc_process_init(bprb_func_process& pro);
bool bvxm_create_local_rpc_process(bprb_func_process& pro);

bool bvxm_create_normalized_image_process_init(bprb_func_process& pro);
bool bvxm_create_normalized_image_process(bprb_func_process& pro);

bool bvxm_create_synth_lidar_data_process_init(bprb_func_process& pro);
bool bvxm_create_synth_lidar_data_process(bprb_func_process& pro);

bool bvxm_create_voxel_world_process_init(bprb_func_process& pro);
bool bvxm_create_voxel_world_process(bprb_func_process& pro);

bool bvxm_detect_changes_process_init(bprb_func_process& pro);
bool bvxm_detect_changes_process_init(bprb_func_process& pro);

bool bvxm_gen_synthetic_world_process_init(bprb_func_process& pro);
bool bvxm_gen_synthetic_world_process(bprb_func_process& pro);

bool bvxm_generate_edge_map_process_init(bprb_func_process& pro);
bool bvxm_generate_edge_map_process(bprb_func_process& pro);

bool bvxm_heightmap_process_init(bprb_func_process& pro);
bool bvxm_heightmap_process(bprb_func_process& pro);

bool bvxm_illum_index_process_init(bprb_func_process& pro);
bool bvxm_illum_index_process(bprb_func_process& pro);

bool bvxm_lidar_edge_detection_process_init(bprb_func_process& pro);
bool bvxm_lidar_edge_detection_process(bprb_func_process& pro);

bool bvxm_lidar_init_process_init(bprb_func_process& pro);
bool bvxm_lidar_init_process(bprb_func_process& pro);

bool bvxm_normalize_image_process_init(bprb_func_process& pro);
bool bvxm_normalize_image_process(bprb_func_process& pro);

bool bvxm_ocp_compare_process_init(bprb_func_process& pro);
bool bvxm_ocp_compare_process(bprb_func_process& pro);

bool bvxm_ocp_hist_process_init(bprb_func_process& pro);
bool bvxm_ocp_hist_process(bprb_func_process& pro);

bool bvxm_pmap_hist_process_init(bprb_func_process& pro);
bool bvxm_pmap_hist_process(bprb_func_process& pro);

bool bvxm_pmap_ratio_process_init(bprb_func_process& pro);
bool bvxm_pmap_ratio_process(bprb_func_process& pro);

bool bvxm_render_expected_image_process_init(bprb_func_process& pro);
bool bvxm_render_expected_image_process(bprb_func_process& pro);

bool bvxm_render_virtual_view_process_init(bprb_func_process& pro);
bool bvxm_render_virtual_view_process(bprb_func_process& pro);

bool bvxm_roc_process_init(bprb_func_process& pro);
bool bvxm_roc_process(bprb_func_process& pro);

bool bvxm_roi_init_process_init(bprb_func_process& pro);
bool bvxm_roi_init_process(bprb_func_process& pro);

bool bvxm_rpc_prob_registration_process_init(bprb_func_process& pro);
bool bvxm_rpc_prob_registration_process(bprb_func_process& pro);

bool bvxm_rpc_registration_process_init(bprb_func_process& pro);
bool bvxm_rpc_registration_process(bprb_func_process& pro);

bool bvxm_save_edges_raw_process_init(bprb_func_process& pro);
bool bvxm_save_edges_raw_process(bprb_func_process& pro);

bool bvxm_save_occupancy_raw_process_init(bprb_func_process& pro);
bool bvxm_save_occupancy_raw_process(bprb_func_process& pro);

bool bvxm_save_occupancy_vff_process_init(bprb_func_process& pro);
bool bvxm_save_occupancy_vff_process(bprb_func_process& pro);

bool bvxm_test_process_init(bprb_func_process& pro);
bool bvxm_test_process(bprb_func_process& pro);

bool bvxm_update_edges_lidar_process_init(bprb_func_process& pro);
bool bvxm_update_edges_lidar_process(bprb_func_process& pro);

bool bvxm_update_lidar_process_init(bprb_func_process& pro);
bool bvxm_update_lidar_process(bprb_func_process& pro);

bool bvxm_update_process_init(bprb_func_process& pro);
bool bvxm_update_process(bprb_func_process& pro);

//the init functions

#endif
