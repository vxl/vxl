#include "sdet_register.h"
#include "sdet_processes.h"
#include "processes/sdet_detect_edges_process.h"

#include <bprb/bprb_macros.h>
#include <bprb/bprb_func_process.h>
#include <bprb/bprb_batch_process_manager.h>

void sdet_register::register_datatype()
{
}

void sdet_register::register_process()
{
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, sdet_detect_edges_process, "sdetDetectEdgesProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, sdet_detect_edge_tangent_process, "sdetDetectEdgesTangentProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, sdet_detect_edge_tangent_interp_process, "sdetDetectEdgesTangentInterpProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, sdet_write_edge_file_process, "sdetWriteEdgeFileProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, sdet_detect_edge_line_fitted_process, "sdetDetectEdgesLineFittedProcess");
}
