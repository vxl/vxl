#include <boxm/opt/open_cl/boxm_opencl_manager.txx>
#include <boxm/opt/open_cl/boxm_ray_trace_manager.h>
#include <boxm/boxm_apm_traits.h>



typedef boxm_ray_trace_manager<float > boxm_ray_trace_manager_float;

BOXM_OPENCL_MANAGER_INSTANTIATE(boxm_ray_trace_manager_float );
