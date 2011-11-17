//:
// \file
// \brief  Test for rpc registration process
// \author Gamze D. Tunali
// \date   March 07, 2008
//
#include <testlib/testlib_test.h>
#include <bvxm/bvxm_voxel_world.h>

#include <vcl_string.h>
#include <vcl_iostream.h>

#include <brdb/brdb_value.h>
#include <brdb/brdb_selection.h>

#include <bprb/bprb_batch_process_manager.h>
#include <bprb/bprb_parameters.h>
#include <bprb/bprb_macros.h>
#include <bprb/bprb_func_process.h>

#include <vil/vil_load.h>
#include <vil/vil_save.h>
#include <vil/vil_image_view_base.h>

#include <vpgl/vpgl_rational_camera.h>
#include <vpgl/vpgl_local_rational_camera.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_vector_3d.h>

#include <vul/vul_file.h>

static void test_bvxm_rpc_registration_process()
{
  DECLARE_FUNC_CONS(bvxm_update_edges_process);
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bvxm_update_edges_process,"bvxmUpdateEdgesProcess");

  DECLARE_FUNC_CONS(bvxm_rpc_registration_process);
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bvxm_rpc_registration_process,"bvxmRpcRegistrationProcess");

  REGISTER_DATATYPE(bvxm_voxel_world_sptr);
  REGISTER_DATATYPE(vpgl_camera_double_sptr);
  REGISTER_DATATYPE(vil_image_view_base_sptr);
  REGISTER_DATATYPE(float);

  vgl_vector_3d<unsigned int> num_voxels(50,50,5);
  float voxel_length = 1.0f;
  vpgl_lvcs_sptr lvcs = new vpgl_lvcs(33.3358982058333,44.38220165,0.0);
  bvxm_world_params_sptr voxel_world_params = new bvxm_world_params();
  // create a test directory for intermediate files
  vul_file::make_directory("./rpc_test");
  voxel_world_params->set_params("./rpc_test", vgl_point_3d<float>(-25.0f,-25.0f,-2.5f), num_voxels, voxel_length, lvcs);
  bvxm_voxel_world_sptr voxel_world = new bvxm_voxel_world();
  voxel_world->set_params(voxel_world_params);

  vcl_string png = "./rpc_registration_image.png";
  vil_image_view_base_sptr img = vil_load(png.c_str());

  for (int dummy = 0; dummy == 0; ++dummy)
  {
    vcl_string rpb = "./rpc_registration_camera.rpb";
    vpgl_rational_camera<double>* camera_rational = read_rational_camera<double>(rpb);
    vpgl_camera<double>* camera = new vpgl_local_rational_camera<double>(*lvcs,*camera_rational);

    bool good = bprb_batch_process_manager::instance()->init_process("bvxmUpdateEdgesProcess");
    TEST("bprb_batch_process_manager::instance()->init_process()", good, true);
    if (!good) break;
    vcl_string xml = "./update_edges_parameters.xml";
    good = bprb_batch_process_manager::instance()->set_params(xml);
    TEST("bprb_batch_process_manager::instance()->set_params()", good, true);

    brdb_value_sptr v0 = new brdb_value_t<bvxm_voxel_world_sptr>(voxel_world);
    brdb_value_sptr v1 = new brdb_value_t<vpgl_camera_double_sptr>(camera);
    brdb_value_sptr v2 = new brdb_value_t<vil_image_view_base_sptr>(img);
    brdb_value_sptr v3 = new brdb_value_t<float>(0.0);
    brdb_value_sptr v4 = new brdb_value_t<unsigned>(0);

    good = bprb_batch_process_manager::instance()->set_input(0, v0)
        && bprb_batch_process_manager::instance()->set_input(1, v1)
        && bprb_batch_process_manager::instance()->set_input(2, v2)
        && bprb_batch_process_manager::instance()->set_input(3, v3)
        && bprb_batch_process_manager::instance()->set_input(4, v4);

    TEST("bprb_batch_process_manager::instance()->set_input()", good, true);
    good = bprb_batch_process_manager::instance()->run_process();
    TEST("run bvxm_update_edges_process", good ,true);
    if (!good) break;

    unsigned id_n_normal;
    good = bprb_batch_process_manager::instance()->commit_output(0, id_n_normal);
    TEST("bprb_batch_process_manager::instance()->commit_output()", good, true);
    if (!good) break;

    // check if the results are in DB
    brdb_query_aptr Q = brdb_query_comp_new("id", brdb_query::EQ, id_n_normal);
    brdb_selection_sptr S = DATABASE->select("float_data", Q);
    if (S->size()!=1) {
      vcl_cout << "in bprb_batch_process_manager::set_input_from_db(.) - no selections\n";
    }

    brdb_value_sptr value_n_normal;
    if (!S->get_value(vcl_string("value"), value_n_normal)) {
      vcl_cout << "in bprb_batch_process_manager::set_input_from_db(.) -"
               << " didn't get value\n";
    }
    TEST("float non-null", value_n_normal>0, true);
  }

  for (int dummy = 0; dummy == 0; ++dummy)
  {
    vcl_string rpb = "./rpc_registration_camera.rpb";
    vpgl_rational_camera<double>* camera_rational = read_rational_camera<double>(rpb);
    vpgl_camera<double>* camera = new vpgl_local_rational_camera<double>(*lvcs,*camera_rational);

    bool good = bprb_batch_process_manager::instance()->init_process("bvxmRpcRegistrationProcess");
    TEST("bprb_batch_process_manager::instance()->init_process()", good, true);
    if (!good) break;

    brdb_value_sptr v0 = new brdb_value_t<bvxm_voxel_world_sptr>(voxel_world);
    brdb_value_sptr v1 = new brdb_value_t<vpgl_camera_double_sptr>(camera);
    brdb_value_sptr v2 = new brdb_value_t<vil_image_view_base_sptr>(img);
    brdb_value_sptr v3 = new brdb_value_t<bool>(false);
    brdb_value_sptr v4 = new brdb_value_t<float>(10.0);
    brdb_value_sptr v5 = new brdb_value_t<float>(0.5);
    brdb_value_sptr v6 = new brdb_value_t<unsigned>(0);

    good = bprb_batch_process_manager::instance()->set_input(0, v0)
        && bprb_batch_process_manager::instance()->set_input(1, v1)
        && bprb_batch_process_manager::instance()->set_input(2, v2)
        && bprb_batch_process_manager::instance()->set_input(3, v3)
        && bprb_batch_process_manager::instance()->set_input(4, v4)
        && bprb_batch_process_manager::instance()->set_input(5, v5)
        && bprb_batch_process_manager::instance()->set_input(6, v6);

    TEST("bprb_batch_process_manager::instance()->set_input()", good, true);
    good = bprb_batch_process_manager::instance()->run_process();
    TEST("run bvxm_rpc_registration_process", good ,true);
    if (!good) break;

    unsigned id_cam, id_expected_edge_img;
    good = bprb_batch_process_manager::instance()->commit_output(0, id_cam)
        && bprb_batch_process_manager::instance()->commit_output(1, id_expected_edge_img);
    TEST("bprb_batch_process_manager::instance()->commit_output()", good, true);
    if (!good) break;

    // check if the results are in DB
    brdb_query_aptr Q = brdb_query_comp_new("id", brdb_query::EQ, id_cam);
    brdb_selection_sptr S = DATABASE->select("vpgl_camera_double_sptr_data", Q);
    if (S->size()!=1) {
      vcl_cout << "in bprb_batch_process_manager::set_input_from_db(.) - no selections\n";
    }

    brdb_value_sptr value;
    if (!S->get_value(vcl_string("value"), value)) {
      vcl_cout << "in bprb_batch_process_manager::set_input_from_db(.) -"
               << " didn't get value\n";
    }
    TEST("vpgl_camera_double_sptr non-null", value>0, true);

    brdb_value_t<vpgl_camera_double_sptr>* result =
      static_cast<brdb_value_t<vpgl_camera_double_sptr>* >(value.ptr());
    vpgl_camera_double_sptr cam = result->value();

    brdb_query_aptr Q_expected_edge_img = brdb_query_comp_new("id", brdb_query::EQ, id_expected_edge_img);
    brdb_selection_sptr S_expected_edge_img = DATABASE->select("vil_image_view_base_sptr_data", Q_expected_edge_img);
    if (S_expected_edge_img->size()!=1) {
      vcl_cout << "in bprb_batch_process_manager::set_input_from_db(.) -"
               << " no selections\n";
    }

    brdb_value_sptr value_expected_edge_img;
    if (!S_expected_edge_img->get_value(vcl_string("value"), value_expected_edge_img)) {
      vcl_cout << "in bprb_batch_process_manager::set_input_from_db(.) -"
               << " didn't get value\n";
    }
    TEST("image output non-null", value_expected_edge_img>0, true);

    brdb_value_t<vil_image_view_base_sptr>* result_expected_edge_img =
      static_cast<brdb_value_t<vil_image_view_base_sptr>* >(value_expected_edge_img.ptr());
    vil_image_view_base_sptr expected_edge_img_out = result_expected_edge_img->value();
    bool saved = vil_save(*expected_edge_img_out, "expected_edge_image.tif");
    TEST("image saved", saved ,true);
  }
}

TESTMAIN(test_bvxm_rpc_registration_process);
