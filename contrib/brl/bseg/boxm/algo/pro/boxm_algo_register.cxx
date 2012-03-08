#include "boxm_algo_register.h"

#include <bprb/bprb_macros.h>
#include <bprb/bprb_batch_process_manager.h>
#include <bprb/bprb_func_process.h>

#include "boxm_algo_processes.h"

void boxm_algo_register::register_datatype()
{
}

void boxm_algo_register::register_process()
{
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm_describe_scene_process,"boxmDescribeSceneProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm_init_scene_process,"boxmInitSceneProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm_line_backproject_process,"boxmLineBackprojectProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm_refine_scene_process, "boxmRefineSceneProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm_replace_const_app_process,"boxmReplaceConstAppProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm_roi_init_process, "boxmROIInitProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm_roi_init_rational_camera_process, "boxmROIInitRationalCameraProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm_save_occupancy_raw_process, "boxmSaveOccupancyRawProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm_save_scene_raw_process, "boxmSaveSceneRawProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm_render_expected_edge_vrml_process, "boxmRenderExpectedEdgeVRMLProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm_merge_mixtures_process, "boxmMergeMixturesProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm_split_scene_process ,"boxmSplitSceneProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm_compute_scene_difference_process, "boxmComputeSceneDifferenceProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm_compute_entropy_process, "boxmComputeEntropyProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm_compute_expected_color_scene_process, "boxmComputeExpectedColorSceneProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm_linear_combination_process, "boxmLinearCombinationProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm_explore_cells_at_level_process, "boxmExploreCellsAtLevelProcess");
  REG_PROCESS_FUNC_CONS2(boxm_fill_internal_cells_process);
  REG_PROCESS_FUNC_CONS2(boxm_remove_level0_process);
  REG_PROCESS_FUNC_CONS2(boxm_create_scene_from_ply_process);
}
