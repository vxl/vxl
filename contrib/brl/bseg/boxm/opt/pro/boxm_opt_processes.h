#ifndef boxm_opt_processes_h_
#define boxm_opt_processes_h_

#include <bprb/bprb_func_process.h>
#include <bprb/bprb_macros.h>

//the init functions
DECLARE_FUNC_CONS(boxm_opt_bayesian_update_process);
DECLARE_FUNC_CONS(boxm_update_rt_process);
DECLARE_FUNC_CONS(boxm_render_expected_rt_process);
DECLARE_FUNC_CONS(boxm_render_expected_depth_rt_process);
DECLARE_FUNC_CONS(boxm_generate_opt_samples_process);


#endif
