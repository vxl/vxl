// This is brl/bpro/core/vil_pro/processes/vil_image_size_process.cxx
#include <bprb/bprb_func_process.h>
//:
// \file

#include <bprb/bprb_parameters.h>
#include <vcl_iostream.h>
#include <vcl_string.h>
#include <vil/vil_load.h>
#include <vil/vil_image_view_base.h>

//: Constructor
bool vil_image_size_process_cons(bprb_func_process& pro)
{
  //input
  bool ok=false;
  vcl_vector<vcl_string> input_types;
  input_types.push_back("vil_image_view_base_sptr"); 
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;

  //output
  vcl_vector<vcl_string> output_types;
  output_types.push_back("unsigned");  // ni of the image
  output_types.push_back("unsigned");  // nj of the image
  ok = pro.set_output_types(output_types);
  if (!ok) return ok;
  return true;
}

//: Execute the process
bool vil_image_size_process(bprb_func_process& pro)
{
  // Sanity check
  if (pro.n_inputs()< 1) {
    vcl_cout << "vil_load_image_view_binary_process: The input number should be 1" << vcl_endl;
    return false;
  }

  // get the inputs
  unsigned i=0;
  //Retrieve image
  vil_image_view_base_sptr image = pro.get_input<vil_image_view_base_sptr>(i++);
  if ( !image ) {
    vcl_cout << "Input image is empty!" << vcl_endl;
    return false;
  }

  i=0;
  pro.set_output_val<unsigned>(i++, image->ni());
  pro.set_output_val<unsigned>(i++, image->nj());
  return true;
}

