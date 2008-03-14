//:
// \file
// \brief  Test for rpc registration process
// \author Gamze D. Tunali
// \date   03/07/2008
//
#include <testlib/testlib_test.h>
#include "../bvxm_rpc_registration_process.h"
#include <bvxm/bvxm_voxel_world.h>

#include <vcl_string.h>
#include <vcl_vector.h>
#include <vcl_iostream.h>

#include <brdb/brdb_value.h>
#include <brdb/brdb_selection.h>

#include <bprb/bprb_batch_process_manager.h>
#include <bprb/bprb_parameters.h>
#include <bprb/bprb_macros.h>

#include <vil/vil_load.h>
#include <vil/vil_save.h>
#include <vil/vil_image_view_base.h>

#include <vpgl/vpgl_rational_camera.h>
#include <vpgl/vpgl_local_rational_camera.h>

MAIN( test_bvxm_rpc_registration_process )
{
  REG_PROCESS(bvxm_rpc_registration_process, bprb_batch_process_manager);
  REGISTER_DATATYPE(bvxm_voxel_world_sptr);
  REGISTER_DATATYPE(vpgl_camera_double_sptr);
  REGISTER_DATATYPE(vil_image_view_base_sptr);

  vgl_vector_3d<unsigned int> num_voxels(20,20,5);
  float voxel_length = 1.0;
  bgeo_lvcs_sptr lvcs = new bgeo_lvcs(33.3358982058333, 44.38220165, 74.5333333333333);
  bvxm_world_params_sptr voxel_world_params = new bvxm_world_params();
  voxel_world_params->set_params("C:\\temp\\voxel_world", vgl_point_3d<float> (-10.0,-10.0,-2.5), num_voxels, voxel_length, lvcs);
  bvxm_voxel_world_sptr voxel_world = new bvxm_voxel_world();
  voxel_world->set_params(voxel_world_params);
 
  vpgl_rational_camera<double>* camera_rational = read_rational_camera<double>("rpc_registration_camera.rpb");
  vpgl_camera<double>* camera = new vpgl_local_rational_camera<double>(*lvcs,*camera_rational);
  vil_image_view_base_sptr img = vil_load("rpc_registration_image.png");

  bool good = bprb_batch_process_manager::instance()->init_process("bvxmRpcRegistrationProcess");
  good = good && bprb_batch_process_manager::instance()->set_params("rpc_registration_parameters.xml");

  brdb_value_sptr v0 = new brdb_value_t<bvxm_voxel_world_sptr>(voxel_world);
  brdb_value_sptr v1 = new brdb_value_t<vpgl_camera_double_sptr>(camera);
  brdb_value_sptr v2 = new brdb_value_t<vil_image_view_base_sptr>(img);
  brdb_value_sptr v3 = new brdb_value_t<bool>(true);
  good = good && bprb_batch_process_manager::instance()->set_input(0, v0);
  good = good && bprb_batch_process_manager::instance()->set_input(1, v1);
  good = good && bprb_batch_process_manager::instance()->set_input(2, v2);
  good = good && bprb_batch_process_manager::instance()->set_input(3, v3);
  good = good && bprb_batch_process_manager::instance()->run_process();

  unsigned id_cam, id_img;
  good = good && bprb_batch_process_manager::instance()->commit_output(0, id_cam);
  good = good && bprb_batch_process_manager::instance()->commit_output(1, id_img);
  TEST("run bvxm_rpc_registration_process", good ,true);

  // check if the results are in DB
  brdb_query_aptr Q = brdb_query_comp_new("id", brdb_query::EQ, id_cam);
  brdb_selection_sptr S = DATABASE->select("vpgl_camera_double_sptr_data", Q);
  if(S->size()!=1){
    vcl_cout << "in bprb_batch_process_manager::set_input_from_db(.) -"
    << " no selections\n";
  }

  brdb_value_sptr value;
  if (!S->get_value(vcl_string("value"), value)) {
    vcl_cout << "in bprb_batch_process_manager::set_input_from_db(.) -"
     << " didn't get value\n";
  }
  bool non_null = (value != 0);
  TEST("vpgl_camera_double_sptr non-null", non_null ,true);

  brdb_value_t<vpgl_camera_double_sptr>* result = 
    static_cast<brdb_value_t<vpgl_camera_double_sptr>* >(value.ptr());
  vpgl_camera_double_sptr cam = result->value();


  brdb_query_aptr Q_img = brdb_query_comp_new("id", brdb_query::EQ, id_img);
  brdb_selection_sptr S_img = DATABASE->select("vil_image_view_base_sptr_data", Q_img);
  if(S_img->size()!=1){
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

  brdb_value_t<vil_image_view_base_sptr>* result_img = 
    static_cast<brdb_value_t<vil_image_view_base_sptr>* >(value_img.ptr());
  vil_image_view_base_sptr img_out = result_img->value();
  bool saved = vil_save(*img_out, "voxel_image.tif");

  SUMMARY();
}
