#include "boxm_opt_register.h"

#include <bprb/bprb_macros.h>
#include <bprb/bprb_batch_process_manager.h>
#include <bprb/bprb_func_process.h>

#include "boxm_opt_processes.h"

void boxm_opt_register::register_datatype()
{
}

void boxm_opt_register::register_process()
{
    REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm_opt_bayesian_update_process,"boxmOptBayesianUpdateProcess");
    REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm_update_rt_process,"boxmUpdateRTProcess");
    REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm_render_expected_rt_process, "boxmRenderExpectedRTProcess");
    REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm_render_expected_depth_rt_process, "boxmRenderExpectedDepthRTProcess");
    REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm_generate_opt_samples_process, "boxmGenerateOptSamplesProcess");
}
