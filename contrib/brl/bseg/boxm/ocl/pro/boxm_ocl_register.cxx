#include "boxm_ocl_register.h"

#include <bprb/bprb_macros.h>
#include <bprb/bprb_batch_process_manager.h>
#include <bprb/bprb_func_process.h>

#include "boxm_ocl_processes.h"

void boxm_ocl_register::register_datatype()
{
}

void boxm_ocl_register::register_process()
{
    REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm_ocl_render_expected_process,"boxmOclRenderExpectedProcess");
    REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm_ocl_stack_render_expected_process,"boxmOclStackRenderExpectedProcess");
    REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm_ocl_ray_bundle_expected_process,"boxmOclRayBundleExpectedProcess");
    REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm_ocl_refine_process,"boxmOclRefineProcess");
    REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm_ocl_render_all_blocks_expected_process,"boxmOclRenderAllBlocksExpectedProcess");
}
