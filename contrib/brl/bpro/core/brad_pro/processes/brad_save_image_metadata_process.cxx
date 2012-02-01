// This is brl/bpro/core/brad_pro/processes/brad_save_image_metadata_process.cxx
#include <bprb/bprb_func_process.h>
#include <brad/brad_image_metadata.h>
//:
// \file

#include <vcl_fstream.h>

//: Constructor
bool brad_save_image_metadata_process_cons(bprb_func_process& pro)
{
  //input
  bool ok=false;
  vcl_vector<vcl_string> input_types;
  input_types.push_back("brad_image_metadata_sptr");
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
bool brad_save_image_metadata_process(bprb_func_process& pro)
{
  // Sanity check
  if (pro.n_inputs() != 2) {
    vcl_cout << "brad_save_image_metadata_process: The number of inputs should be 2" << vcl_endl;
    return false;
  }

  // get the inputs
  brad_image_metadata_sptr metadata = pro.get_input<brad_image_metadata_sptr>(0);
  vcl_string out_file = pro.get_input<vcl_string>(1);

  //output file name
  vcl_ofstream os(out_file.c_str());
  if (!os.is_open()) {
    vcl_cout << "in save_image_metadata_process, couldn't open output file : " << out_file << vcl_endl;
    return false;
  }

  os << *metadata;

  os.close();

  return true;
}

