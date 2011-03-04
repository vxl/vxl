#include "bvpl_octree_register.h"

#include <bprb/bprb_macros.h>
#include <bprb/bprb_batch_process_manager.h>
#include <bprb/bprb_func_process.h>

#include "bvpl_octree_processes.h"
#include <bvpl/bvpl_octree/bvpl_taylor_scenes_map.h>
#include <bvpl/bvpl_octree/bvpl_pca_error_scenes.h>
#include <bvpl/bvpl_octree/bvpl_discover_pca_kernels.h>

void bvpl_octree_register::register_datatype()
{
  REGISTER_DATATYPE(bvpl_taylor_scenes_map_sptr);
  REGISTER_DATATYPE(bvpl_pca_error_scenes_sptr);
  REGISTER_DATATYPE(bvpl_discover_pca_kernels_sptr);
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
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bvpl_discover_pca_features_process, "bvplDiscoverPCAFeaturesProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bvpl_compute_pca_test_error_process, "bvplComputeTestErrorProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bvpl_load_taylor_scenes_process, "bvplLoadTaylorScenesProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bvpl_compute_taylor_error_process, "bvplComputeTaylorErrorProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bvpl_add_taylor_errors_process, "bvplAddTaylorErrorsProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bvpl_scene_histogram_process, "bvplSceneHistorgramProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bvpl_compute_pca_error_scene_process, "bvplComputePCAErrorSceneProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bvpl_compute_pca_error_block_process, "bvplComputePCAErrorBlockProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bvpl_load_pca_error_scenes_process, "bvplLoadPCAErrorSceneProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bvpl_block_avg_value_process, "bvplBlockAvgValueProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bvpl_add_pca_errors_process, "bvplAddPCAErrorsProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bvpl_load_pca_info_process, "bvplLoadPCAInfoProcess");
}
