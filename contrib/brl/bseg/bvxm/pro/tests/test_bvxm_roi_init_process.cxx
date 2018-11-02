#include <string>
#include <iostream>
#include <testlib/testlib_test.h>
#include <bvxm/bvxm_world_params.h>
#include <bvxm/bvxm_voxel_world.h>

//For backwards compatibility
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <brdb/brdb_value.h>
#include <brdb/brdb_selection.h>

#include <bprb/bprb_batch_process_manager.h>
#include <bprb/bprb_parameters.h>
#include <bprb/bprb_macros.h>
#include <bprb/bprb_func_process.h>

#include <vil/vil_load.h>
#include <vil/vil_save.h>
#include <vil/file_formats/vil_nitf2_image.h>

#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_vector_3d.h>
#include <vpgl/file_formats/vpgl_nitf_rational_camera.h>

static void test_bvxm_roi_init_process()
{
  DECLARE_FUNC_CONS(bvxm_roi_init_process);
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bvxm_roi_init_process, "bvxmRoiInitProcess");
  REGISTER_DATATYPE_LONG_FORM(std::string,vcl_string);
  REGISTER_DATATYPE(bvxm_voxel_world_sptr);
  REGISTER_DATATYPE(vil_image_view_base_sptr);
  REGISTER_DATATYPE(vpgl_camera_double_sptr);

  std::string image_path = "./po_39928_pan_0000000_chip_700_700_noICHIPB.nitf";
  // set the inputs
  brdb_value_sptr v0 = new brdb_value_t<std::string>(image_path);

  // extract the camera
  vil_image_resource_sptr img = vil_load_image_resource(image_path.c_str());
  auto* nitf =  static_cast<vil_nitf2_image*> (img.ptr());
  vpgl_camera_double_sptr camera = new vpgl_nitf_rational_camera(nitf, true);
  brdb_value_sptr v1 = new brdb_value_t<vpgl_camera_double_sptr>(camera);

  // create the voxel world
  vgl_vector_3d<unsigned int> num_voxels(30,30,5);
  float voxel_length = 10;
  vpgl_lvcs_sptr lvcs = new vpgl_lvcs(32.716835, 117.163714, 0);
  bvxm_world_params_sptr world_params = new bvxm_world_params();
  world_params->set_params("./", vgl_point_3d<float>(0.0f,0.0f,0.0f), num_voxels, voxel_length, lvcs);
  bvxm_voxel_world_sptr world = new bvxm_voxel_world();
  world->set_params(world_params);
  brdb_value_sptr v2 = new brdb_value_t<bvxm_voxel_world_sptr>(world);

  brdb_value_sptr v3 = new brdb_value_t<unsigned>(0);

  bool good = bprb_batch_process_manager::instance()->init_process("bvxmRoiInitProcess");
  good = good && bprb_batch_process_manager::instance()->set_params("roi_params.xml");
  good = good && bprb_batch_process_manager::instance()->set_input(0, v0);
  good = good && bprb_batch_process_manager::instance()->set_input(1, v1);
  good = good && bprb_batch_process_manager::instance()->set_input(2, v2);
  good = good && bprb_batch_process_manager::instance()->set_input(3, v3);
  good = good && bprb_batch_process_manager::instance()->run_process();
  TEST("run roi process should be unsuccesful", good ,false);

  // run with good lvcs setting
  vpgl_lvcs_sptr lvcs_good = new vpgl_lvcs(32.716835, -117.163714, 0);
  world_params->set_params("./", vgl_point_3d<float>(0.0f,0.0f,0.0f), num_voxels, voxel_length, lvcs_good);
  world->set_params(world_params);
  good=true;
  good = good && bprb_batch_process_manager::instance()->run_process();

  unsigned id_cam, id_img;
  good = good && bprb_batch_process_manager::instance()->commit_output(0, id_cam);
  good = good && bprb_batch_process_manager::instance()->commit_output(1, id_img);
  TEST("run roi process", good ,true);

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
  TEST("camera output non-null", non_null ,true);

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
  TEST("camera output non-null", non_null ,true);

  auto* result =
    static_cast<brdb_value_t<vil_image_view_base_sptr>* >(value_img.ptr());
  vil_image_view_base_sptr nitf_roi = result->value();
  bool saved = vil_save(*nitf_roi, "./roi.tif");
  TEST("saved", saved, true);
}

TESTMAIN(test_bvxm_roi_init_process);
