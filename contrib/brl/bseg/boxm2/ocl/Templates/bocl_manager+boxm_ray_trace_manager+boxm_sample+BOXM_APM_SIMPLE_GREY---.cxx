#include <bocl/bocl_manager.txx>
#include <boxm2/ocl/boxm_ray_trace_manager.h>
#include <boxm2/boxm_apm_traits.h>



typedef boxm_ray_trace_manager<boxm_sample<BOXM_APM_SIMPLE_GREY> > boxm_ray_trace_manager_simple_grey;

BOCL_MANAGER_INSTANTIATE(boxm_ray_trace_manager_simple_grey );
