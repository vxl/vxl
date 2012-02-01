// This is brl/bpro/core/brad_pro/processes/brad_save_atmospheric_parameters_process.cxx
#include <bprb/bprb_func_process.h>
#include <brad/brad_atmospheric_parameters.h>
//:
// \file

#include <vcl_fstream.h>

//: Constructor
bool brad_save_atmospheric_parameters_process_cons(bprb_func_process& pro)
{
  //input
  bool ok=false;
  vcl_vector<vcl_string> input_types;
  input_types.push_back("brad_atmospheric_parameters_sptr");
  input_types.push_back("vcl_string");
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;

  //output
  vcl_vector<vcl_string> output_types; // no outputs
  ok = pro.set_output_types(output_types);
  if (!ok) return ok;
  return true;
}

//: Execute the process
bool brad_save_atmospheric_parameters_process(bprb_func_process& pro)
{
  // Sanity check
  if (pro.n_inputs() != 2) {
    vcl_cout << "brad_save_atmospheric_parameters_process: The number of inputs should be 2" << vcl_endl;
    return false;
  }

  // get the inputs
  brad_atmospheric_parameters_sptr atm_params = pro.get_input<brad_atmospheric_parameters_sptr>(0);
  vcl_string out_file = pro.get_input<vcl_string>(1);

  //output file name
  vcl_ofstream os(out_file.c_str());
  if (!os.is_open()) {
    vcl_cout << "in save_atmospheric_parameters_process, couldn't open file : " << out_file << vcl_endl;
    return false;
  }

  os << *atm_params;

  os.close();

  return true;
}

