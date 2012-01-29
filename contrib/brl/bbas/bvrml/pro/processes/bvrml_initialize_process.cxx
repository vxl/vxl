//This is brl/bbas/bvrml/pro/processes/bvrml_initialize_process.cxx
//:
// \file
//   initialize a vrml file
//
#include <bprb/bprb_func_process.h>
#include <bprb/bprb_parameters.h>

#include <vcl_string.h>
#ifdef DEBUG
#include <vcl_iostream.h>
#endif

#include <brdb/brdb_value.h>
#include <bvrml/bvrml_write.h>

//: sets input and output types
bool bvrml_initialize_process_cons(bprb_func_process& pro)
{
  //inputs
  vcl_vector<vcl_string> input_types_(1);
  input_types_[0] = "vcl_string";

  //output
  vcl_vector<vcl_string> output_types_(0);
  
  bool good = pro.set_input_types(input_types_) &&
              pro.set_output_types(output_types_);
  return good;
}

bool bvrml_initialize_process(bprb_func_process& pro)
{
  // check number of inputs
  if (!pro.verify_inputs())
  {
    vcl_cout << pro.name() << ": Invalid inputs" << vcl_endl;
    return false;
  }

  //get the inputs
  vcl_string fname = pro.get_input<vcl_string>(0);
  vcl_ofstream ofs(fname.c_str());
  bvrml_write::write_vrml_header(ofs);
  vcl_cout << "written to " << fname << vcl_endl;
  ofs.close();
  return true;
}

