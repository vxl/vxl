#ifndef boxm2_processes_h_
#define boxm2_processes_h_

#include <bprb/bprb_func_process.h>
#include <bprb/bprb_macros.h>

//the init functions
DECLARE_FUNC_CONS(boxm2_load_scene_process);
DECLARE_FUNC_CONS(boxm2_create_cache_process);
DECLARE_FUNC_CONS(boxm2_create_scene_process);
DECLARE_FUNC_CONS(boxm2_write_scene_xml_process);
DECLARE_FUNC_CONS(boxm2_add_block_process);
DECLARE_FUNC_CONS(boxm2_export_textured_mesh_process);
DECLARE_FUNC_CONS(boxm2_export_mesh_process);
DECLARE_FUNC_CONS(boxm2_texture_mesh_process);
DECLARE_FUNC_CONS(boxm2_describe_scene_process);
DECLARE_FUNC_CONS(boxm2_create_stream_cache_process);
DECLARE_FUNC_CONS(boxm2_roi_init_process);
DECLARE_FUNC_CONS(boxm2_scene_illumination_info_process);
DECLARE_FUNC_CONS(boxm2_persp_cam_from_scene_process);
DECLARE_FUNC_CONS(boxm2_write_cache_process);
DECLARE_FUNC_CONS(boxm2_stream_cache_close_files_process);

#endif
