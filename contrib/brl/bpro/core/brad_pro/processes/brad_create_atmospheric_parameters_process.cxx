// This is brl/bpro/core/brad_pro/processes/brad_create_atmospheric_parameters_process.cxx
#include <bprb/bprb_func_process.h>
#include <brad/brad_atmospheric_parameters.h>
//:
// \file

#include <vcl_fstream.h>

//: Constructor
bool brad_create_atmospheric_parameters_process_cons(bprb_func_process& pro)
{
  //input
  bool ok=false;
  vcl_vector<vcl_string> input_types;
  input_types.push_back("float");
  input_types.push_back("float");
  input_types.push_back("float");
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;

  //output
  vcl_vector<vcl_string> output_types; 
  output_types.push_back("brad_atmospheric_parameters_sptr"); 
  ok = pro.set_output_types(output_types);
  if (!ok) return ok;
  return true;
}

//: Execute the process
bool brad_create_atmospheric_parameters_process(bprb_func_process& pro)
{
  // Sanity check
  if (pro.n_inputs() != pro.input_types().size()) {
    vcl_cout << "brad_create_atmospheric_parameters_process: The input number should be " << pro.input_types().size() << vcl_endl;
    return false;
  }

  // get the inputs
  float airlight = pro.get_input<float>(0);
  float skylight = pro.get_input<float>(1);
  float optical_depth = pro.get_input<float>(2);

  brad_atmospheric_parameters_sptr metadata = new brad_atmospheric_parameters();
  metadata->airlight_ = airlight;
  metadata->skylight_ = skylight;
  metadata->optical_depth_ = optical_depth;

  pro.set_output_val<brad_atmospheric_parameters_sptr>(0,metadata);

  return true;
}

