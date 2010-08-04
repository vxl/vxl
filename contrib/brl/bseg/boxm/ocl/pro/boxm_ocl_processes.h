#ifndef boxm_ocl_processes_h_
#define boxm_ocl_processes_h_

#include <bprb/bprb_func_process.h>
#include <bprb/bprb_macros.h>

//the init functions
DECLARE_FUNC_CONS(boxm_ocl_render_expected_process);
DECLARE_FUNC_CONS(boxm_ocl_ray_bundle_expected_process);
DECLARE_FUNC_CONS(boxm_ocl_refine_process);
DECLARE_FUNC_CONS(boxm_ocl_render_all_blocks_expected_process);
DECLARE_FUNC_CONS(boxm_ocl_convert_boxm_to_ocl_process);


#endif
