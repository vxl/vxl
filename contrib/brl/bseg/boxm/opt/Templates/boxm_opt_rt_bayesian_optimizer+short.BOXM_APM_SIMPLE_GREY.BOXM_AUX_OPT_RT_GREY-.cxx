#include <boxm/boxm_apm_traits.h>
#include <boxm/opt/boxm_aux_traits.h>
#include <boxm/boxm_simple_grey_processor.h>

#include "../boxm_opt_rt_bayesian_optimizer.txx"

BOXM_OPT_RT_BAYESIAN_OPTIMIZER_INSTANTIATE(short, BOXM_APM_SIMPLE_GREY, BOXM_AUX_OPT_RT_GREY);
