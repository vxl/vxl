#include "brad_register.h"
#include "brad_processes.h"


#include <bprb/bprb_macros.h>
#include <bprb/bprb_func_process.h>
#include <bprb/bprb_batch_process_manager.h>

void brad_register::register_datatype()
{
}

void brad_register::register_process()
{
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, brad_estimate_phongs_model_process, "bradEstimatePhongsModelProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, brad_estimate_shadow_model_process, "bradEstimateShadowModelProcess");

  
}
