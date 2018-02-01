#include "bsgm_register.h"
#include "bsgm_processes.h"
#include <bprb/bprb_macros.h>
#include <bprb/bprb_func_process.h>
#include <bprb/bprb_batch_process_manager.h>

void bsgm_register::register_datatype() {}

void bsgm_register::register_process()
{
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bsgm_matching_stereo_process, "bsgmMatchingStereoProcess");
}
