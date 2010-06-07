#include <bocl/bocl_manager.txx>
#include <boxm/ocl/boxm_stack_ray_trace_manager.h>
#include <boxm/boxm_apm_traits.h>



typedef boxm_stack_ray_trace_manager<boxm_sample<BOXM_APM_SIMPLE_GREY> > boxm_stack_ray_trace_manager_simple_grey;

BOCL_MANAGER_INSTANTIATE(boxm_stack_ray_trace_manager_simple_grey );
