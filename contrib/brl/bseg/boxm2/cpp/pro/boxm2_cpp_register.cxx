#include "boxm2_cpp_register.h"

#include <bprb/bprb_macros.h>
#include <bprb/bprb_batch_process_manager.h>
#include <bprb/bprb_func_process.h>

#include "boxm2_cpp_processes.h"

void boxm2_cpp_register::register_datatype()
{
}

void boxm2_cpp_register::register_process()
{
  // utilities
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm2_cpp_render_expected_image_process, "boxm2CppRenderExpectedImageProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm2_cpp_render_cone_expected_image_process, "boxm2CppRenderConeExpectedImageProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm2_cpp_update_image_process, "boxm2CppUpdateImageProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm2_cpp_cone_update_image_process, "boxm2CppConeUpdateImageProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm2_cpp_refine_process2, "boxm2CppRefineProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm2_cpp_change_detection_process2, "boxm2CppChangeDetectionProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm2_cpp_query_cell_data_process, "boxm2CppQueryCellDataProcess");
//  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm2_cpp_render_expected_depth_process, "boxm2CppRenderExpectedDepthProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm2_cpp_cast_intensities_process, "boxm2CppCastIntensitiesProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm2_cpp_mean_intensities_batch_process, "boxm2CppMeanIntensitiesBatchProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm2_cpp_mean_intensities_print_process, "boxm2CppMeanIntensitiesPrintProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm2_cpp_filter_process, "boxm2CppFilterProcess");
}
