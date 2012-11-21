#include "bstm_cpp_register.h"

#include <bprb/bprb_macros.h>
#include <bprb/bprb_batch_process_manager.h>
#include <bprb/bprb_func_process.h>

#include "bstm_cpp_processes.h"

#include <bstm/bstm_scene.h>
#include <bstm/io/bstm_cache.h>

void bstm_cpp_register::register_datatype()
{
  REGISTER_DATATYPE( bstm_cache_sptr );
  REGISTER_DATATYPE( bstm_scene_sptr );
}

void bstm_cpp_register::register_process()
{
  // utilities
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bstm_cpp_ingest_boxm2_scene_process, "bstmCppIngestBoxm2SceneProcess");


}
