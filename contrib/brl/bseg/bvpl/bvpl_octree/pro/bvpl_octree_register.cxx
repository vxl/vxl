#include "bvpl_octree_register.h"

#include <bprb/bprb_macros.h>
#include <bprb/bprb_batch_process_manager.h>
#include <bprb/bprb_func_process.h>
#include "bvpl_octree_processes.h"


void bvpl_octree_register::register_datatype()
{
}

void bvpl_octree_register::register_process()
{
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bvpl_scene_vector_operator_process, "bvplSceneVectorOperatorProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bvpl_save_vrml_process, "bvplSaveVrmlProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bvpl_create_scene_process, "bvplCreateSceneProcess");

}
