#include <bocl/bocl_manager.txx>
#include <boxm/ocl/tests/online_update_test_manager.h>


typedef online_update_test_manager<float > online_update_test_manager_float;

BOCL_MANAGER_INSTANTIATE(online_update_test_manager_float );
