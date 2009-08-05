#include "boxm_opt_register.h"

#include <bprb/bprb_macros.h>
#include <bprb/bprb_batch_process_manager.h>
#include <bprb/bprb_func_process.h>

#include "boxm_opt_processes.h"

void boxm_opt_register::register_datatype()
{
  //REGISTER_DATATYPE( boxm_scene_base_sptr );
}

void boxm_opt_register::register_process()
{
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm_opt_bayesian_update_process,"boxmOptBayesianUpdateProcess");
}
