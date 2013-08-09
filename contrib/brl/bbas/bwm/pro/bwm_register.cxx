#include "bwm_register.h"

#include <bprb/bprb_macros.h>
#include <bprb/bprb_batch_process_manager.h>
#include <bprb/bprb_func_process.h>
#include "bwm_processes.h"
#include <bwm/algo/bwm_satellite_resources_sptr.h>


void bwm_register::register_datatype()
{
  REGISTER_DATATYPE(bwm_satellite_resources_sptr);
}

void bwm_register::register_process()
{
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bwm_create_corr_file_process, "bwmCreateCorrFileProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bwm_create_satellite_resources_process, "bwmCreateSatResourcesProcess");
}
