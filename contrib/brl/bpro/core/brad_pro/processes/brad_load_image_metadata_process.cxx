// This is brl/bpro/core/brad_pro/processes/brad_load_image_metadata_process.cxx
#include <bprb/bprb_func_process.h>
#include <brad/brad_image_metadata.h>
//:
// \file

#include <vcl_fstream.h>

//: Constructor
bool brad_load_image_metadata_process_cons(bprb_func_process& pro)
{
  //input
  bool ok=false;
  vcl_vector<vcl_string> input_types;
  input_types.push_back("vcl_string");
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;

  //output
  vcl_vector<vcl_string> output_types; 
  output_types.push_back("brad_image_metadata_sptr"); 
  ok = pro.set_output_types(output_types);
  if (!ok) return ok;
  return true;
}

//: Execute the process
bool brad_load_image_metadata_process(bprb_func_process& pro)
{
  // Sanity check
  if (pro.n_inputs() != 1) {
    vcl_cout << "brad_load_image_metadata_process: The input number should be 1" << vcl_endl;
    return false;
  }

  // get the input
  vcl_string filename = pro.get_input<vcl_string>(0);

  brad_image_metadata_sptr metadata = new brad_image_metadata();

  vcl_ifstream ifs(filename.c_str());
  if (!ifs.is_open()) {
    vcl_cout << "in load_image_metadata_process, couldn't open file : " << filename << vcl_endl;
    return false;
  }

  ifs >> *metadata;

  ifs.close();

  pro.set_output_val<brad_image_metadata_sptr>(0,metadata);

  return true;
}

