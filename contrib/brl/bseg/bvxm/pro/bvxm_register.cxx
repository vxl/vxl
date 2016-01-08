#include "bvxm_register.h"

#include <bprb/bprb_macros.h>
#include <bprb/bprb_batch_process_manager.h>
#include <bprb/bprb_func_process.h>
#include "bvxm_processes.h"
#include <bvxm/bvxm_voxel_world.h>
#include <bvxm/grid/bvxm_voxel_slab_base.h>


void bvxm_register::register_datatype()
{
  REGISTER_DATATYPE( bvxm_voxel_world_sptr );
  REGISTER_DATATYPE( bvxm_voxel_slab_base_sptr );
}

void bvxm_register::register_process()
{

  //Voxel world processes
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bvxm_create_voxel_world_process, "bvxmCreateVoxelWorldProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bvxm_detect_changes_process, "bvxmDetectChangesProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bvxm_change_detection_display_process, "bvxmChangeDetectionDisplayProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bvxm_expected_edge_image_process, "bvxmExpectedEdgeImageProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bvxm_detect_edge_tangent_process, "bvxmDetectEdgeTangentProcess");
  REG_PROCESS_FUNC_CONS_INIT(bprb_func_process, bprb_batch_process_manager, bvxm_update_edge_tangent_process, "bvxmUpdateEdgeTangentProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bvxm_render_virtual_view_process, "bvxmRenderVirtualViewProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bvxm_render_expected_image_process,"bvxmRenderExpectedImageProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bvxm_update_process, "bvxmUpdateProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bvxm_create_synth_lidar_data_process, "bvxmCreateSynthLidarDataProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bvxm_update_lidar_process, "bvxmUpdateLidarProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bvxm_lidar_edge_detection_process,"bvxmLidarEdgeDetectionProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bvxm_heightmap_process, "bvxmHeightmapProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bvxm_compare_rpc_process, "bvxmCompareRpcProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bvxm_ocp_compare_process, "bvxmOcpCompareProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bvxm_ocp_hist_process, "bvxmOcpHistProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bvxm_pmap_hist_process, "bvxmPmapHistProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bvxm_pmap_ratio_process,"bvxmPmapRatioProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bvxm_clean_world_process, "bvxmCleanWorldProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bvxm_create_local_rpc_process, "bvxmCreateLocalRpcProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bvxm_create_normalized_image_process, "bvxmCreateNormalizedImageProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bvxm_lidar_init_process, "bvxmLidarInitProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bvxm_gen_synthetic_world_process, "bvxmGenSyntheticWorldProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bvxm_normalize_image_process, "bvxmNormalizeImageProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bvxm_illum_index_process, "bvxmIllumIndexProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bvxm_roi_init_process, "bvxmRoiInitProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bvxm_compare_3d_voxels_process,"bvxmCompare3dVoxelsProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bvxm_detect_scale_process,"bvxmDetectScaleProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bvxm_lidar_edge_detection_process,"bvxmLidarEdgeDetectionProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bvxm_update_edges_process, "bvxmUpdateEdgesProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bvxm_update_edges_lidar_process, "bvxmUpdateEdgesLidarProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bvxm_roc_process,"bvxmRocProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bvxm_save_occupancy_vff_process, " bvxmSaveOccupancyVffPprocess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bvxm_save_occupancy_raw_process,"bvxmSaveOccupancyRawProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bvxm_save_edges_raw_process, "bvxmSaveEdgesRawProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bvxm_rpc_registration_process,"bvxmRpcRegistrationProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bvxm_create_mog_image_process,"bvxmCreateMOGImageProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bvxm_locate_region_process,"bvxmLocateRegionProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bvxm_get_grid_process,"bvxmGetGridProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bvxm_display_edge_tangent_world_vrml_process,"bvxmDisplayEdgeTangentWorldVRMLProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bvxm_update_point_cloud_process,"bvxmUpdatePointCloudProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bvxm_report_statistics_point_cloud_process,"bvxmReportStatisticsPointCloudProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bvxm_estimate_camera_process,"bvxmEstimateCameraProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bvxm_heightmap_ortho_process, "bvxmHeightmapOrthoProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bvxm_detect_edges_process, "bvxmDetectEdgesProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bvxm_edgemap_ortho_process, "bvxmEdgemapOrthoProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bvxm_scene_box_process, "bvxmSceneBoxProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bvxm_scene_local_box_process, "bvxmSceneLocalBoxProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bvxm_create_ortho_camera_process, "bvxmCreateOrthoCameraProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bvxm_scene_model_dir_process, "bvxmSceneModelDirProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bvxm_scene_lvcs_process, "bvxmSceneLvcsProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bvxm_scene_origin_process, "bvxmSceneOriginProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bvxm_orthorectify_process, "bvxmOrthorectifyProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bvxm_scene_kml_process, "bvxmSceneKmlProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bvxm_scene_poly_overlap_process, "bvxmScenePolyOverlapProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bvxm_dem_to_xyz_process, "bvxmDemToXYZProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bvxm_dem_to_xyz_process2, "bvxmDemToXYZProcess2");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bvxm_heightmap_exp_process, "bvxmHeightmapExpectedProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bvxm_uncertainty_process, "bvxmUncertaintyProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bvxm_create_land_map_process, "bvxmCreateLandMapProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bvxm_create_scene_xml_process, "bvxmCreateSceneXmlProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bvxm_create_scene_xml_large_scale_process, "bvxmCreateSceneXmlLargeScaleProcess");
}
