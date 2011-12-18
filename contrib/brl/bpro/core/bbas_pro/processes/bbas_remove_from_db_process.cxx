
// This is brl/bpro/core/bbas_pro/processes/bbas_remove_from_db_process.cxx
#include <bprb/bprb_func_process.h>
#include <bprb/bprb_batch_process_manager.h>
//:
// \file

#include <vcl_fstream.h>

//: Constructor
bool bbas_remove_from_db_process_cons(bprb_func_process& pro)
{
  //input
  bool ok=false;
  vcl_vector<vcl_string> input_types;
  input_types.push_back("unsigned"); // databse id to be removed
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;

  //output
  vcl_vector<vcl_string> output_types; // no outputs
  ok = pro.set_output_types(output_types);
  if (!ok) return ok;
  return true;
}

//: Execute the process
bool bbas_remove_from_db_process(bprb_func_process& pro)
{
  // Sanity check
  if (pro.n_inputs()<1) {
    vcl_cout << "bbas_remove_from_db_process: The input number should be 1" << vcl_endl;
    return false;
  }
  // get the inputs
  unsigned db_id = pro.get_input<unsigned>(0);
  bprb_batch_process_manager::instance()->remove_data(db_id);
  return true;
}
