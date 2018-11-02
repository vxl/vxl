#include <iostream>
#include <testlib/testlib_test.h>
#include <brdb/brdb_relation.h>
#include <brdb/brdb_query.h>
#include <brdb/brdb_tuple.h>
#include <brdb/brdb_selection.h>
#include <brdb/brdb_database_manager.h>
#include <brdb/brdb_value.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include "bprb_test_process.h"
#include <bprb/bprb_parameters.h>
#include <bprb/bprb_macros.h>

void test_process_params()
{
  REG_PROCESS(bprb_test_process, bprb_batch_process_manager);
  REGISTER_DATATYPE(float);

  // set the inputs
  brdb_value_sptr v0 = new brdb_value_t<float>(1.0f);
  brdb_value_sptr v1 = new brdb_value_t<float>(2.0f);
  bool good = bprb_batch_process_manager::instance()->init_process("Process");
  good &= bprb_batch_process_manager::instance()->set_params("params.xml");
  good = good && bprb_batch_process_manager::instance()->set_input(0, v0);
  good = good && bprb_batch_process_manager::instance()->set_input(1, v1);
  good = good && bprb_batch_process_manager::instance()->run_process();

  unsigned id;
  good = good && bprb_batch_process_manager::instance()->commit_output(0, id);
  TEST("run process", good ,true);
  //Check if result is in the database
    // query to get the data
  brdb_query_aptr Q = brdb_query_comp_new("id", brdb_query::EQ, id);

  brdb_selection_sptr selec = DATABASE->select("float_data", std::move(Q));
  if (selec->size()!=1) {
    std::cout << "in bprb_batch_process_manager::set_input_from_db(.) -"
             << " no selections\n";
  }

  brdb_value_sptr value;
  if (!selec->get_value(std::string("value"), value)) {
    std::cout << "in bprb_batch_process_manager::set_input_from_db(.) -"
             << " didn't get value\n";
  }
  auto* result =
    static_cast<brdb_value_t<float>* >(value.ptr());
  float rv = result->value();
  TEST_NEAR("test result in DB", rv, 13.0f, 0.01);
}

TESTMAIN(test_process_params);
