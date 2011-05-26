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
  // utilities
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm2_ocl_render_expected_image_process, "boxm2OclRenderExpectedImageProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm2_ocl_render_gl_expected_image_process, "boxm2OclRenderGlExpectedImageProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm2_ocl_render_expected_depth_process, "boxm2OclRenderExpectedDepthProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm2_ocl_render_expected_height_map_process, "boxm2OclRenderExpectedHeightMapProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm2_ocl_update_process, "boxm2OclUpdateProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm2_ocl_refine_process, "boxm2OclRefineProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm2_ocl_render_expected_color_process, "boxm2OclRenderExpectedColorProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm2_ocl_update_color_process, "boxm2OclUpdateColorProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm2_ocl_render_gl_expected_color_process, "boxm2OclRenderGlExpectedColorProcess"); 
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm2_ocl_change_detection_process, "boxm2OclChangeDetectionProcess"); 
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm2_ocl_filter_process, "boxm2OclFilterProcess");

  
  // research
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm2_ocl_update_histogram_process, "boxm2OclUpdateHistogramProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm2_ocl_batch_probability_process, "boxm2OclBatchProbabilityProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm2_ocl_probability_of_image_process, "boxm2OclProbabilityOfImageProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm2_ocl_probability_of_image_gl_process, "boxm2OclProbabilityOfImageGlProcess");
  // tools
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm2_ocl_query_hist_data_process, "boxm2OclQueryHistDataProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm2_ocl_query_cell_data_process, "boxm2OclQueryCellDataProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm2_ocl_convert_float_image_to_rgba_process, "boxm2OclConvertFloatImageToRgbaProcess");
  
  //alternative ray trace
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm2_ocl_render_cone_expected_image_process, "boxm2OclRenderConeExpectedImageProcess");

  // admin
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm2_create_opencl_cache_process, "boxm2CreateOpenclCacheProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm2_clear_opencl_cache_process, "boxm2ClearOpenclCacheProcess");
}
