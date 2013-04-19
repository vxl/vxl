#include "bstm_register.h"

#include <bprb/bprb_macros.h>
#include <bprb/bprb_batch_process_manager.h>
#include <bprb/bprb_func_process.h>

#include "bstm_processes.h"

#include <bstm/bstm_scene.h>
#include <bstm/io/bstm_cache.h>

void bstm_register::register_datatype()
{
  REGISTER_DATATYPE( bstm_cache_sptr );
  REGISTER_DATATYPE( bstm_scene_sptr );
}

void bstm_register::register_process()
{

  // utilities
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bstm_clear_cache_process, "bstmClearCacheProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bstm_create_scene_process, "bstmCreateSceneProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bstm_write_scene_xml_process, "bstmWriteSceneXMLProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bstm_load_scene_process, "bstmLoadSceneProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bstm_create_cache_process, "bstmCreateCacheProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bstm_add_block_process, "bstmAddBlockProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bstm_write_cache_process, "bstmWriteCacheProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bstm_scene_statistics_process, "bstmSceneStatisticsProcess");

}
