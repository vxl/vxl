#include "vcon_register.h"

#include <bprb/bprb_macros.h>
#include <bprb/bprb_batch_process_manager.h>
#include <bprb/bprb_func_process.h>
#include "vcon_processes.h"

void vcon_register::register_datatype()
{

}

void vcon_register::register_process()
{
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vcon_calculate_trafficability_process, "vconTrafficabilityProcess");

}
