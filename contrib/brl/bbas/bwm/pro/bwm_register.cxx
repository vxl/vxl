#include "bwm_register.h"

#include <bprb/bprb_macros.h>
#include <bprb/bprb_batch_process_manager.h>
#include <bprb/bprb_func_process.h>
#include "bwm_processes.h"


void bwm_register::register_datatype()
{

}

void bwm_register::register_process()
{
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bwm_create_corr_file_process, "bwmCreateCorrFileProcess");
}
