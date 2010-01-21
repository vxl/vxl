#include "brip_register.h"
#include "brip_processes.h"
#include "processes/brip_detect_edges_process.h"

#include <bprb/bprb_macros.h>
#include <bprb/bprb_func_process.h>
#include <bprb/bprb_batch_process_manager.h>

void brip_register::register_datatype()
{
}

void brip_register::register_process()
{
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, brip_extrema_process, "bripExtremaProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, brip_detect_edges_process, "bripDetectEdgesProcess");
}
