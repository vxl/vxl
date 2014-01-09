# This is contrib/brl/bseg/bvxm/pro/CMakeLists.txt
# bvxm's Process Library

# bprb depends on expat and bvxm_pro depends on bprb primarily
INCLUDE( ${BRL_MODULE_PATH}/FindEXPAT.cmake )
IF(EXPAT_FOUND)
  INCLUDE_DIRECTORIES( ${EXPAT_INCLUDE_DIR} )
  INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bpro )
  INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bpro/bprb )
  INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bseg )
  INCLUDE_DIRECTORIES( ${GEL_INCLUDE_DIR}/mrc )
  INCLUDE_DIRECTORIES( ${GEL_INCLUDE_DIR})
  INCLUDE(${VXL_CMAKE_DIR}/FindTIFF.cmake)
  IF(TIFF_FOUND)
    INCLUDE_DIRECTORIES(${TIFF_INCLUDE_DIR})
    INCLUDE( ${VXL_CMAKE_DIR}/FindGEOTIFF.cmake)
    IF(GEOTIFF_FOUND)
      INCLUDE_DIRECTORIES(${GEOTIFF_INCLUDE_DIR})
    ENDIF(GEOTIFF_FOUND)
  ENDIF(TIFF_FOUND)
  IF(EXPATPP_FOUND)
    INCLUDE_DIRECTORIES (${EXPAT_INCLUDE_DIRS} )
    INCLUDE_DIRECTORIES (${EXPATPP_INCLUDE_DIRS} )
  ENDIF(EXPATPP_FOUND)
  SET(bvxm_pro_sources
      bvxm_processes.h
      bvxm_register.h    bvxm_register.cxx
     )

  AUX_SOURCE_DIRECTORY(Templates bvxm_pro_sources)

  SET(bvxm_pro_sources ${bvxm_pro_sources}
    processes/bvxm_normalization_util.h               processes/bvxm_normalization_util.cxx
    processes/bvxm_illum_index_process.h              processes/bvxm_illum_index_process.cxx
    processes/bvxm_create_local_rpc_process.h         processes/bvxm_create_local_rpc_process.cxx
    processes/bvxm_compare_3d_voxels_process.h        processes/bvxm_compare_3d_voxels_process.cxx
    processes/bvxm_get_grid_process.h                 processes/bvxm_get_grid_process.cxx
    processes/bvxm_rpc_registration_process.h         processes/bvxm_rpc_registration_process.cxx
    processes/bvxm_render_expected_image_process.h    processes/bvxm_render_expected_image_process.cxx
    processes/bvxm_locate_region_process.h            processes/bvxm_locate_region_process.cxx
    processes/bvxm_update_edges_process.h             processes/bvxm_update_edges_process.cxx
    processes/bvxm_ocp_compare_process.h              processes/bvxm_ocp_compare_process.cxx
    processes/bvxm_clean_world_process.h              processes/bvxm_clean_world_process.cxx
    processes/bvxm_detect_scale_process.h             processes/bvxm_detect_scale_process.cxx
    processes/bvxm_update_lidar_process.h             processes/bvxm_update_lidar_process.cxx
    processes/bvxm_ocp_hist_process.h                 processes/bvxm_ocp_hist_process.cxx
    processes/bvxm_roc_process.h                      processes/bvxm_roc_process.cxx
    processes/bvxm_lidar_edge_detection_process.h     processes/bvxm_lidar_edge_detection_process.cxx
    processes/bvxm_create_normalized_image_process.h  processes/bvxm_create_normalized_image_process.cxx
    processes/bvxm_change_detection_display_process.h processes/bvxm_change_detection_display_process.cxx
    processes/bvxm_create_voxel_world_process.h       processes/bvxm_create_voxel_world_process.cxx
    processes/bvxm_normalize_image_process.h          processes/bvxm_normalize_image_process.cxx
    processes/bvxm_roi_init_process.h                 processes/bvxm_roi_init_process.cxx
    processes/bvxm_heightmap_process.h                processes/bvxm_heightmap_process.cxx
	processes/bvxm_heightmap_ortho_process.h          processes/bvxm_heightmap_ortho_process.cxx
    processes/bvxm_update_process.h                   processes/bvxm_update_process.cxx
    processes/bvxm_detect_changes_process.h           processes/bvxm_detect_changes_process.cxx
    processes/bvxm_save_occupancy_raw_process.h       processes/bvxm_save_occupancy_raw_process.cxx
    processes/bvxm_save_occupancy_vff_process.h       processes/bvxm_save_occupancy_vff_process.cxx
    processes/bvxm_pmap_ratio_process.h               processes/bvxm_pmap_ratio_process.cxx
    processes/bvxm_render_virtual_view_process.h      processes/bvxm_render_virtual_view_process.cxx
    processes/bvxm_update_edges_lidar_process.h       processes/bvxm_update_edges_lidar_process.cxx
    processes/bvxm_lidar_init_process.h               processes/bvxm_lidar_init_process.cxx
    processes/bvxm_detect_edge_tangent_process.h      processes/bvxm_detect_edge_tangent_process.cxx
    processes/bvxm_detect_edges_process.h             processes/bvxm_detect_edges_process.cxx
    processes/bvxm_update_edge_tangent_process.h      processes/bvxm_update_edge_tangent_process.cxx
    processes/bvxm_compare_rpc_process.h              processes/bvxm_compare_rpc_process.cxx
    processes/bvxm_pmap_hist_process.h                processes/bvxm_pmap_hist_process.cxx
    processes/bvxm_create_mog_image_process.h         processes/bvxm_create_mog_image_process.cxx
    processes/bvxm_synth_world_generator.h            processes/bvxm_synth_world_generator.cxx
    processes/bvxm_gen_synthetic_world_process.h      processes/bvxm_gen_synthetic_world_process.cxx
    processes/bvxm_save_edges_raw_process.h           processes/bvxm_save_edges_raw_process.cxx
    processes/bvxm_display_edge_tangent_world_vrml_process.h processes/bvxm_display_edge_tangent_world_vrml_process.cxx
    processes/bvxm_expected_edge_image_process.h      processes/bvxm_expected_edge_image_process.cxx
    processes/bvxm_create_synth_lidar_data_process.h  processes/bvxm_create_synth_lidar_data_process.cxx
    processes/bvxm_update_point_cloud_process.h       processes/bvxm_update_point_cloud_process.cxx
    processes/bvxm_report_statistics_point_cloud_process.h processes/bvxm_report_statistics_point_cloud_process.cxx
    processes/bvxm_estimate_camera_process.h          processes/bvxm_estimate_camera_process.cxx
    processes/bvxm_expected_edge_functor.h            processes/bvxm_expected_edge_functor.cxx
    processes/bvxm_estimate_camera_utils.h            processes/bvxm_estimate_camera_utils.cxx
    processes/bvxm_atmospheric_corr_process.h         processes/bvxm_atmospheric_corr_process.cxx
    processes/bvxm_edgemap_ortho_process.h            processes/bvxm_edgemap_ortho_process.cxx
    processes/bvxm_scene_box_process.h                processes/bvxm_scene_box_process.cxx
    processes/bvxm_scene_kml_process.h                processes/bvxm_scene_kml_process.cxx
    processes/bvxm_scene_local_box_process.h          processes/bvxm_scene_local_box_process.cxx
    processes/bvxm_create_ortho_camera_process.h      processes/bvxm_create_ortho_camera_process.cxx
    processes/bvxm_scene_model_dir_process.h          processes/bvxm_scene_model_dir_process.cxx
    processes/bvxm_orthorectify_process.h             processes/bvxm_orthorectify_process.cxx
    processes/bvxm_dem_to_xyz_process.h               processes/bvxm_dem_to_xyz_process.cxx
  )

  ADD_LIBRARY(bvxm_pro ${bvxm_pro_sources})

  TARGET_LINK_LIBRARIES(bvxm_pro bprb bvxm bvxm_io bvxm_grid bvxm_grid_io expatpp sdet vpgl vpgl_io vpgl_algo vpgl_file_formats vpgl_pro vsol vil_io vil_pro vgl_algo vnl_algo vnl_io vnl vil_algo vil vgl vul brdb vbl_io bkml)

  IF( BUILD_TESTING )
    SUBDIRS(tests)
  ENDIF( BUILD_TESTING )
ENDIF(EXPAT_FOUND)
