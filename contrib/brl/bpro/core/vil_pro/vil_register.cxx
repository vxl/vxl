#include "vil_register.h"
#include "vil_processes.h"

#include <bprb/bprb_macros.h>
#include <bprb/bprb_func_process.h>
#include <bprb/bprb_batch_process_manager.h>

#include <vil/vil_image_view_base.h>
#include <bil/bil_raw_image_istream.h>
#include <bil/bil_arf_image_istream.h>
#include <vil/vil_image_resource.h>

void vil_register::register_datatype()
{
  REGISTER_DATATYPE(vil_image_view_base_sptr);
  REGISTER_DATATYPE(bil_raw_image_istream_sptr);
  REGISTER_DATATYPE(bil_arf_image_istream_sptr);
  REGISTER_DATATYPE(vil_image_resource_sptr);
}

void vil_register::register_process()
{
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vil_binary_image_op_process, "vilBinaryImageOpProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vil_combine_grey_images_process, "vilCombineGreyImagesProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vil_convert_to_n_planes_process, "vilConvertToNPlanesProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vil_crop_image_process, "vilCropImageProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vil_crop_image_res_process, "vilCropImageResProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vil_gaussian_process, "vilGaussianProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vil_gradient_process, "vilGradientProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vil_image_pair_process, "vilImagePairProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vil_load_from_ascii_process, "vilLoadFromAsciiProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vil_load_image_view_binary_process, "vilLoadImageViewBinaryProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vil_load_image_view_process, "vilLoadImageViewProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vil_map_image_binary_process, "vilMapImageBinaryProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vil_map_image_process, "vilMapImageProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vil_rgbi_to_grey_process, "vilRGBIToGreyProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vil_save_image_view_binary_process, "vilSaveImageViewBinaryProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vil_save_image_view_process, "vilSaveImageViewProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vil_stretch_image_process, "vilStretchImageProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vil_threshold_image_process, "vilThresholdImageProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vil_image_size_process, "vilImageSizeProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vil_two_planes_composite_process, "vilTwoPlanesCompositeProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bil_compass_edge_detector_process, "bilCompassEdgeDetectorProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vil_resample_process, "vilResampleProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vil_filter_image_process, "vilImageFilterProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vil_blob_detection_process, "vilBlobDetectionProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vil_pixelwise_roc_process, "vilPixelwiseRocProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vil_pixelwise_roc_process2, "vilPixelwiseRocProcess2");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vil_rgb_to_grey_process, "vilRGBToGreyProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vil_nitf_date_time_process, "vilNITFDateTimeProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vil_nitf_remove_margin_process, "vilNITFRemoveMarginProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vil_convert_pixel_type_process, "vilConvertPixelTypeProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vil_median_filter_process, "vilMedianFilterProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vil_gradient_angle_process, "vilGradientAngleProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vil_image_ssd_process, "vilImageSSDProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vil_image_mean_process, "vilImageMeanProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vil_image_range_process, "vilImageRangeProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vil_init_float_image_process, "vilInitFloatImageProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vil_init_byte_image_process, "vilInitByteImageProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bil_create_raw_image_istream_process, "bilCreateRawImageIstreamProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bil_read_frame_process, "bilReadFrameProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bil_seek_frame_process, "bilSeekFrameProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vil_pixel_value_process, "vilPixelValueProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vil_scale_and_offset_values_process, "vilScaleAndOffsetValuesProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vil_set_float_image_pixel_process, "vilSetFloatImagePixelProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vil_image_sum_process, "vilImageSumProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vil_image_dilate_disk_process, "vilImageDilateDiskProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vil_shadow_detection_process, "vilShadowDetectionProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vil_shadow_ridge_detection_process, "vilShadowRidgeDetectionProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vil_image_mean_and_variance_process, "vilImageMeanAndVarianceProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vil_mean_and_variance_image_process, "vilMeanAndVarianceImageProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vil_image_normalise_process, "vilImageNormaliseProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vil_debayer_BGGR_to_RGB_process, "vilDebayerBGGRToRGBProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bil_create_arf_image_istream_process, "bilCreateArfImageIstreamProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bil_arf_read_frame_process, "bilArfReadFrameProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bil_arf_seek_frame_process, "bilArfSeekFrameProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bil_read_CLIF07_data_process, "bilReadCLIF07DataProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vil_fill_holes_in_regions_process, "vilFillHolesInRegionsProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vil_EO_IR_combine_process, "vilEOIRCombineProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vil_truncate_image_process, "vilTruncateImageProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vil_grey_to_rgb_process, "vilGreyToRGBProcess");

  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vil_threshold_max_image_process, "vilThresholdMaxImageProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vil_threshold_image_region_process, "vilThresholdImageInsideProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vil_mask_image_using_ids_process, "vilMaskImageUsingIDsProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vil_get_plane_process, "vilGetPlaneProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vil_combine_planes_process, "vilCombinePlanesProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vil_block_entropy_process, "vilBlockEntropyProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vil_load_image_resource_process, "vilLoadImageResourceProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vil_save_image_resource_process, "vilSaveImageResourceProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vil_get_number_of_planes_process, "vilGetNumberOfPlanesProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vil_edge_detection_process, "vilEdgeDetectionProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vil_binary_edge_detection_process, "vilBinaryEdgeDetectionProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vil_BGR_to_RGB_process, "vilBGRToRGBProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vil_combine_planes_process2, "vilCombinePlanesProcess2");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vil_histogram_equalize_process, "vilHistogramEqualizeProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vil_invert_float_image_process, "vilInvertFloatImageProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vil_combine_palnes_8_bands_process, "vilCombinePlanes8BandsProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vil_image_registration_process, "vilImageRegistrationProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vil_multi_plane_view_to_grey_process, "vilMultiPlaneViewToGreyProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vil_binarize_otsu_process, "vilBinarizeOtsuProcess");
}
