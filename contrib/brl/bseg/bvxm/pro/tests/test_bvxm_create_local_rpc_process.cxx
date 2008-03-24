//:
// \file
// \brief  Test for rpc registration process
// \author Ibrahim Eden
// \date   03/14/2008
//
#include <testlib/testlib_test.h>
#include "../bvxm_create_local_rpc_process.h"
#include <bvxm/bvxm_voxel_world.h>

#include <vcl_string.h>
#include <vcl_iostream.h>

#include <brdb/brdb_value.h>
#include <brdb/brdb_selection.h>

#include <bprb/bprb_batch_process_manager.h>
#include <bprb/bprb_parameters.h>
#include <bprb/bprb_macros.h>

#include <vpgl/vpgl_rational_camera.h>
#include <vpgl/vpgl_local_rational_camera.h>

#include <vgl/vgl_vector_3d.h>

#include <vul/vul_file.h>

MAIN( test_bvxm_create_local_rpc_process )
{
  REG_PROCESS(bvxm_create_local_rpc_process, bprb_batch_process_manager);
  REGISTER_DATATYPE(bvxm_voxel_world_sptr);
  REGISTER_DATATYPE(vpgl_camera_double_sptr);

  vgl_vector_3d<unsigned int> num_voxels(20,20,5);
  float voxel_length = 1.0;
  bgeo_lvcs_sptr lvcs = new bgeo_lvcs(33.3358982058333, 44.38220165, 74.5333333333333);
  bvxm_world_params_sptr voxel_world_params = new bvxm_world_params();
  // create a test directory for intermediate files
  vul_file::make_directory("./local_rpc_test");
  voxel_world_params->set_params("./local_rpc_test", vgl_point_3d<float> (-10.0,-10.0,-2.5), num_voxels, voxel_length, lvcs);
  bvxm_voxel_world_sptr voxel_world = new bvxm_voxel_world();
  voxel_world->set_params(voxel_world_params);

  vpgl_rational_camera<double>* camera = read_rational_camera<double>("rpc_registration_camera.rpb");

  bool good = bprb_batch_process_manager::instance()->init_process("bvxmCreateLocalRpcProcess");

  brdb_value_sptr v0 = new brdb_value_t<bvxm_voxel_world_sptr>(voxel_world);
  brdb_value_sptr v1 = new brdb_value_t<vpgl_camera_double_sptr>(camera);
  good = good && bprb_batch_process_manager::instance()->set_input(0, v0);
  good = good && bprb_batch_process_manager::instance()->set_input(1, v1);
  good = good && bprb_batch_process_manager::instance()->run_process();

  unsigned id_cam;
  good = good && bprb_batch_process_manager::instance()->commit_output(0, id_cam);
  TEST("run bvxm_create_local_rpc_process", good ,true);

  // check if the results are in DB
  brdb_query_aptr Q = brdb_query_comp_new("id", brdb_query::EQ, id_cam);
  brdb_selection_sptr S = DATABASE->select("vpgl_camera_double_sptr_data", Q);
  if (S->size()!=1){
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

  SUMMARY();
}
