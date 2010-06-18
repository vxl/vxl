#include <bocl/bocl_manager.txx>
#include <boxm/ocl/boxm_online_update_manager.h>
#include <boxm/boxm_apm_traits.h>



typedef boxm_online_update_manager<boxm_sample<BOXM_APM_MOG_GREY> > boxm_online_update_manager_mog_grey;

BOCL_MANAGER_INSTANTIATE(boxm_online_update_manager_mog_grey );
