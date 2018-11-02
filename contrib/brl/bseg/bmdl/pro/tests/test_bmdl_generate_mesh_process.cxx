#include <iostream>
#include <string>
#include <testlib/testlib_test.h>
#include "../bmdl_processes.h"

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <brdb/brdb_value.h>
#include <brdb/brdb_selection.h>

#include <bprb/bprb_batch_process_manager.h>
#include <bprb/bprb_func_process.h>
#include <bprb/bprb_parameters.h>
#include <bprb/bprb_macros.h>

#include <vil/vil_load.h>
#include <vil/vil_image_view_base.h>

#include <vpgl/vpgl_camera.h>
#include <vpgl/file_formats/vpgl_geo_camera.h>

static void test_bmdl_generate_mesh_process(int argc, char* argv[])
{
  REG_PROCESS_FUNC(bprb_func_process, bprb_batch_process_manager, bmdl_generate_mesh_process, "bmdlGenerateMeshProcess");
  REGISTER_DATATYPE_LONG_FORM(std::string,vcl_string);
  REGISTER_DATATYPE(vil_image_view_base_sptr);

  std::string image_base;
  if ( argc >= 2 ) {
    image_base = argv[1];
    image_base += "/";
  }

  //create vil_image_view_base_sptr
  std::string label_img_path = image_base + "label.tif";
  vil_image_view_base_sptr label = vil_load(label_img_path.c_str());

  std::string height_img_path = image_base + "height.tif";
  vil_image_view_base_sptr height = vil_load(height_img_path.c_str());

  std::string ground_img_path = image_base + "ground.tif";
  vil_image_view_base_sptr ground = vil_load(ground_img_path.c_str());

  std::string polygons_path = "polygons.bin";
  std::string meshes_path = "meshes.obj";
  vpgl_camera_double_sptr camera = new vpgl_geo_camera();

  brdb_value_sptr v0 = new brdb_value_t<std::string>(polygons_path);
  brdb_value_sptr v1 = new brdb_value_t<vil_image_view_base_sptr>(label);
  brdb_value_sptr v2 = new brdb_value_t<vil_image_view_base_sptr>(height);
  brdb_value_sptr v3 = new brdb_value_t<vil_image_view_base_sptr>(ground);
  brdb_value_sptr v4 = new brdb_value_t<std::string>(meshes_path);
  brdb_value_sptr v5 = new brdb_value_t<vpgl_camera_double_sptr>(camera);

  bool good = bprb_batch_process_manager::instance()->init_process("bmdlGenerateMeshProcess");
  good = good && bprb_batch_process_manager::instance()->set_input(0, v0);
  good = good && bprb_batch_process_manager::instance()->set_input(1, v1);
  good = good && bprb_batch_process_manager::instance()->set_input(2, v2);
  good = good && bprb_batch_process_manager::instance()->set_input(3, v3);
  good = good && bprb_batch_process_manager::instance()->set_input(4, v4);
  good = good && bprb_batch_process_manager::instance()->set_input(5, v5);
  good = good && bprb_batch_process_manager::instance()->run_process();

  TEST("run generate mesh process", good ,true);
}

TESTMAIN_ARGS(test_bmdl_generate_mesh_process);
