#include "vpgl_register.h"
#include "vpgl_processes.h"
#include <vpgl/vpgl_camera_double_sptr.h>
#include <vpgl/vpgl_lvcs_sptr.h>

#include <bprb/bprb_macros.h>
#include <bprb/bprb_func_process.h>
#include <bprb/bprb_batch_process_manager.h>
#include "vpgl_dem_manager.h"
void vpgl_register::register_datatype()
{
  REGISTER_DATATYPE( vpgl_camera_double_sptr );
  REGISTER_DATATYPE( vpgl_lvcs_sptr );
#if HAS_GEOTIFF
  REGISTER_DATATYPE( vpgl_dem_manager_sptr );
#endif
}

void vpgl_register::register_process()
{
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vpgl_load_perspective_camera_process, "vpglLoadPerspectiveCameraProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vpgl_load_proj_camera_process, "vpglLoadProjCameraProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vpgl_load_rational_camera_nitf_process, "vpglLoadRationalCameraNITFProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vpgl_load_rational_camera_process, "vpglLoadRationalCameraProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vpgl_load_local_rational_camera_process, "vpglLoadLocalRationalCameraProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vpgl_nitf_camera_coverage_process, "vpglNITFCameraCoverageProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vpgl_save_perspective_camera_process, "vpglSavePerspectiveCameraProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vpgl_save_rational_camera_process, "vpglSaveRationalCameraProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vpgl_create_local_rational_camera_process, "vpglCreateLocalRationalCameraProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vpgl_convert_to_local_rational_camera_process, "vpglConvertToLocalRationalCameraProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vpgl_create_local_rational_camera_nitf_process, "vpglCreateLocalRationalCameraNITFProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vpgl_projective_to_rational_camera_process, "vpglProjectiveToRationalCameraProcess");
#if 0 //deprecate scale perspective camera
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vpgl_scale_perspective_camera_process, "vpglScalePerspectiveCameraProcess");
#endif
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vpgl_correct_rational_camera_process, "vpglCorrectRationalCameraProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vpgl_create_perspective_camera_process, "vpglCreatePerspectiveCameraProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vpgl_save_perspective_camera_vrml_process, "vpglSavePerspectiveCameraVrmlProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vpgl_save_perspective_cameras_vrml_process, "vpglSavePerspectiveCamerasVrmlProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vpgl_correct_rational_cameras_process, "vpglCorrectRationalCamerasProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vpgl_correct_rational_cameras_mult_corr_process, "vpglCorrectRationalCamerasMultCorrProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vpgl_correct_rational_cameras_mult_corr_refine_process, "vpglCorrectRationalCamerasMultCorrRefineProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vpgl_project_process, "vpglProjectProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vpgl_convert_to_generic_camera_process, "vpglConvertToGenericCameraProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vpgl_convert_local_rational_to_generic_process, "vpglConvertLocalRationalToGenericProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vpgl_resample_perspective_camera_process, "vpglResamplePerspectiveCameraProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vpgl_generate_3d_point_from_depth_process, "vpglGenerate3dPointFromDepthProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vpgl_generate_xyz_from_depth_image_process, "vpglGenerateXYZFromDepthImageProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vpgl_get_view_direction_at_point_process, "vpglGetViewDirectionAtPointProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vpgl_get_rpc_offsets_process, "vpglGetRPCOffsetsProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vpgl_get_bounding_box_process, "vpglGetBoundingBoxProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vpgl_nitf_footprint_process,  "vpglNITFFootprintProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vpgl_nitf_footprint_process2, "vpglNITFFootprintProcess2");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vpgl_generate_3d_point_from_cams_process, "vpglGenerate3dPointFromCamsProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vpgl_convert_to_local_coordinates_process, "vpglConvertToLocalCoordinatesProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vpgl_perturb_persp_cam_orient_process, "vpglPerturbPerspCamOrientProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vpgl_rotate_persp_cam_process, "vpglRotatePerspCamProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vpgl_persp_cam_distance_process, "vpglPerspCamDistanceProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vpgl_convert_to_generic_camera_w_margin_process, "vpglConvertToGenericCameraWithMarginProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vpgl_compute_persp_cam_pa_covariance_process, "vpglComputePerspCamPACovarianceProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vpgl_create_perspective_camera_process2, "vpglCreatePerspectiveCameraProcess2");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vpgl_get_perspective_cam_center_process, "vpglGetPerspectiveCamCenterProcess");
#if HAS_GEOTIFF
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vpgl_load_geo_camera_process, "vpglLoadGeoCameraProcess");
#endif
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vpgl_create_perspective_camera_process3, "vpglCreatePerspectiveCameraProcess3");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vpgl_create_perspective_camera_process4, "vpglCreatePerspectiveCameraProcess4");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vpgl_create_perspective_camera_process5, "vpglCreatePerspectiveCameraProcess5");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vpgl_perturb_uniform_persp_cam_orient_process, "vpglPerturbUniformPerspCamOrientProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bpgl_transform_perspective_cameras_process, "bpglTransformPerspectiveCamerasProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vpgl_transform_space_process, "vpglTransformSpaceProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vpgl_transform_box_process, "vpglTransformBoxProcess");

  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vpgl_load_affine_camera_process, "vpglLoadAffineCameraProcess");

  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vpgl_geo_footprint_process, "vpglGeoFootprintProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vpgl_geo_footprint_process2, "vpglGeoFootprintProcess2");
