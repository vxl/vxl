#include <string>
#include <iostream>
#include <testlib/testlib_test.h>

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <brdb/brdb_value.h>
#include <brdb/brdb_selection.h>

#include <bprb/bprb_batch_process_manager.h>
#include <bprb/bprb_macros.h>
#include <bprb/bprb_func_process.h>

#include <vil/vil_save.h>
#include <vil/vil_image_view_base.h>
#include <vpgl/vpgl_camera_double_sptr.h>

#include <bvxm/pro/processes/bvxm_synth_world_generator.h>

static void test_bvxm_create_synth_lidar_data_process()
{
  DECLARE_FUNC_CONS(bvxm_create_synth_lidar_data_process);
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager,bvxm_create_synth_lidar_data_process, "bvxmCreateSynthLidarDataProcess");
  REGISTER_DATATYPE(vil_image_view_base_sptr);
  REGISTER_DATATYPE(vpgl_camera_double_sptr);

  bool good = bprb_batch_process_manager::instance()->init_process("bvxmCreateSynthLidarDataProcess");
  good = good && bprb_batch_process_manager::instance()->set_params("synth_test_params.xml");
  good = good && bprb_batch_process_manager::instance()->run_process();
  TEST("run generate synthetic lidar data process should be successful", good, true);

  unsigned id_cam, id_img;
  good = good && bprb_batch_process_manager::instance()->commit_output(0, id_img);
  good = good && bprb_batch_process_manager::instance()->commit_output(1, id_cam);
  TEST("run generate synthetic lidar data process commits output", good, true);

  // check if the results are in DB
  brdb_query_aptr Q_cam = brdb_query_comp_new("id", brdb_query::EQ, id_cam);
  brdb_selection_sptr S_cam = DATABASE->select("vpgl_camera_double_sptr_data", std::move(Q_cam));
  if (S_cam->size()!=1){
    std::cout << "in bprb_batch_process_manager::set_input_from_db(.) -"
             << " no selections\n";
  }

  brdb_value_sptr value;
  if (!S_cam->get_value(std::string("value"), value)) {
    std::cout << "in bprb_batch_process_manager::set_input_from_db(.) -"
             << " didn't get value\n";
  }
  bool non_null = (value != nullptr);
  TEST("camera output non-null", non_null, true);

  brdb_query_aptr Q_img = brdb_query_comp_new("id", brdb_query::EQ, id_img);
  brdb_selection_sptr S_img = DATABASE->select("vil_image_view_base_sptr_data", std::move(Q_img));
  if (S_img->size()!=1){
    std::cout << "in bprb_batch_process_manager::set_input_from_db(.) -"
             << " no selections\n";
  }

  brdb_value_sptr value_img;
  if (!S_img->get_value(std::string("value"), value_img)) {
    std::cout << "in bprb_batch_process_manager::set_input_from_db(.) -"
             << " didn't get value\n";
  }
  non_null = (value_img != nullptr);
  TEST("image output non-null", non_null, true);

  auto* result =
    static_cast<brdb_value_t<vil_image_view_base_sptr>* >(value_img.ptr());
  vil_image_view_base_sptr img = result->value();

  bool saved = vil_save(*img, "./lidar_img.tif");

  TEST("image saved", saved, true);
}

TESTMAIN(test_bvxm_create_synth_lidar_data_process);
