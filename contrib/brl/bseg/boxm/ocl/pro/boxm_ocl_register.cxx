#include "boxm_ocl_register.h"

#include <bprb/bprb_macros.h>
#include <bprb/bprb_batch_process_manager.h>
#include <bprb/bprb_func_process.h>

#include "boxm_ocl_processes.h"
#include <boxm/ocl/boxm_render_probe_manager_sptr.h>
#include <boxm/ocl/boxm_ocl_change_detection_manager_sptr.h>
#include <boxm/basic/boxm_util_data_types.h>
void boxm_ocl_register::register_datatype()
{  
    REGISTER_DATATYPE( boxm_ocl_change_detection_manager_sptr );
    REGISTER_DATATYPE( boxm_render_probe_manager_sptr );
    REGISTER_DATATYPE( boxm_array_1d_float_sptr );

}

void boxm_ocl_register::register_process()
{
    REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm_ocl_ray_bundle_expected_process,"boxmOclRayBundleExpectedProcess");
    REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm_ocl_refine_process,"boxmOclRefineProcess");
    REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm_ocl_convert_boxm_to_ocl_process,"boxmOclConvertBoxmToOclProcess");
    REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm_ocl_init_render_probe_process,"boxmOclInitRenderProbeProcess");
    REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm_ocl_run_render_probe_process,"boxmOclRunRenderProbeProcess");
    REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm_ocl_finish_render_probe_process,"boxmOclFinishRenderProbeProcess");
    REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm_ocl_init_change_detection_process,"boxmOclInitChangeDetectionProcess");
    REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm_ocl_run_change_detection_process,"boxmOclRunChangeDetectionProcess");
    REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm_ocl_finish_change_detection_process,"boxmOclFinishChangeDetectionProcess");

}
