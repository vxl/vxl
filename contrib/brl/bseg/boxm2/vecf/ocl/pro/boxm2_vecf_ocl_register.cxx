#include "boxm2_vecf_ocl_register.h"

#include <bprb/bprb_macros.h>
#include <bprb/bprb_batch_process_manager.h>
#include <bprb/bprb_func_process.h>
#include "boxm2_vecf_ocl_processes.h"

void boxm2_vecf_ocl_register::register_process()
{
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm2_vecf_ocl_transform_scene_process, "boxm2VecfOclTransformSceneProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm2_vecf_ocl_cache_neighbor_info_process, "boxm2VecfOclCacheNeighborInfoProcess");
}
