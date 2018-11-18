//This is brl/bbas/bvrml/pro/processes/bvrml_initialize_process.cxx
//:
// \file
//   initialize a vrml file
//
#include <string>
#include <iostream>
#include <bprb/bprb_func_process.h>
#include <bprb/bprb_parameters.h>

#ifdef DEBUG
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#endif

#include <brdb/brdb_value.h>
#include <bvrml/bvrml_write.h>

//: sets input and output types
bool bvrml_initialize_process_cons(bprb_func_process& pro)
{
  //inputs
  std::vector<std::string> input_types_(1);
  input_types_[0] = "vcl_string";

  //output
  std::vector<std::string> output_types_(0);

  bool good = pro.set_input_types(input_types_) &&
              pro.set_output_types(output_types_);
  return good;
}

bool bvrml_initialize_process(bprb_func_process& pro)
{
  // check number of inputs
  if (!pro.verify_inputs())
  {
    std::cout << pro.name() << ": Invalid inputs" << std::endl;
    return false;
  }

  //get the inputs
  std::string fname = pro.get_input<std::string>(0);
  std::ofstream ofs(fname.c_str());
  bvrml_write::write_vrml_header(ofs);
  std::cout << "written to " << fname << std::endl;
  ofs.close();
  return true;
}
