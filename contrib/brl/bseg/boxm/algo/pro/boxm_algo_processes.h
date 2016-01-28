#ifndef boxm_algo_processes_h_
#define boxm_algo_processes_h_

#include <bprb/bprb_func_process.h>
#include <bprb/bprb_macros.h>

// the init functions
DECLARE_FUNC_CONS(boxm_describe_scene_process);
DECLARE_FUNC_CONS(boxm_init_scene_process);
DECLARE_FUNC_CONS(boxm_line_backproject_process);
DECLARE_FUNC_CONS(boxm_refine_scene_process);
DECLARE_FUNC_CONS(boxm_replace_const_app_process);
DECLARE_FUNC_CONS(boxm_roi_init_process);
DECLARE_FUNC_CONS(boxm_roi_init_rational_camera_process);
DECLARE_FUNC_CONS(boxm_save_occupancy_raw_process);
DECLARE_FUNC_CONS(boxm_save_scene_raw_process);
DECLARE_FUNC_CONS(boxm_render_expected_edge_vrml_process);
DECLARE_FUNC_CONS(boxm_merge_mixtures_process);
DECLARE_FUNC_CONS(boxm_split_scene_process);
DECLARE_FUNC_CONS(boxm_compute_scene_difference_process);
DECLARE_FUNC_CONS(boxm_compute_entropy_process);
DECLARE_FUNC_CONS(boxm_compute_expected_color_scene_process);
DECLARE_FUNC_CONS(boxm_linear_combination_process);
DECLARE_FUNC_CONS(boxm_explore_cells_at_level_process);
DECLARE_FUNC_CONS(boxm_fill_internal_cells_process);
DECLARE_FUNC_CONS(boxm_remove_level0_process);
DECLARE_FUNC_CONS(boxm_create_scene_from_ply_process);
#endif
