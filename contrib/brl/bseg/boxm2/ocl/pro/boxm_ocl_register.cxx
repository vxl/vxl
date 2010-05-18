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
}
