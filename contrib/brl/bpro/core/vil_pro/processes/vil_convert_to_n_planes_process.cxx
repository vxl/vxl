// This is brl/bpro/core/vil_pro/processes/vil_convert_to_n_planes_process.cxx
#include <bprb/bprb_func_process.h>
//:
// \file

#include <bprb/bprb_parameters.h>
#include <vil/vil_convert.h>
#include <vil/vil_image_view_base.h>

//: Constructor
bool vil_convert_to_n_planes_process_cons(bprb_func_process& pro)
{
  //this process takes two inputs:
  // input(0): the input image vil_image_view_base_sptr
  // input(1): the number of planes in the output image 
  bool ok=false;
  vcl_vector<vcl_string> input_types;
  input_types.push_back("vil_image_view_base_sptr"); 
  input_types.push_back("unsigned"); 
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;

  //this process has 1 outputs
  // output(0): the output image with the specified number of planes
  vcl_vector<vcl_string> output_types;
  output_types.push_back("vil_image_view_base_sptr");  // label image
  ok = pro.set_output_types(output_types);
  if (!ok) return ok;
  return true;
}


//: Execute the process
bool vil_convert_to_n_planes_process(bprb_func_process& pro)
{
  // Sanity check
  if (pro.n_inputs()< 2) {
    vcl_cout << "vil_convert_to_n_planes_process: The input number should be 2" << vcl_endl;
    return false;
  }

  // get the inputs
  unsigned i=0;
  //Retrieve image from input
  vil_image_view_base_sptr img = pro.get_input<vil_image_view_base_sptr>(i++);
  unsigned nplanes = pro.get_input<unsigned>(i++);

  vil_image_view_base_sptr out_img_ptr = vil_convert_to_n_planes(nplanes, img);

  pro.set_output_val<vil_image_view_base_sptr>(0, out_img_ptr);
  return true;
}

