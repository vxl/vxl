#include "boxm_algo_sp_register.h"

#include <bprb/bprb_macros.h>
#include <bprb/bprb_batch_process_manager.h>
#include <bprb/bprb_func_process.h>

#include "boxm_algo_sp_processes.h"

void boxm_algo_sp_register::register_datatype()
{
}

void boxm_algo_sp_register::register_process()
{
    REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm_update_process,"boxmUpdateSPProcess");
    REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm_render_expected_process, "boxmRenderExpectedProcess");
    REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm_volume_visibility_process, "boxmVolumeVisibilityProcess");
}