#if HAS_GEOTIFF
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vpgl_translate_geo_camera_process, "vpglTranslateGeoCameraProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vpgl_convert_geo_camera_to_generic_process, "vpglConvertGeoCameraToGenericProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vpgl_convert_geotiff_to_point_cloud_process, "vpglConvertGeotiffToPointCloudProcess");
#endif
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vpgl_create_perspective_camera_from_kml_process, "vpglCreatePerspCameraFromKMLProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vpgl_convert_to_local_coordinates_process2, "vpglConvertToLocalCoordinatesProcess2");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vpgl_load_perspective_camera_from_kml_file_process, "vpglLoadPerspCameraFromKMLFileProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vpgl_geo_cam_global_to_img_process, "vpglGeoGlobalToImgProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vpgl_geo_cam_img_to_global_process, "vpglGeoImgToGlobalProcess");
#if HAS_GEOTIFF
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vpgl_load_geo_camera_process2, "vpglLoadGeoCameraProcess2");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vpgl_find_connected_component_process, "vpglFindConnectedComponentProcess");
#endif
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vpgl_export_cameras_to_nvm_process, "vpglExportCamerasToNvmProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vpgl_create_lvcs_process, "vpglCreateLVCSProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vpgl_convert_local_to_global_coordinates_process, "vpglConvertLocalToGlobalCoordinatesProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vpgl_convert_local_to_global_coordinates_array_process, "vpglConvertLocalToGlobalCoordinatesArrayProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vpgl_convert_local_rational_to_perspective_process, "vpglConvertLocalRationalToPerspectiveProcess");
#if HAS_GEOTIFF
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vpgl_create_geo_camera_process, "vpglCreateGeoCameraProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vpgl_load_geo_camera_process3, "vpglLoadGeotiffCamFromHeaderProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vpgl_save_geo_camera_tfw_process, "vpglSaveGeoCameraTFWProcess");
#endif
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vpgl_interpolate_perspective_cameras_process, "vpglInterpolatePerspectiveCamerasProcess");

  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vpgl_load_lvcs_process, "vpglLoadLVCSProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vpgl_save_lvcs_process, "vpglSaveLVCSProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vpgl_create_and_save_lvcs_process, "vpglCreateAndSaveLVCSProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vpgl_get_rational_camera_offsets_process, "vpglGetRationalCameraOffsetsProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vpgl_write_generic_camera_process, "vpglWriteGenericCameraProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vpgl_compute_affine_from_rat_process, "vpglComputeAffineFromRationalProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vpgl_affine_rectify_images_process, "vpglAffineRectifyImagesProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vpgl_save_projective_camera_process, "vpglSaveProjectiveCameraProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vpgl_affine_rectify_images_process2, "vpglAffineRectifyImagesProcess2");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vpgl_crop_img_using_3d_box_process, "vpglCropImgUsing3DboxProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vpgl_crop_img_using_3d_box_dem_process, "vpglCropImgUsing3DboxDemProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vpgl_offset_cam_using_3d_box_process, "vpglOffsetCamUsing3DboxProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vpgl_compute_utm_zone_process, "vpglComputeUTMZoneProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vpgl_rational_cam_img_to_global_process, "vpglRationalImgToGlobalProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vpgl_get_backproject_ray_process, "vpglGetBackprojectRayProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vpgl_get_rpc_backproject_ray_process, "vpglGetRpcBackprojectRayProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vpgl_construct_height_map_process, "vpglConstructHeightMapProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vpgl_construct_disparity_map_process, "vpglConstructDisparityMapProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vpgl_rational_cam_nadirness_process, "vpglRationalCamNadirnessProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vpgl_rational_cam_rotation_to_north_process, "vpglRationalCamRotationToNorthProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vpgl_rational_cam_rotation_to_up_vector_process, "vpglRationalCamRotationToUpVectorProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vpgl_compute_image_to_world_homography_process, "vpglComputeImageToWorldHomographyProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vpgl_crop_ortho_using_3d_box_process, "vpglCropOrthoUsing3DboxPRocess");
#if HAS_GEOTIFF
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vpgl_convert_non_nadir_geo_camera_to_generic_process, "vpglConvertNonNadirGeoCameraToGenericProcess");
#endif
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vpgl_get_generic_camera_ray_process, "vpglGetGenericCamRayProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vpgl_persp_cam_from_photo_overlay_process, "vpglPerspCameraFromPhotoOverlayProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vpgl_affine_f_matrix_process, "vpglAffineFMatrixProcess");

  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vpgl_get_generic_camera_ray_process, "vpglGetGenericCamRayProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vpgl_isfm_rational_camera_process, "vpglIsfmRationalCameraProcess");
#if HAS_GEOTIFF
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vpgl_isfm_rational_camera_seed_process, "vpglIsfmRationalCameraSeedProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vpgl_isfm_rational_camera_with_initial_process, "vpglIsfmRationalCameraWithInitialProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vpgl_create_dem_manager_process, "vpglCreateDemManagerProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vpgl_backproject_dem_process, "vpglBackprojectDemProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vpgl_dem_image_projection_process, "vpglDemImageProjectionProcess");
#endif
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vpgl_load_rational_camera_from_txt_process, "vpglLoadRationalCameraFromTXTProcess");

}
