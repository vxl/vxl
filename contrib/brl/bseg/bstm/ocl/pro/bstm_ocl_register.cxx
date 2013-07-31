#include "bstm_ocl_register.h"

#include <bprb/bprb_macros.h>
#include <bprb/bprb_batch_process_manager.h>
#include <bprb/bprb_func_process.h>

#include "bstm_ocl_processes.h"
#include <bstm/ocl/bstm_opencl_cache.h>

void bstm_ocl_register::register_datatype()
{
  REGISTER_DATATYPE( bstm_opencl_cache_sptr );
}

void bstm_ocl_register::register_process()
{
  // utilities
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bstm_clear_opencl_cache_process, "bstmOclClearCacheProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bstm_create_opencl_cache_process, "bstmOclCreateCacheProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bstm_ocl_render_expected_image_process, "bstmOclRenderExpectedImageProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bstm_ocl_render_expected_change_process, "bstmOclRenderExpectedChangeProcess");

  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bstm_ocl_render_gl_expected_image_process, "bstmOclRenderGlExpectedImageProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bstm_ocl_render_gl_expected_color_process, "bstmOclRenderGlExpectedColorProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bstm_ocl_change_detection_process, "bstmOclChangeDetectionProcess");

  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bstm_ocl_label_along_ray_process, "bstmOclLabelRayProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bstm_ocl_update_change_process, "bstmOclUpdateChangeProcess");

  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bstm_ocl_get_surface_pt_process, "bstmOclGetSurfacePtProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bstm_ocl_minfo_process, "bstmOclMIProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bstm_ocl_track_process, "bstmOclTrackProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bstm_ocl_update_process, "bstmOclUpdateProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bstm_ocl_update_color_process, "bstmOclUpdateColorProcess");

}
