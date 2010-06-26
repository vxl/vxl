#include <bocl/bocl_manager.txx>
#include <boxm/ocl/boxm_render_image_manager.h>
#include <boxm/boxm_apm_traits.h>



typedef boxm_render_image_manager<float > boxm_online_update_manager_float;

BOCL_MANAGER_INSTANTIATE(boxm_online_update_manager_float );
