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

SOURCES += multiscale_pro/bvxm_compare_3d_voxels_process.cxx
SOURCES += multiscale_pro/bvxm_create_multiscale_voxel_world_process.cxx
SOURCES += multiscale_pro/bvxm_detect_multiscale_process.cxx
SOURCES += multiscale_pro/bvxm_detect_scale_process.cxx
SOURCES += multiscale_pro/bvxm_save_multiscale_occupancy_raw_process.cxx
SOURCES += multiscale_pro/bvxm_update_multiscale_process.cxx

#######################################
include ${VXLROOT}/config/top-params.mk
#######################################

ifndef NOTEST
SUBDIRS += tests
endif

######################################
include ${VXLROOT}/config/top-rules.mk
######################################
