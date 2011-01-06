#ifndef bvpl_octree_processes_h_
#define bvpl_octree_processes_h_

#include <bprb/bprb_func_process.h>
#include <bprb/bprb_macros.h>

DECLARE_FUNC_CONS(bvpl_scene_vector_operator_process);
DECLARE_FUNC_CONS(bvpl_scene_kernel_operator_process);
DECLARE_FUNC_CONS(bvpl_save_vrml_process);
DECLARE_FUNC_CONS(bvpl_create_scene_process);
DECLARE_FUNC_CONS(bvpl_plane_propagate_process);
DECLARE_FUNC_CONS(bvpl_nonmax_supp_process);
DECLARE_FUNC_CONS(bvpl_compute_gauss_gradients);
DECLARE_FUNC_CONS(bvpl_discover_pca_features_process);
#endif
