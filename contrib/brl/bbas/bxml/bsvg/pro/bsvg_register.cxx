#include "bsvg_register.h"
#include "bsvg_processes.h"

#include <bprb/bprb_macros.h>
#include <bprb/bprb_batch_process_manager.h>
#include <bprb/bprb_func_process.h>

#include <bxml/bxml_document.h>

void bsvg_register::register_datatype()
{
  REGISTER_DATATYPE( bxml_document_sptr );
}

void bsvg_register::register_process()
{
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bsvg_plot_roc_process, "bsvgPlotROCProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bsvg_bar_plot_initialize_process, "bsvgBarPlotInitializeProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bsvg_bar_plot_add_process, "bsvgBarPlotAddProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bsvg_plot_write_process, "bsvgPlotWriteProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bsvg_plot_roc_process2, "bsvgPlotROCProcess2");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bsvg_plot_initialize_process, "bsvgPlotInitializeProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bsvg_roc_plot_add_process, "bsvgROCPlotAddProcess");
}
