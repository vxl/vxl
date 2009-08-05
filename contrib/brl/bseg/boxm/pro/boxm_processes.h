#ifndef boxm_processes_h_
#define boxm_processes_h_

#include <bprb/bprb_func_process.h>
#include <bprb/bprb_macros.h>

//the init functions
DECLARE_FUNC_CONS(boxm_load_scene_process);
DECLARE_FUNC_CONS(boxm_render_expected_process);
DECLARE_FUNC_CONS(boxm_save_occupancy_raw_process);
DECLARE_FUNC_CONS(boxm_create_scene_process);
DECLARE_FUNC_CONS(boxm_update_process);
DECLARE_FUNC_CONS(boxm_refine_scene_process);
DECLARE_FUNC_CONS(boxm_roi_init_process);
DECLARE_FUNC_CONS(boxm_volume_visibility_process);
DECLARE_FUNC_CONS(boxm_proj_local_cam_process);
DECLARE_FUNC_CONS(boxm_upload_mesh_process);
DECLARE_FUNC_CONS(boxm_replace_const_app_process);
DECLARE_FUNC_CONS(boxm_opt_bayesian_update_process);

#endif
