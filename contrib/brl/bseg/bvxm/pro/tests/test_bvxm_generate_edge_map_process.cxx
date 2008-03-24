//:
// \file
// \brief  Test for creating edge map from an image
// \author Gamze D. Tunali
// \date   03/06/2008
//
#include <testlib/testlib_test.h>
#include "../bvxm_generate_edge_map_process.h"
#include <bvxm/bvxm_voxel_world.h>

#include <vcl_string.h>
#include <vcl_iostream.h>

#include <brdb/brdb_value.h>
#include <brdb/brdb_selection.h>

#include <bprb/bprb_batch_process_manager.h>
#include <bprb/bprb_parameters.h>
#include <bprb/bprb_macros.h>

#include <vil/vil_load.h>
#include <vil/vil_save.h>
#include <vil/vil_image_view_base.h>

MAIN( test_bvxm_generate_edge_map_process )
{
  REG_PROCESS(bvxm_generate_edge_map_process, bprb_batch_process_manager);
  REGISTER_DATATYPE(vil_image_view_base_sptr);

  // read the image and generate the input
  vil_image_view_base_sptr img = vil_load("two_box_synth.tif");

  bool good = bprb_batch_process_manager::instance()->init_process("bvxmGenerateEdgeMapProcess");
  good = good && bprb_batch_process_manager::instance()->set_params("edge_map_params.xml");

  brdb_value_sptr v0 = new brdb_value_t<vil_image_view_base_sptr>(img);
  good = good && bprb_batch_process_manager::instance()->set_input(0, v0);
  good = good && bprb_batch_process_manager::instance()->run_process();

  unsigned id0;
  good = good && bprb_batch_process_manager::instance()->commit_output(0, id0);
  TEST("run bvxm_generate_edge_map_process", good ,true);

  // check if the results are in DB
  brdb_query_aptr Q1 = brdb_query_comp_new("id", brdb_query::EQ, id0);
  brdb_selection_sptr S1 = DATABASE->select("vil_image_view_base_sptr_data", Q1);
  if (S1->size()!=1){
    vcl_cout << "in bprb_batch_process_manager::set_input_from_db(.) -"
             << " no selections\n";
  }

  brdb_value_sptr value;
  if (!S1->get_value(vcl_string("value"), value)) {
    vcl_cout << "in bprb_batch_process_manager::set_input_from_db(.) -"
             << " didn't get value\n";
  }
  bool non_null = (value != 0);
  TEST("vil_image_view_base_sptr non-null", non_null ,true);

  brdb_value_t<vil_image_view_base_sptr>* result =
    static_cast<brdb_value_t<vil_image_view_base_sptr>* >(value.ptr());
  vil_image_view<unsigned char> img0(result->value());

  vil_save(img0, "res1.tif");

  SUMMARY();
}
