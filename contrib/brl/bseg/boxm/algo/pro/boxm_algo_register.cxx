#include "boxm_algo_processes.h"
#include "boxm_algo_register.h"

#include <bprb/bprb_macros.h>
#include <bprb/bprb_batch_process_manager.h>
#include <bprb/bprb_func_process.h>



void boxm_algo_register::register_datatype()
{
}

void boxm_algo_register::register_process()
{
    REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm_split_scene_process ,"boxmSplitSceneProcess");
}
