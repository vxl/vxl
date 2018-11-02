
// This is brl/bpro/core/bbas_pro/processes/bbas_remove_from_db_process.cxx
#include <iostream>
#include <fstream>
#include <bprb/bprb_func_process.h>
#include <bprb/bprb_batch_process_manager.h>
//:
// \file

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: Constructor
bool bbas_remove_from_db_process_cons(bprb_func_process& pro)
{
  //input
  bool ok=false;
  std::vector<std::string> input_types;
  input_types.emplace_back("unsigned"); // databse id to be removed
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;

  //output
  std::vector<std::string> output_types; // no outputs
  ok = pro.set_output_types(output_types);
  if (!ok) return ok;
  return true;
}

//: Execute the process
bool bbas_remove_from_db_process(bprb_func_process& pro)
{
  // Sanity check
  if (pro.n_inputs()<1) {
    std::cout << "bbas_remove_from_db_process: The input number should be 1" << std::endl;
    return false;
  }
  // get the inputs
  auto db_id = pro.get_input<unsigned>(0);
  bprb_batch_process_manager::instance()->remove_data(db_id);
  return true;
}
