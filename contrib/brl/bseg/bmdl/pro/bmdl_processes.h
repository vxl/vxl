#ifndef bmdl_processes_h_
#define bmdl_processes_h_

#include <bprb/bprb_func_process.h>

// execute functions
bool bmdl_lidar_roi_process(bprb_func_process& pro);
bool bmdl_classify_process(bprb_func_process& pro);
bool bmdl_generate_mesh_process(bprb_func_process& pro);
bool bmdl_trace_boundaries_process(bprb_func_process& pro);

// init functions
bool bmdl_lidar_roi_process_init(bprb_func_process& pro);
bool bmdl_classify_process_init(bprb_func_process& pro);
bool bmdl_generate_mesh_process_init(bprb_func_process& pro);
bool bmdl_trace_boundaries_process_init(bprb_func_process& pro);
//bool bmdl_modeling_process(bprb_func_process& pro);

#endif
