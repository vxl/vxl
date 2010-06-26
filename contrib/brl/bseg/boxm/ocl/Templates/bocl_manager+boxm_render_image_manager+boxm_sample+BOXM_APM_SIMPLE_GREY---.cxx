#include <bocl/bocl_manager.txx>
#include <boxm/ocl/boxm_render_image_manager.h>
#include <boxm/boxm_apm_traits.h>



typedef boxm_render_image_manager<boxm_sample<BOXM_APM_SIMPLE_GREY> > boxm_render_image_manager_simple_grey;

BOCL_MANAGER_INSTANTIATE(boxm_render_image_manager_simple_grey );
