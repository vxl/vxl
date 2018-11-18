#include "ihog_register.h"
#include "ihog_processes.h"

#include <bprb/bprb_macros.h>
#include <bprb/bprb_func_process.h>
#include <bprb/bprb_batch_process_manager.h>

void ihog_register::register_datatype()
{

}

void ihog_register::register_process()
{
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, ihog_register_translational_process, "ihogRegisterTranslationalProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, ihog_compute_mi_cost_surface_process, "ihogComputeMiCostSurfaceProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, ihog_mutual_information_process, "ihogMutualInformationProcess");

}
