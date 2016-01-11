#include "boxm2_multi_register.h"

#include <bprb/bprb_macros.h>
#include <bprb/bprb_batch_process_manager.h>
#include <bprb/bprb_func_process.h>


#include <boxm2_multi/boxm2_multi_cache.h>


#include "boxm2_multi_processes.h"

void boxm2_multi_register::register_datatype()
{
  REGISTER_DATATYPE( boxm2_multi_cache_sptr );
}

void boxm2_multi_register::register_process()
{
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm2_create_multi_cache_process,      "boxm2CreateMultiCacheProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm2_multi_update_process,      "boxm2MultiUpdateProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm2_multi_render_process,      "boxm2MultiRenderProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm2_write_multi_cache_process,      "boxm2WriteMultiCacheProcess");

}
