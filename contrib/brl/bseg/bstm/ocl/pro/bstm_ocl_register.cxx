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
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bstm_ocl_render_gl_expected_image_process, "bstmOclRenderGlExpectedImageProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bstm_ocl_render_gl_expected_color_process, "bstmOclRenderGlExpectedColorProcess");

}
