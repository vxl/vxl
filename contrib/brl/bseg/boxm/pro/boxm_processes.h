#ifndef boxm_processes_h_
#define boxm_processes_h_

#include <bprb/bprb_func_process.h>
#include <bprb/bprb_macros.h>

//the init functions
DECLARE_FUNC_CONS(boxm_load_scene_process);
DECLARE_FUNC_CONS(boxm_create_scene_process);
DECLARE_FUNC_CONS(boxm_construct_scene_from_image_process);
DECLARE_FUNC_CONS(boxm_proj_local_cam_process);
DECLARE_FUNC_CONS(boxm_camera_viewing_scene_process);
DECLARE_FUNC_CONS(boxm_upload_mesh_process);
DECLARE_FUNC_CONS(boxm_clean_scene_process);
DECLARE_FUNC_CONS(boxm_force_write_blocks_process);
#endif
