#include "brec_register.h"

#include <bprb/bprb_macros.h>
#include <bprb/bprb_batch_process_manager.h>
#include <bprb/bprb_func_process.h>

#include "bmdl_processes.h"

void bmdl_register::register_datatype()
{
}

void bmdl_register::register_process()
{
  REG_PROCESS_FUNC_INIT_FINALIZE(bprb_func_process, bprb_batch_process_manager, bmdl_classify_process,
    "bmdlClassifyProcess", bmdl_classify_process_init, 0);
  REG_PROCESS_FUNC_INIT_FINALIZE(bprb_func_process, bprb_batch_process_manager, bmdl_trace_boundaries_process,
    "bmdlTraceBoundariesProcess", bmdl_trace_boundaries_process_init, 0);
  REG_PROCESS_FUNC_INIT_FINALIZE(bprb_func_process, bprb_batch_process_manager, bmdl_generate_mesh_process,
    "bmdlGenerateMeshProcess", bmdl_generate_mesh_process_init, 0);
  REG_PROCESS_FUNC_INIT_FINALIZE(bprb_func_process, bprb_batch_process_manager, bmdl_lidar_roi_process,
    "bmdlLidarRoiProcess", bmdl_lidar_roi_process_init, 0);
}
