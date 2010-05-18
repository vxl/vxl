#include <bocl/bocl_manager.txx>
#include <boxm2/ocl/boxm_ray_trace_manager.h>
#include <boxm2/boxm_apm_traits.h>

typedef boxm_ray_trace_manager<float > boxm_ray_trace_manager_float;

BOCL_MANAGER_INSTANTIATE(boxm_ray_trace_manager_float );
