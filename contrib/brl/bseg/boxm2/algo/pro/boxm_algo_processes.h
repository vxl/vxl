#ifndef boxm2_algo_processes_h_
#define boxm2_algo_processes_h_

#include <bprb/bprb_func_process.h>
#include <bprb/bprb_macros.h>

//the init functions
DECLARE_FUNC_CONS(boxm_describe_scene_process);
DECLARE_FUNC_CONS(boxm_init_scene_process);
DECLARE_FUNC_CONS(boxm_line_backproject_process);
DECLARE_FUNC_CONS(boxm_refine_scene_process);
DECLARE_FUNC_CONS(boxm_replace_const_app_process);
DECLARE_FUNC_CONS(boxm_roi_init_process);
DECLARE_FUNC_CONS(boxm_roi_init_rational_camera_process);
DECLARE_FUNC_CONS(boxm_save_occupancy_raw_process);

#endif
