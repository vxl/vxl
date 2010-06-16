#include <bocl/bocl_manager.txx>
#include <boxm/ocl/boxm_refine_manager.h>
#include <boxm/boxm_apm_traits.h>



typedef boxm_refine_manager<boxm_sample<BOXM_APM_MOG_GREY> > boxm_refine_manager_mog_grey;

BOCL_MANAGER_INSTANTIATE(boxm_refine_manager_mog_grey );
