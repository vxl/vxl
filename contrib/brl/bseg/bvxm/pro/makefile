USES += contrib/brl contrib/brl/bseg core

LIBRARY := bvxm_pro
VERSION := 1.0

SOURCES += bvxm_change_detection_display_process.cxx
SOURCES += bvxm_clean_world_process.cxx
SOURCES += bvxm_create_voxel_world_process.cxx
SOURCES += bvxm_detect_changes_process.cxx
SOURCES += bvxm_generate_edge_map_process.cxx
SOURCES += bvxm_illum_index_process.cxx
SOURCES += bvxm_normalize_image_process.cxx
SOURCES += bvxm_render_expected_image_process.cxx
SOURCES += bvxm_render_virtual_view_process.cxx
SOURCES += bvxm_roi_init_process.cxx
SOURCES += bvxm_rpc_registration_process.cxx
SOURCES += bvxm_save_occupancy_raw_process.cxx
SOURCES += bvxm_save_occupancy_vff_process.cxx
SOURCES += bvxm_update_process.cxx
SOURCES += bvxm_update_lidar_process.cxx
SOURCES += bvxm_create_local_rpc_process.cxx
SOURCES += bvxm_create_normalized_image_process.cxx
SOURCES += bvxm_create_synth_lidar_data_process.cxx
SOURCES += bvxm_gen_synthetic_world_process.cxx
SOURCES += bvxm_lidar_init_process.cxx
SOURCES += bvxm_save_edges_raw_process.cxx
SOURCES += bvxm_heightmap_process.cxx
SOURCES += bvxm_compare_rpc_process.cxx
SOURCES += bvxm_ocp_compare_process.cxx
SOURCES += bvxm_ocp_hist_process.cxx
SOURCES += bvxm_roc_process.cxx
SOURCES += bvxm_pmap_hist_process.cxx
SOURCES += bvxm_pmap_ratio_process.cxx
SOURCES += bvxm_rpc_prob_registration_process.cxx
SOURCES += bvxm_update_edges_lidar_process.cxx
SOURCES += bvxm_lidar_edge_detection_process.cxx
SOURCES += bvxm_register.cxx

SOURCES += multiscale_pro/bvxm_compare_3d_voxels_process.cxx
SOURCES += multiscale_pro/bvxm_detect_scale_process.cxx

MANPAGE_SOURCES += bvxm_processes.h
SOURCES += processes/bvxm_change_detection_display_process.cxx
SOURCES += processes/bvxm_clean_world_process.cxx
SOURCES += processes/bvxm_compare_rpc_process.cxx
SOURCES += processes/bvxm_create_normalized_image_process.cxx
SOURCES += processes/bvxm_create_local_rpc_processes.cxx
SOURCES += processes/bvxm_create_voxel_world_process.cxx
SOURCES += processes/bvxm_detect_changes_process.cxx
SOURCES += processes/bvxm_gen_synthetic_world_process.cxx
SOURCES += processes/bvxm_generate_edge_map_process.cxx
SOURCES += processes/bvxm_heightmap_process.cxx
SOURCES += processes/bvxm_illum_index_process.cxx
SOURCES += processes/bvxm_lidar_edge_detection_process.cxx
SOURCES += processes/bvxm_lidar_init_process.cxx
SOURCES += processes/bvxm_normalize_image_process.cxx
SOURCES += processes/bvxm_ocp_compare_process.cxx
SOURCES += processes/bvxm_ocp_hist_process.cxx
SOURCES += processes/bvxm_pmap_hist_process.cxx

#######################################
include ${VXLROOT}/config/top-params.mk
#######################################

ifndef NOTEST
SUBDIRS += tests
endif

######################################
include ${VXLROOT}/config/top-rules.mk
######################################
