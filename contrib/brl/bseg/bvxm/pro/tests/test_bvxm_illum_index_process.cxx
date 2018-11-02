//:
// \file
// \brief  Test for getting a bin number from an illumination direction
// \author Isabel Restrepo
// \date   February 29, 2008

#include <string>
#include <iostream>
#include <testlib/testlib_test.h>
#include <bvxm/pro/processes/bvxm_illum_index_process.h>
#include <bvxm/bvxm_world_params.h>
#include <bvxm/bvxm_voxel_world.h>

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <brdb/brdb_value.h>
#include <brdb/brdb_selection.h>

#include <bprb/bprb_batch_process_manager.h>
#include <bprb/bprb_parameters.h>
#include <bprb/bprb_macros.h>
#include <bprb/bprb_func_process.h>


static void test_bvxm_illum_index_process()
{
  // This test consists of two parts
  // 1. Run process using the process manager, checking that
  //    parameter, inputs and outputs are being set properly
  // 2. Test the indexing algorithm for various number of bins

  // 1. Run process using Process Manager
  {
    REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bvxm_illum_index_process, "bvxmIllumIndexProcess");
    REGISTER_DATATYPE(unsigned);

    // set the inputs
    brdb_value_sptr v0 = new brdb_value_t<std::string>("eq_area");
    brdb_value_sptr v1 = new brdb_value_t<std::string>("po_39928_pan_0000000_chip_700_700_withICHIPB.nitf");
    brdb_value_sptr v2 = new brdb_value_t<unsigned>(10);
    brdb_value_sptr v3 = new brdb_value_t<unsigned>(0);

    //bgeo_lvcs_sptr lvcs = new bgeo_lvcs(32.724233, -117.156105, 39);

    bool good = bprb_batch_process_manager::instance()->init_process("bvxmIllumIndexProcess");
    good = good && bprb_batch_process_manager::instance()->set_input(0, v0);
    good = good && bprb_batch_process_manager::instance()->set_input(1, v1);
    good = good && bprb_batch_process_manager::instance()->set_input(2, v2);
    good = good && bprb_batch_process_manager::instance()->set_input(3, v3);
    good = good && bprb_batch_process_manager::instance()->run_process();

    unsigned id_bin_idx;
    good = good && bprb_batch_process_manager::instance()->commit_output(0, id_bin_idx);

    TEST("run illum_index_process", good ,true);

    bprb_batch_process_manager::instance()->print_db();

    // check if the results are in DB

    if (good)
    {
      brdb_query_aptr bin_idx_Q = brdb_query_comp_new("id", brdb_query::EQ, id_bin_idx);
      brdb_selection_sptr bin_idx_S = DATABASE->select("unsigned_data", std::move(bin_idx_Q));
      if (bin_idx_S->size()!=1){
        std::cout << "in bprb_batch_process_manager::set_input_from_db(.) -"
                 << " no selections\n";
      }

      brdb_value_sptr value;
      if (!bin_idx_S->get_value(std::string("value"), value)) {
        std::cout << "in bprb_batch_process_manager::set_input_from_db(.) -"
                 << " didn't get value\n";
      }
      bool non_null = (value != nullptr);
      TEST("bin index output non-null", non_null ,true);

      // check that for zero regions choice, the index is zero
      auto* result =
        static_cast<brdb_value_t<unsigned>* >(value.ptr());
      unsigned idx = result->value();

      std::cout<<idx<< std::endl;
      TEST("default index", idx ,0);
    }
  }

  // 2. Check indexing algorithm for a number of choices
  {
    using namespace bvxm_illum_index_process_globals;
    unsigned idx;

    idx = bin_index("eq_area",(180.0/8),0,2,0);
    TEST("bin_idx", idx ,0);

    idx = bin_index("eq_area",(180.0/8),90.0,2,0);
    TEST("bin_idx", idx ,0);

    idx = bin_index("eq_area",(180.0/8),180.0,2,0);
    TEST("bin_idx", idx ,0);

    idx = bin_index("eq_area",(180.0/8),270.0,2,0);
    TEST("bin_idx", idx ,0);

     //----

    idx = bin_index("eq_area",(90.0),0,2,0);
    TEST("bin_idx", idx ,1);

    idx = bin_index("eq_area",(90.0),90.0,2,0);
    TEST("bin_idx", idx ,1);

    idx = bin_index("eq_area",(90.0),180.0,2,0);
    TEST("bin_idx", idx ,1);

    idx = bin_index("eq_area",(90.0),270.0,2,0);
    TEST("bin_idx", idx ,1);

         //----

    idx = bin_index("eq_area",(180.0/8),0,2,2);
    TEST("bin_idx", idx ,0);

    idx = bin_index("eq_area",(180.0/8),90.0,2,2);
    TEST("bin_idx", idx ,0);

    idx = bin_index("eq_area",(180.0/8),180.0,2,2);
    TEST("bin_idx", idx ,1);

    idx = bin_index("eq_area",(180.0/8),270.0,2,2);
    TEST("bin_idx", idx ,1);

         //----

    idx = bin_index("eq_area",90.0,0,2,2);
    TEST("bin_idx", idx ,2);

    idx = bin_index("eq_area",90.0,90.0,2,2);
    TEST("bin_idx", idx ,2);

    idx = bin_index("eq_area",90.0,180.0,2,2);
    TEST("bin_idx", idx ,3);

    idx = bin_index("eq_area",90.0,270.0,2,2);
    TEST("bin_idx", idx ,3);
  }
}

TESTMAIN(test_bvxm_illum_index_process);
