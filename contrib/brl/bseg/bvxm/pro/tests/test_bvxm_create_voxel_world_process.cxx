//:
// \file
// \brief  Test for creating voxel world from world model parameters
// \author Gamze D. Tunali
// \date   02/23/2008
//
#include <testlib/testlib_test.h>
#include "../bvxm_create_voxel_world_process.h"
#include <bvxm/bvxm_world_params.h>
#include <bvxm/bvxm_voxel_world.h>

#include <vcl_string.h>
#include <vcl_iostream.h>

#include <brdb/brdb_value.h>
#include <brdb/brdb_selection.h>

#include <bprb/bprb_batch_process_manager.h>
#include <bprb/bprb_parameters.h>
#include <bprb/bprb_macros.h>

#include <vgl/vgl_vector_3d.h>

#include <vul/vul_file.h>

MAIN( test_bvxm_create_voxel_world_process )
{
  REG_PROCESS(bvxm_create_voxel_world_process, bprb_batch_process_manager);
  REGISTER_DATATYPE(bvxm_voxel_world_sptr);

  vcl_string test_dir("./create_world_test");
  vul_file::make_directory(test_dir);

  bool good = bprb_batch_process_manager::instance()->init_process("bvxmCreateVoxelWorldProcess");
  good = good && bprb_batch_process_manager::instance()->set_params("world_model_params.xml");
  good = good && bprb_batch_process_manager::instance()->run_process();

  unsigned id;
  good = good && bprb_batch_process_manager::instance()->commit_output(0, id);
  TEST("run create voxel world process", good ,true);

  // check if the results are in DB
  brdb_query_aptr Q = brdb_query_comp_new("id", brdb_query::EQ, id);
  brdb_selection_sptr S = DATABASE->select("bvxm_voxel_world_sptr_data", Q);
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
  TEST("bvxm_voxel_world_sptr non-null", non_null ,true);

  brdb_value_t<bvxm_voxel_world_sptr>* result =
    static_cast<brdb_value_t<bvxm_voxel_world_sptr>* >(value.ptr());

  // compare the values with the params given
  bvxm_voxel_world_sptr voxel_world = result->value();
  bvxm_world_params_sptr params = voxel_world->get_params();
  vcl_string dir = params->model_dir();

  bool comp = (!dir.compare("./create_world_test"));
  comp = comp && (params->corner() == vgl_point_3d<float>(0,0,0));
  comp = comp && (params->num_voxels() == vgl_vector_3d<unsigned int> (50,50,5));
  comp = comp && (params->voxel_length() == 10);
  comp = comp && (0.0009 < params->min_occupancy_prob()) && (params->min_occupancy_prob() < 0.0011);
  comp = comp && (0.98< params->max_occupancy_prob()) && (params->max_occupancy_prob() < 1.0);
  comp = comp && (params->lvcs());   // non-null lvcs
  // fill out the values in the test lvcs file
  bgeo_lvcs lvcs(32.724233, -117.156105, 39.0, bgeo_lvcs::wgs84, 0.1, 0.1);
  comp = comp && (*(params->lvcs()) == lvcs);
  TEST("world parameters check", comp ,true);

  SUMMARY();
}
