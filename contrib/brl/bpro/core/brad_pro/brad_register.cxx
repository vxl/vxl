#include "brad_register.h"

#include <bprb/bprb_macros.h>
#include <bprb/bprb_batch_process_manager.h>
#include <bprb/bprb_func_process.h>

#include <brad/brad_eigenspace.h>
#include <brad/brad_image_metadata.h>
#include <brad/brad_atmospheric_parameters.h>

#include "brad_processes.h"

void brad_register::register_datatype()
{
  REGISTER_DATATYPE( brad_eigenspace_sptr );
  REGISTER_DATATYPE( brad_image_metadata_sptr );
  REGISTER_DATATYPE( brad_atmospheric_parameters_sptr );
}

void brad_register::register_process()
{
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, brad_display_sun_index_process, "bradDisplaySunIndexProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, brad_sun_dir_bin_process, "bradSunDirBinProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager,brad_save_sun_index_process , "bradSaveSunIndexProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager,brad_create_eigenspace_process , "bradCreateEigenspaceProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager,brad_save_eigenspace_process , "bradSaveEigenspaceProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager,brad_load_eigenspace_process , "bradLoadEigenspaceProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager,brad_describe_eigenspace_process , "bradDescribeEigenspaceProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager,brad_compute_eigenspace_process , "bradComputeEigenspaceProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager,brad_train_histograms_process , "bradTrainHistogramsProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager,brad_update_joint_hist_3d_process , "bradUpdateJointHist3dProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager,brad_classify_image_process , "bradClassifyImageProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager,brad_classify_image_pixel_process, "bradClassifyImagePixelProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, brad_estimate_phongs_model_process, "bradEstimatePhongsModelProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, brad_estimate_shadow_model_process, "bradEstimateShadowModelProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, brad_eigenimage_pixel_process, "bradEigenimagePixelProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, brad_estimate_synoptic_function_1d_process, "bradEstimateSynopticFunction1dProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, brad_estimate_empty_process, "bradEstimateEmptyProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, brad_nitf_abs_radiometric_calibration_process, "bradNITFAbsRadiometricCalibrationProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, brad_nitf_read_metadata_process, "bradNITFReadMetadataProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, brad_estimate_atmospheric_parameters_process, "bradEstimateAtmosphericParametersProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, brad_estimate_reflectance_process, "bradEstimateReflectanceProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, brad_convert_reflectance_to_digital_count_process, "bradConvertReflectanceToDigitalCountProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, brad_save_image_metadata_process, "bradSaveImageMetadataProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, brad_save_atmospheric_parameters_process, "bradSaveAtmosphericParametersProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, brad_load_image_metadata_process, "bradLoadImageMetadataProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, brad_load_atmospheric_parameters_process, "bradLoadAtmosphericParametersProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, brad_get_sun_angles_process, "bradGetSunAnglesProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, brad_set_sun_angles_process, "bradSetSunAnglesProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, brad_get_sun_angles_date_time_process, "bradGetSunAnglesDateTimeProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, brad_create_image_metadata_process, "bradCreateImageMetadataProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, brad_create_atmospheric_parameters_process, "bradCreateAtmosphericParametersProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, brad_estimate_shadows_process, "bradEstimateShadowsProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, brad_get_meta_data_info_process, "bradGetMetaDataInfoProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, brad_get_cloud_coverage_process, "bradGetCloudCoverageProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, brad_get_image_coverage_process, "bradGetImageCoverageProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, brad_compute_appearance_index_process, "bradComputeAppearanceIndexProcess");
}
