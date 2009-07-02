#ifndef bvxm_grid_processes_h_
#define bvxm_grid_processes_h_
//:
// \file

#include <bprb/bprb_func_process.h>
#include <bprb/bprb_macros.h>

//:Grid processes
DECLARE_FUNC_CONS(bvxm_load_grid_process);
DECLARE_FUNC_CONS(bvxm_save_grid_raw_process);
DECLARE_FUNC_CONS(bvxm_crop_grid_process);
DECLARE_FUNC_CONS(bvxm_multiply_grids_process);
DECLARE_FUNC_CONS(bvxm_threshold_grid_process);
DECLARE_FUNC_CONS(bvxm_grid_to_image_stack_process);
DECLARE_FUNC_CONS(bvxm_combine_grids_process);
#endif
