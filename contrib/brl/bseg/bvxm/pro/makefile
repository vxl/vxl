USES += contrib/brl contrib/brl/bseg core

LIBRARY := bvxm_pro
VERSION := 1.0

SOURCES += bvxm_register.cxx

SOURCES += processes/bvxm_change_detection_display_process.cxx
SOURCES += processes/bvxm_clean_world_process.cxx
SOURCES += processes/bvxm_compare_rpc_process.cxx
SOURCES += processes/bvxm_create_local_rpc_process.cxx
SOURCES += processes/bvxm_create_normalized_image_process.cxx
SOURCES += processes/bvxm_create_voxel_world_process.cxx
SOURCES += processes/bvxm_detect_changes_process.cxx
SOURCES += processes/bvxm_gen_synthetic_world_process.cxx
SOURCES += processes/bvxm_heightmap_process.cxx
SOURCES += processes/bvxm_illum_index_process.cxx
SOURCES += processes/bvxm_lidar_edge_detection_process.cxx
SOURCES += processes/bvxm_lidar_init_process.cxx
SOURCES += processes/bvxm_normalize_image_process.cxx
SOURCES += processes/bvxm_ocp_compare_process.cxx
SOURCES += processes/bvxm_ocp_hist_process.cxx
SOURCES += processes/bvxm_pmap_hist_process.cxx
SOURCES += processes/bvxm_pmap_ratio_process.cxx
SOURCES += processes/bvxm_render_expected_image_process.cxx
SOURCES += processes/bvxm_render_virtual_view_process.cxx
SOURCES += processes/bvxm_roc_process.cxx
SOURCES += processes/bvxm_rpc_registration_process.cxx
SOURCES += processes/bvxm_save_edges_raw_process.cxx
SOURCES += processes/bvxm_save_occupancy_raw_process.cxx
SOURCES += processes/bvxm_save_occupancy_vff_process.cxx
SOURCES += processes/bvxm_update_edges_lidar_process.cxx
SOURCES += processes/bvxm_update_lidar_process.cxx
SOURCES += processes/bvxm_update_process.cxx
SOURCES += processes/bvxm_roi_init_process.cxx
SOURCES += processes/bvxm_normalization_util.cxx
SOURCES += processes/bvxm_update_edges_process.cxx
SOURCES += processes/bvxm_create_mog_image_process.cxx
SOURCES += processes/bvxm_compare_3d_voxels_process.cxx # multiscale_pro/bvxm_compare_3d_voxels_process.cxx
SOURCES += processes/bvxm_detect_scale_process.cxx      # multiscale_pro/bvxm_detect_scale_process.cxx
SOURCES += processes/bvxm_synth_world_generator.cxx
SOURCES += processes/bvxm_locate_region_process.cxx
SOURCES += processes/bvxm_get_grid_process.cxx
SOURCES += processes/bvxm_create_synth_lidar_data_process.cxx
SOURCES += processes/bvxm_update_point_cloud_process.cxx
SOURCES += processes/bvxm_report_statistics_point_cloud_process.cxx
SOURCES += processes/bvxm_display_edge_tangent_world_vrml_process.cxx
SOURCES += processes/bvxm_expected_edge_image_process.cxx
SOURCES += processes/bvxm_update_edge_tangent_process.cxx
SOURCES += processes/bvxm_detect_edge_tangent_process.cxx
SOURCES += processes/bvxm_estimate_camera_process.cxx
SOURCES += processes/bvxm_estimate_camera_utils.cxx
SOURCES += processes/bvxm_expected_edge_functor.cxx
SOURCES += processes/bvxm_atmospheric_corr_process.cxx

MANPAGE_SOURCES += bvxm_processes.h

#######################################
include ${VXLROOT}/config/top-params.mk
#######################################

ifndef NOTEST
SUBDIRS += tests
endif

######################################
include ${VXLROOT}/config/top-rules.mk
######################################
