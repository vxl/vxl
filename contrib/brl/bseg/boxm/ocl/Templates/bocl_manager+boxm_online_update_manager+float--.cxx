#include <bocl/bocl_manager.txx>
#include <boxm/ocl/boxm_online_update_manager.h>


typedef boxm_online_update_manager<float > boxm_online_update_manager_float;

BOCL_MANAGER_INSTANTIATE(boxm_online_update_manager_float );
