#include "boxm2_register.h"

#include <bprb/bprb_macros.h>
#include <bprb/bprb_batch_process_manager.h>
#include <bprb/bprb_func_process.h>

#include <boxm2/boxm2_scene.h>
#include <boxm2/io/boxm2_cache.h>
#include <boxm2/io/boxm2_cache1.h>
#include <boxm2/io/boxm2_stream_cache.h>

#include <imesh/imesh_mesh.h>
#include <vil/vil_config.h> // for HAS_GEOTIFF definition

#include "boxm2_processes.h"

void boxm2_register::register_datatype()
{
  REGISTER_DATATYPE( boxm2_scene_sptr );
  REGISTER_DATATYPE( boxm2_cache_sptr );
  REGISTER_DATATYPE( boxm2_cache1_sptr );
  REGISTER_DATATYPE( imesh_mesh_sptr );
  REGISTER_DATATYPE( boxm2_stream_cache_sptr );
}

void boxm2_register::register_process()
{
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm2_load_scene_process,      "boxm2LoadSceneProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm2_modify_scene_appearance_process,      "boxm2ModifySceneAppearanceProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm2_create_cache1_process,    "boxm2CreateCache1Process");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm2_create_cache_process,    "boxm2CreateCacheProcess");

  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm2_create_scene_process,    "boxm2CreateSceneProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm2_write_scene_xml_process, "boxm2WriteSceneXMLProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm2_add_block_process,       "boxm2AddBlockProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm2_export_textured_mesh_process,"boxm2ExportTexturedMeshProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm2_export_mesh_process,"boxm2ExportMeshProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm2_texture_mesh_process,"boxm2TextureMeshProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm2_describe_scene_process,"boxm2DescribeSceneProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm2_create_stream_cache_process, "boxm2CreateStreamCacheProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm2_roi_init_process, "boxm2RoiInitProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm2_scene_illumination_info_process, "boxm2SceneIlluminationInfoProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm2_persp_cam_from_scene_process, "boxm2PerspCamFromSceneProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm2_ortho_geo_cam_from_scene_process, "boxm2OrthoGeoCamFromSceneProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm2_write_cache_process, "boxm2WriteCacheProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm2_stream_cache_close_files_process, "boxm2StreamCacheCloseFilesProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm2_compute_sun_affine_camera_process, "boxm2ComputeSunAffineCameraProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm2_mask_sift_features_process, "boxm2MaskSiftFeaturesProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm2_bundle_to_scene_process, "boxm2BundleToSceneProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm2_clear_cache_process, "boxm2ClearCacheProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm2_blob_change_detection_process, "boxm2BlobChangeDetectionProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm2_blob_precision_recall_process, "boxm2BlobPrecisionRecallProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm2_scene_bbox_process, "boxm2SceneBboxProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm2_transform_model_process, "boxm2TransformModelProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm2_create_scene_mask_process, "boxm2CreateSceneMaskProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm2_paint_mesh_process, "boxm2PaintMeshProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm2_dem_to_xyz_process, "boxm2DemToXYZProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm2_combine_expected_images_process, "boxm2CombineExpectedImagesProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm2_scene_from_box_cams_process, "boxm2SceneFromBoxCamsProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm2_scene_from_nvm_txt_process, "boxm2SceneFromNVMtxtProcess");

  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm2_load_mesh_process, "boxm2LoadMeshProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm2_compactify_mog6_view_process, "boxm2CompactifyMog6ViewProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm2_scene_statistics_process, "boxm2SceneStatisticsProcess");


#if HAS_GEOTIFF
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm2_roi_init_geotiff_process, "boxm2RoiInitGeotiffProcess");
#endif
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm2_scene_lvcs_process, "boxm2SceneLVCSProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm2_create_scene_and_blocks_process, "boxm2CreateSceneAndBlocksProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm2_create_poly_scene_and_blocks_process, "boxm2CreatePolySceneAndBlocksProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm2_distribute_scene_blocks_process, "boxm2DistributeSceneBlocksProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm2_scene_kml_process, "boxm2SceneKmlProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm2_scene_vrml_process, "boxm2SceneVrmlProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm2_ingest_convex_mesh_process, "boxm2IngestConvexMeshProcess");
  REG_PROCESS_FUNC_CONS2(boxm2_add_aux_info_to_ply_process);
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm2_lidar_to_xyz_process, "boxm2LidarToXYZProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm2_dem_to_xyz_process2, "boxm2DemToXYZProcess2");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm2_initialize_ground_xyz_process, "boxm2InitializeGroundXYZProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm2_label_to_xyz_process, "boxm2LabelImgToXYZProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm2_label_to_xyz_process2, "boxm2LabelImgToXYZProcess2");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm2_prune_scene_blocks_process, "boxm2PruneSceneBlocksProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm2_prune_scene_blocks_by_dem_process, "boxm2PruneSceneBlocksByDemProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm2_create_camera_from_kml_path_process, "boxm2CreateCameraFromKmlPath");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm2_import_point_cloud_process, "boxm2ImportPointCloudProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm2_export_color_point_cloud_process, "boxm2ExportColorPointCloudProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm2_export_error_point_cloud_process, "boxm2ExportErrorPointCloudProcess");

  //REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm2_scene_fetch_alpha_process, "boxm2SceneReadingAlphaProcess");
  //REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm2_create_crop_img_process, "boxm2CreateCropImgProcess");
}
