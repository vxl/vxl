#include "boxm_algo_rt_register.h"

#include <bprb/bprb_macros.h>
#include <bprb/bprb_batch_process_manager.h>
#include <bprb/bprb_func_process.h>

#include "boxm_algo_rt_processes.h"

void boxm_algo_rt_register::register_datatype()
{
}

void boxm_algo_rt_register::register_process()
{
    REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm_opt_bayesian_update_process,"boxmOptBayesianUpdateProcess");
    REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm_update_rt_process,"boxmUpdateRTProcess");
    REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm_render_expected_rt_process, "boxmRenderExpectedRTProcess");
    REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm_render_expected_depth_rt_process, "boxmRenderExpectedDepthRTProcess");
    REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm_generate_opt_samples_process, "boxmGenerateOptSamplesProcess");
    REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm_generate_edge_samples_process, "boxmGenerateEdgeSamplesProcess");
    REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm_edge_update_process, "boxmEdgeUpdateProcess");
    REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm_render_expected_edge_process, "boxmRenderExpectedEdgeProcess");
    REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm_rpc_registration_process, "boxmRPCRegistrationProcess");
    REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm_line_backproject_process, "boxmLineBackprojectProcess");
    REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm_opt2_update_process,"boxmOpt2UpdateProcess");
    REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm_opt3_update_process,"boxmOpt3UpdateProcess");
    REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm_generate_opt2_samples_process,"boxmGenerateOpt2SamplesProcess");
    REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm_generate_opt3_samples_process,"boxmGenerateOpt3SamplesProcess");
    REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm_ray_statistics_process,"boxmRayStatisticsProcess");
    REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm_change_probability_process,"boxmChangeProbabilityProcess");
    REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm_estimate_camera_process,"boxmEstimateCameraProcess");
    REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm_edge_refine_updates_process, "boxmEdgeRefineUpdatesProcess");
}
