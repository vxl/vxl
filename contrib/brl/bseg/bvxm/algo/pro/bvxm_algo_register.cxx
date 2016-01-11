#include "bvxm_algo_register.h"

#include <bprb/bprb_macros.h>
#include <bprb/bprb_batch_process_manager.h>
#include <bprb/bprb_func_process.h>
#include "bvxm_algo_processes.h"
#include <bvxm/bvxm_voxel_world.h>
#include <bvxm/grid/bvxm_voxel_slab_base.h>


void bvxm_algo_register::register_datatype()
{
}

void bvxm_algo_register::register_process()
{

  // Algo Processes
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bvxm_merge_mog_process, "bvxmMergeMogProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bvxm_mog_to_mpm_process, "bvxmMogToMpmProcess");
 // REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bvxm_mog_l2_process, "bvxmMogL2Process");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm_scene_to_bvxm_grid_process, "boxmSceneToBvxmGridProcess");
}
