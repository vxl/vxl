#include "boxm2_ocl_register.h"

#include <bprb/bprb_macros.h>
#include <bprb/bprb_batch_process_manager.h>
#include <bprb/bprb_func_process.h>


#include "boxm2_ocl_processes.h"
#include <boxm2/ocl/boxm2_opencl_cache.h>

void boxm2_ocl_register::register_datatype()
{
  REGISTER_DATATYPE( boxm2_opencl_cache_sptr );
}

void boxm2_ocl_register::register_process()
{
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm2_create_opencl_cache_process, "boxm2CreateOpenclCacheProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm2_ocl_render_expected_image_process, "boxm2OclRenderExpectedImageProcess");
}
