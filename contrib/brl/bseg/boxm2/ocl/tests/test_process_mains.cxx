//:
// \file
// \author Andy Miller
// \date 26-Oct-2010
#include <iostream>
#include <algorithm>
#include <testlib/testlib_test.h>
#include <testlib/testlib_root_dir.h>
#include <vcl_where_root_dir.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <boxm2/boxm2_scene.h>
#include <boxm2/boxm2_util.h>
#include <bocl/bocl_manager.h>
#include <boxm2/ocl/boxm2_ocl_util.h>
#include <boxm2/ocl/boxm2_opencl_cache.h>

#include <bocl/bocl_device.h>
#include <bocl/bocl_kernel.h>
#include <brdb/brdb_value.h>
#include <brdb/brdb_selection.h>
#include <bprb/bprb_batch_process_manager.h>
#include <bprb/bprb_parameters.h>
#include <bprb/bprb_macros.h>
#include <bprb/bprb_func_process.h>
#include <vil/vil_image_view_base.h>
#include <vil/vil_save.h>
#include <vpgl/vpgl_perspective_camera.h>
#include <vnl/vnl_random.h>


void test_process_mains()
{
  // Create scene from file
  //register data types and process functions
  DECLARE_FUNC_CONS(boxm2_ocl_update_process);
  DECLARE_FUNC_CONS(boxm2_ocl_refine_process);
  DECLARE_FUNC_CONS(boxm2_write_cache_process);
  DECLARE_FUNC_CONS(boxm2_ocl_render_expected_color_process);
  DECLARE_FUNC_CONS(boxm2_ocl_render_expected_image_process);

  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm2_ocl_render_expected_color_process, "boxm2OclRenderExpectedColorProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm2_ocl_render_expected_image_process, "boxm2OclRenderExpectedImageProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm2_ocl_update_process, "boxm2OclUpdateProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm2_ocl_refine_process, "boxm2OclRefineProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm2_write_cache_process, "boxm2WriteCacheProcess");

  REGISTER_DATATYPE(boxm2_cache_sptr);
  REGISTER_DATATYPE(boxm2_opencl_cache_sptr);
  REGISTER_DATATYPE(boxm2_scene_sptr);
  REGISTER_DATATYPE(bocl_mem_sptr);
  REGISTER_DATATYPE(vil_image_view_base_sptr);
  REGISTER_DATATYPE(float);

  bool good = bprb_batch_process_manager::instance()->init_process("boxm2OclUpdateProcess");
  TEST("Update Process Initialized",good, true);
  good = bprb_batch_process_manager::instance()->init_process("boxm2OclRefineProcess");
  TEST("Refine Process Initialized",good, true);
  good = bprb_batch_process_manager::instance()->init_process("boxm2WriteCacheProcess");
  TEST("Writing Cache Initialized",good, true);
  good = bprb_batch_process_manager::instance()->init_process("boxm2OclRenderExpectedImageProcess");
  TEST("Rendering Expected Image Initialized",good, true);
  good = bprb_batch_process_manager::instance()->init_process("boxm2OclRenderExpectedColorProcess");
  TEST("Rendering Expected Color Image Initialized",good, true);

}

TESTMAIN( test_process_mains );
