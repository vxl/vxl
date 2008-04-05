#include <testlib/testlib_test.h>
#include "../bvxm_create_synth_lidar_data_process.h"

#include <vcl_string.h>
#include <vcl_iostream.h>

#include <brdb/brdb_value.h>
#include <brdb/brdb_selection.h>

#include <bprb/bprb_batch_process_manager.h>
#include <bprb/bprb_parameters.h>
#include <bprb/bprb_macros.h>

#include <vil/vil_save.h>

#include <vpgl/vpgl_camera.h>


MAIN( test_bvxm_create_synth_lidar_data_process)
{
  REG_PROCESS(bvxm_create_synth_lidar_data_process, bprb_batch_process_manager);
  REGISTER_DATATYPE(vil_image_view_base_sptr);
  REGISTER_DATATYPE(vpgl_camera_double_sptr);

  bool good = bprb_batch_process_manager::instance()->init_process("bvxmCreateSynthLidarDataProcess");
  good = bprb_batch_process_manager::instance()->set_params("synth_test_params.xml");
  good = good && bprb_batch_process_manager::instance()->run_process();
  TEST("run generate synthetic lidar data process should be successful", good ,true);

  unsigned id_cam, id_img;
  good = good && bprb_batch_process_manager::instance()->commit_output(0, id_img);
  good = good && bprb_batch_process_manager::instance()->commit_output(1, id_cam);
  TEST("run generate synthetic lidar data process commits output", good ,true);

  // check if the results are in DB
  brdb_query_aptr Q_cam = brdb_query_comp_new("id", brdb_query::EQ, id_cam);
  brdb_selection_sptr S_cam = DATABASE->select("vpgl_camera_double_sptr_data", Q_cam);
  if (S_cam->size()!=1){
    vcl_cout << "in bprb_batch_process_manager::set_input_from_db(.) -"
             << " no selections\n";
  }

  brdb_value_sptr value;
  if (!S_cam->get_value(vcl_string("value"), value)) {
    vcl_cout << "in bprb_batch_process_manager::set_input_from_db(.) -"
             << " didn't get value\n";
  }
  bool non_null = (value != 0);
  TEST("camera output non-null", non_null ,true);

  brdb_query_aptr Q_img = brdb_query_comp_new("id", brdb_query::EQ, id_img);
  brdb_selection_sptr S_img = DATABASE->select("vil_image_view_base_sptr_data", Q_img);
  if (S_img->size()!=1){
    vcl_cout << "in bprb_batch_process_manager::set_input_from_db(.) -"
             << " no selections\n";
  }

  brdb_value_sptr value_img;
  if (!S_img->get_value(vcl_string("value"), value_img)) {
    vcl_cout << "in bprb_batch_process_manager::set_input_from_db(.) -"
             << " didn't get value\n";
  }
  non_null = (value_img != 0);
  TEST("image output non-null", non_null ,true);

  brdb_value_t<vil_image_view_base_sptr>* result =
    static_cast<brdb_value_t<vil_image_view_base_sptr>* >(value_img.ptr());
  vil_image_view_base_sptr img = result->value();
  bool saved = vil_save(*img, "./lidar_img.tif");

  SUMMARY();
}
