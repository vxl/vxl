# This is contrib/brl/bseg/bvxm/pro/CMakeLists.txt
# bvxm's Process Library

# bprb depends on expat and bvxm_pro depends on bprb primarily
include( ${BRL_MODULE_PATH}/FindEXPAT.cmake )
if(EXPAT_FOUND)
  include_directories( ${EXPAT_INCLUDE_DIR} )
  include_directories( ${BRL_INCLUDE_DIR}/bpro )
  include_directories( ${BRL_INCLUDE_DIR}/bpro/bprb )
  include_directories( ${BRL_INCLUDE_DIR}/bseg )
  include_directories( ${GEL_INCLUDE_DIR}/mrc )
  include_directories( ${GEL_INCLUDE_DIR})
  include(${VXL_CMAKE_DIR}/FindTIFF.cmake)
  if(TIFF_FOUND)
    include_directories(${TIFF_INCLUDE_DIR})
    include( ${VXL_CMAKE_DIR}/FindGEOTIFF.cmake)
    if(GEOTIFF_FOUND)
      include_directories(${GEOTIFF_INCLUDE_DIR})
    endif()
  endif()
  if(EXPATPP_FOUND)
    include_directories(${EXPAT_INCLUDE_DIRS} )
    include_directories(${EXPATPP_INCLUDE_DIRS} )
  endif()
  set(bvxm_pro_sources
      bvxm_processes.h
      bvxm_register.h    bvxm_register.cxx
     )

  aux_source_directory(Templates bvxm_pro_sources)

  set(bvxm_pro_sources ${bvxm_pro_sources}
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
    processes/bvxm_scene_lvcs_process.h               processes/bvxm_scene_lvcs_process.cxx
    processes/bvxm_orthorectify_process.h             processes/bvxm_orthorectify_process.cxx
    processes/bvxm_dem_to_xyz_process.h               processes/bvxm_dem_to_xyz_process.cxx
    processes/bvxm_create_land_map_process.h          processes/bvxm_create_land_map_process.cxx
    processes/bvxm_create_scene_xml_process.h         processes/bvxm_create_scene_xml_process.cxx
  )

  vxl_add_library(LIBRARY_NAME bvxm_pro LIBRARY_SOURCES ${bvxm_pro_sources})

  target_link_libraries(bvxm_pro bprb bvxm bvxm_io bvxm_grid bvxm_grid_io expatpp sdet ${VXL_LIB_PREFIX}vpgl ${VXL_LIB_PREFIX}vpgl_io ${VXL_LIB_PREFIX}vpgl_algo ${VXL_LIB_PREFIX}vpgl_file_formats vpgl_pro vsol ${VXL_LIB_PREFIX}vil_io vil_pro ${VXL_LIB_PREFIX}vgl_algo ${VXL_LIB_PREFIX}vnl_algo ${VXL_LIB_PREFIX}vnl_io ${VXL_LIB_PREFIX}vnl ${VXL_LIB_PREFIX}vil_algo ${VXL_LIB_PREFIX}vil ${VXL_LIB_PREFIX}vgl ${VXL_LIB_PREFIX}vul brdb ${VXL_LIB_PREFIX}vbl_io bkml volm bvgl_algo)

  if( BUILD_TESTING )
    add_subdirectory(tests)
  endif()
endif()
