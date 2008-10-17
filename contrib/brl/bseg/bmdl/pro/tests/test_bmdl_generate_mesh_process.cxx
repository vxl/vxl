#include <testlib/testlib_test.h>
#include "../bmdl_generate_mesh_process.h"

#include <vcl_string.h>
#include <vcl_iostream.h>

#include <vgl/vgl_polygon.h>
#include <vgl/io/vgl_io_polygon.h>

#include <brdb/brdb_value.h>
#include <brdb/brdb_selection.h>

#include <bprb/bprb_batch_process_manager.h>
#include <bprb/bprb_parameters.h>
#include <bprb/bprb_macros.h>

#include <vil/vil_load.h>
#include <vil/vil_save.h>
#include <vil/vil_image_view.h>

MAIN( test_bmdl_generate_mesh_process )
{
  REG_PROCESS(bmdl_generate_mesh_process, bprb_batch_process_manager);
  REGISTER_DATATYPE(vcl_string);
  REGISTER_DATATYPE(vil_image_view_base_sptr);

  //create vil_image_view_base_sptr
  vcl_string label_img_path = "label.tif";
  vil_image_view_base_sptr label = vil_load(label_img_path.c_str());

  vcl_string height_img_path = "height.tif";
  vil_image_view_base_sptr height = vil_load(height_img_path.c_str());

  vcl_string ground_img_path = "ground.tif";
  vil_image_view_base_sptr ground = vil_load(ground_img_path.c_str());

  vcl_string polygons_path = "polygons.bin";
  vcl_string meshes_path = "meshes.obj";

  brdb_value_sptr v0 = new brdb_value_t<vcl_string>(polygons_path);
  brdb_value_sptr v1 = new brdb_value_t<vil_image_view_base_sptr>(label);
  brdb_value_sptr v2 = new brdb_value_t<vil_image_view_base_sptr>(height);
  brdb_value_sptr v3 = new brdb_value_t<vil_image_view_base_sptr>(ground);
  brdb_value_sptr v4 = new brdb_value_t<vcl_string>(meshes_path);

  bool good = bprb_batch_process_manager::instance()->init_process("bmdlGenerateMeshProcess");
  good = good && bprb_batch_process_manager::instance()->set_input(0, v0);
  good = good && bprb_batch_process_manager::instance()->set_input(1, v1);
  good = good && bprb_batch_process_manager::instance()->set_input(2, v2);
  good = good && bprb_batch_process_manager::instance()->set_input(3, v3);
  good = good && bprb_batch_process_manager::instance()->set_input(4, v4);
  good = good && bprb_batch_process_manager::instance()->run_process();

  TEST("run generate mesh process", good ,true);

  SUMMARY();
}
