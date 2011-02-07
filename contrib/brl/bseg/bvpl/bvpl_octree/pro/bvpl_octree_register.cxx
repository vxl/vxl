#include "bvpl_octree_register.h"

#include <bprb/bprb_macros.h>
#include <bprb/bprb_batch_process_manager.h>
#include <bprb/bprb_func_process.h>
#include "bvpl_octree_processes.h"


void bvpl_octree_register::register_datatype()
{
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
}
