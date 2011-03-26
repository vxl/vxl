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
DECLARE_FUNC_CONS(boxm2_save_data_process);
DECLARE_FUNC_CONS(boxm2_export_textured_mesh_process);
#endif
