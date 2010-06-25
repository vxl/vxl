#include <bocl/bocl_manager.txx>
#include <boxm/ocl/tests/online_update_test_manager.h>
#include <boxm/boxm_apm_traits.h>



typedef online_update_test_manager<boxm_sample<BOXM_APM_MOG_GREY> > online_update_test_manager_mog_grey;

BOCL_MANAGER_INSTANTIATE(online_update_test_manager_mog_grey );
