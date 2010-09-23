#include "vil_register.h"
#include "vil_processes.h"

#include <bprb/bprb_macros.h>
#include <bprb/bprb_func_process.h>
#include <bprb/bprb_batch_process_manager.h>

#include <vil/vil_image_view_base.h>

void vil_register::register_datatype()
{
  REGISTER_DATATYPE(vil_image_view_base_sptr);
}

void vil_register::register_process()
{
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vil_binary_image_op_process, "vilBinaryImageOpProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vil_combine_grey_images_process, "vilCombineGreyImagesProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vil_convert_to_n_planes_process, "vilConvertToNPlanesProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vil_crop_image_process, "vilCropImageProcess");
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

  
}

