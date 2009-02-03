#ifndef bmdl_processes_h_
#define bmdl_processes_h_

#include <bprb/bprb_macros.h>
#include <bprb/bprb_func_process.h>

// execute functions
DECLARE_FUNC_CONS(bmdl_lidar_roi_process);
DECLARE_FUNC_CONS(bmdl_classify_process);
DECLARE_FUNC_CONS(bmdl_generate_mesh_process);
DECLARE_FUNC_CONS(bmdl_trace_boundaries_process);
DECLARE_FUNC_CONS(bmdl_modeling_process);

#endif
