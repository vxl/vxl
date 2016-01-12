#include "bvpl_octree_register.h"

#include <bprb/bprb_macros.h>
#include <bprb/bprb_batch_process_manager.h>
#include <bprb/bprb_func_process.h>

#include "bvpl_octree_processes.h"
#include <bvpl_octree/bvpl_taylor_scenes_map.h>
#include <bvpl_octree/bvpl_pca_error_scenes.h>
#include <bvpl_octree/bvpl_discover_pca_kernels.h>
#include <bvpl_octree/bvpl_global_pca.h>
#include <bvpl_octree/bvpl_global_taylor.h>
#include <bvpl_octree/bvpl_global_corners.h>

void bvpl_octree_register::register_datatype()
{
  typedef vbl_smart_ptr<bvpl_global_taylor<double, 10> > bvpl_global_taylor_sptr;

  REGISTER_DATATYPE(bvpl_taylor_scenes_map_sptr);
  REGISTER_DATATYPE(bvpl_pca_error_scenes_sptr);
  REGISTER_DATATYPE(bvpl_discover_pca_kernels_sptr);
  REGISTER_DATATYPE(bvpl_global_pca_125_sptr);
  REGISTER_DATATYPE(bvpl_global_taylor_sptr);
  REGISTER_DATATYPE(bvpl_global_corners_sptr);

}

void bvpl_octree_register::register_process()
{
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bvpl_scene_vector_operator_process, "bvplSceneVectorOperatorProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bvpl_scene_kernel_operator_process, "bvplSceneKernelOperatorProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bvpl_block_kernel_operator_process, "bvplBlockKernelOperatorProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bvpl_save_vrml_process, "bvplSaveVrmlProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bvpl_create_scene_process, "bvplCreateSceneProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bvpl_plane_propagate_process, "bvplPlanePropagateProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bvpl_nonmax_supp_process, "bvplNonmaxSuppProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bvpl_compute_gauss_gradients, "bvplComputeGaussGradients");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bvpl_compute_pca_test_error_process, "bvplComputeTestErrorProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bvpl_scene_histogram_process, "bvplSceneHistorgramProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bvpl_grad_scene_to_bin_process, "bvplGradSceneToBinProcess");

  //PCA related
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bvpl_discover_pca_features_process, "bvplDiscoverPCAFeaturesProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bvpl_compute_pca_error_scene_process, "bvplComputePCAErrorSceneProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bvpl_compute_pca_error_block_process, "bvplComputePCAErrorBlockProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bvpl_load_pca_error_scenes_process, "bvplLoadPCAErrorSceneProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bvpl_block_avg_value_process, "bvplBlockAvgValueProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bvpl_add_pca_errors_process, "bvplAddPCAErrorsProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bvpl_load_pca_info_process, "bvplLoadPCAInfoProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bvpl_pca_project_process, "bvplPCAProjectProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bvpl_normalize_pca_training_error_process, "bvplNormalizePCATrainingErrorProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bvpl_pca_global_statistics_process, "bvplPCAGlobalStatisticsProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bvpl_combine_pairwise_statistics_process, "bvplCombinePairwiseStatisticsProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bvpl_global_pca_process, "bvplGlobalPCAProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bvpl_init_global_pca_process, "bvplInitGlobalPCAProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bvpl_load_global_pca_125_process, "bvplLoadGlobalPCA125Process");
  REG_PROCESS_FUNC_CONS2(bvpl_pca_global_proj_error_process);
  REG_PROCESS_FUNC_CONS2(bvpl_pca_global_add_error_process);

  //Taylor related
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bvpl_load_taylor_scenes_process, "bvplLoadTaylorScenesProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bvpl_compute_taylor_error_process, "bvplComputeTaylorErrorProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bvpl_add_taylor_errors_process, "bvplAddTaylorErrorsProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bvpl_init_global_taylor_process, "bvplInitGlobalTaylorProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bvpl_compute_taylor_coefficients_process, "bvplComputeTaylorCoefficients");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bvpl_explore_coefficient_scene_process, "bvplExploreCoefficientSceneProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bvpl_load_global_taylor_process, "bvplLoadGlobalTaylorProcess");
  REG_PROCESS_FUNC_CONS2(bvpl_taylor_global_proj_error_process);
  REG_PROCESS_FUNC_CONS2(bvpl_taylor_global_add_error_process);

  //Corner related
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bvpl_threshold_corners_process, "bvplThresholdCornersProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bvpl_compute_harris_measure_process, "bvplComputeHarrisMeasureProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bvpl_load_global_corners_process, "bvplLoadGlobalCornersProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bvpl_corner_statistics_process, "bvplCornerStatisticsProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bvpl_compute_beaudet_measure_process, "bvplComputeBeaudetMeasureProcess");

  //Steerable filters
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bvpl_init_sf_response_scene_process, "bvplInitSFResponseSceneProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bvpl_compute_sf_raw_response_process, "bvplComputeSFRawResponseProcess");
}
