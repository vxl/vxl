// This is brl/bpro/core/vpgl_pro/processes/vgpl_load_lvcs_process.cxx
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief A process for loading vpgl_lvcs from text file
//
// \author Yi Dong
// \date Nov. 11, 2013

#include <bprb/bprb_parameters.h>
#include <vcl_iostream.h>
#include <vpgl/vpgl_lvcs.h>
#include <vpgl/vpgl_lvcs_sptr.h>

//: initialization
bool vpgl_load_lvcs_process_cons(bprb_func_process& pro)
{
  //this process take one input and one output
  vcl_vector<vcl_string> input_types;
  input_types.push_back("vcl_string");  // text file where the lvcs is saved
  vcl_vector<vcl_string> output_types;
  output_types.push_back("vpgl_lvcs_sptr");

  return pro.set_input_types(input_types) && pro.set_output_types(output_types);
}

//: execute the process
bool vpgl_load_lvcs_process(bprb_func_process& pro)
{
  if ( pro.n_inputs() != 1) {
    vcl_cerr << pro.name() << ": The input number should be 1" << vcl_endl;
    return false;
  }

  // get inputs
  vcl_string lvcs_filename = pro.get_input<vcl_string>(0);

  // load lvcs from file
  vpgl_lvcs_sptr lvcs = new vpgl_lvcs;
  vcl_ifstream ifs(lvcs_filename.c_str());
  if (!ifs.good()) {
    vcl_cerr << pro.name() << ": can not open lvcs text file: " << lvcs_filename << vcl_endl;
    return false;
  }

  lvcs->read(ifs);
  ifs.close();
  // store output
  pro.set_output_val<vpgl_lvcs_sptr>(0, lvcs);
  return true;
}

