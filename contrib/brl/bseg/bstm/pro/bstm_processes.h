#ifndef bstm_processes_h_
#define bstm_processes_h_

#include <bprb/bprb_func_process.h>
#include <bprb/bprb_macros.h>

//the init functions
DECLARE_FUNC_CONS(bstm_write_scene_xml_process);
DECLARE_FUNC_CONS(bstm_create_scene_process);
DECLARE_FUNC_CONS(bstm_load_scene_process);
DECLARE_FUNC_CONS(bstm_create_cache_process);
DECLARE_FUNC_CONS(bstm_clear_cache_process);
DECLARE_FUNC_CONS(bstm_write_cache_process);
DECLARE_FUNC_CONS(bstm_add_block_process);
DECLARE_FUNC_CONS(bstm_scene_statistics_process);
DECLARE_FUNC_CONS(bstm_bundle_to_scene_process);
DECLARE_FUNC_CONS(bstm_boxm2_scene_to_bstm_process);
DECLARE_FUNC_CONS(bstm_describe_scene_process);
DECLARE_FUNC_CONS(bstm_scene_bbox_process);
DECLARE_FUNC_CONS(bstm_scene_lvcs_process);

#endif
