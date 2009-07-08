#include "bsvg_register.h"
#include "bsvg_processes.h"

#include <bprb/bprb_macros.h>
#include <bprb/bprb_batch_process_manager.h>
#include <bprb/bprb_func_process.h>

void bsvg_register::register_datatype()
{
  
}

void bsvg_register::register_process()
{
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bsvg_plot_roc_process, "bsvgPlotROCProcess");
}
