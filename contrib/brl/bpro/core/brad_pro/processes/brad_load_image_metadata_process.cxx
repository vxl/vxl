// This is brl/bpro/core/brad_pro/processes/brad_load_image_metadata_process.cxx
#include <iostream>
#include <fstream>
#include <bprb/bprb_func_process.h>
#include <brad/brad_image_metadata.h>
//:
// \file

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: Constructor
bool brad_load_image_metadata_process_cons(bprb_func_process& pro)
{
  //input
  bool ok=false;
  std::vector<std::string> input_types;
  input_types.emplace_back("vcl_string");
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;

  //output
  std::vector<std::string> output_types;
  output_types.emplace_back("brad_image_metadata_sptr");
  ok = pro.set_output_types(output_types);
  if (!ok) return ok;
  return true;
}

//: Execute the process
bool brad_load_image_metadata_process(bprb_func_process& pro)
{
  // Sanity check
  if (pro.n_inputs() != 1) {
    std::cout << "brad_load_image_metadata_process: The input number should be 1" << std::endl;
    return false;
  }

  // get the input
  std::string filename = pro.get_input<std::string>(0);

  brad_image_metadata_sptr metadata = new brad_image_metadata();

  std::ifstream ifs(filename.c_str());
  if (!ifs.is_open()) {
    std::cout << "in load_image_metadata_process, couldn't open file : " << filename << std::endl;
    return false;
  }

  ifs >> *metadata;

  ifs.close();

  pro.set_output_val<brad_image_metadata_sptr>(0,metadata);

  return true;
}
