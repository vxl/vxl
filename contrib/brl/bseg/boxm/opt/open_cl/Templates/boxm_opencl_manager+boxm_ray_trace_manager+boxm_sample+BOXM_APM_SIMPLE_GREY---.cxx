#include <boxm/opt/open_cl/boxm_opencl_manager.txx>
#include <boxm/opt/open_cl/boxm_ray_trace_manager.h>
#include <boxm/boxm_apm_traits.h>



typedef boxm_ray_trace_manager<boxm_sample<BOXM_APM_SIMPLE_GREY> > boxm_ray_trace_manager_simple_grey;

BOXM_OPENCL_MANAGER_INSTANTIATE(boxm_ray_trace_manager_simple_grey );
