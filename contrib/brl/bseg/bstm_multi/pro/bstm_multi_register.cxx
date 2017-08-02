#include "bstm_multi_register.h"

#include "bstm_multi_processes.h"

#include <bprb/bprb_batch_process_manager.h>
#include <bprb/bprb_func_process.h>
#include <bprb/bprb_macros.h>

#include <bstm_multi/bstm_multi_typedefs.h>

void bstm_multi_register::register_datatype() {
  REGISTER_DATATYPE(bstm_multi_cache_sptr);
  REGISTER_DATATYPE(bstm_multi_scene_sptr);
}

void bstm_multi_register::register_process() {

  // utilities
  REG_PROCESS_FUNC_CONS(bprb_func_process,
                        bprb_batch_process_manager,
                        bstm_multi_clear_cache_process,
                        "bstmClearCacheProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process,
                        bprb_batch_process_manager,
                        bstm_multi_create_scene_process,
                        "bstmCreateSceneProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process,
                        bprb_batch_process_manager,
                        bstm_multi_write_scene_xml_process,
                        "bstmWriteSceneXMLProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process,
                        bprb_batch_process_manager,
                        bstm_multi_load_scene_process,
                        "bstmLoadSceneProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process,
                        bprb_batch_process_manager,
                        bstm_multi_create_cache_process,
                        "bstmCreateCacheProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process,
                        bprb_batch_process_manager,
                        bstm_multi_add_block_process,
                        "bstmAddBlockProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process,
                        bprb_batch_process_manager,
                        bstm_multi_write_cache_process,
                        "bstmWriteCacheProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process,
                        bprb_batch_process_manager,
                        bstm_multi_scene_statistics_process,
                        "bstmSceneStatisticsProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process,
                        bprb_batch_process_manager,
                        bstm_multi_bundle_to_scene_process,
                        "bstmBundleToSceneProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process,
                        bprb_batch_process_manager,
                        bstm_multi_boxm2_scene_to_bstm_multi_process,
                        "bstmBoxm2SceneToBstmProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process,
                        bprb_batch_process_manager,
                        bstm_multi_describe_scene_process,
                        "bstmDescribeSceneProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process,
                        bprb_batch_process_manager,
                        bstm_multi_scene_bbox_process,
                        "bstmSceneBboxProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process,
                        bprb_batch_process_manager,
                        bstm_multi_scene_lvcs_process,
                        "bstmSceneLVCSProcess");
}
