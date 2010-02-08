#ifndef boxm_algo_processes_h_
#define boxm_algo_processes_h_

#include <bprb/bprb_func_process.h>
#include <bprb/bprb_macros.h>

//the init functions
DECLARE_FUNC_CONS(boxm_split_scene_process);
DECLARE_FUNC_CONS(boxm_merge_mixtures_process);
DECLARE_FUNC_CONS(boxm_save_scene_vrml_process);
DECLARE_FUNC_CONS(boxm_crop_scene_process);
DECLARE_FUNC_CONS(boxm_scene_to_bvxm_grid_process);
DECLARE_FUNC_CONS(boxm_fill_internal_cells_process);

#endif
