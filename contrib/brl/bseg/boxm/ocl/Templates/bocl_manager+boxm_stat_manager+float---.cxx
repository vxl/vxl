#include <bocl/bocl_manager.txx>
#include <boxm/ocl/boxm_stat_manager.h>
#include <boxm/boxm_apm_traits.h>

typedef boxm_stat_manager<float > boxm_stat_manager_float;

BOCL_MANAGER_INSTANTIATE(boxm_stat_manager_float );
