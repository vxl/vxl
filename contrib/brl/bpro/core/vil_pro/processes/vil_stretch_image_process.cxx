// This is brl/bpro/core/vil_pro/processes/vil_stretch_image_process.cxx
#include <bprb/bprb_func_process.h>
//:
// \file

#include "../vil_math_functors.h"
#include <bprb/bprb_parameters.h>
#include <vil/vil_image_view.h>
#include <vil/vil_convert.h>

//: Constructor
bool vil_stretch_image_process_cons(bprb_func_process& pro)
{
  //input
  bool ok=false;
  vcl_vector<vcl_string> input_types;
  input_types.push_back("vil_image_view_base_sptr");  
  input_types.push_back("float");   // min limit
  input_types.push_back("float");   // max limit
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;

  //output
  vcl_vector<vcl_string> output_types;
  output_types.push_back("vil_image_view_base_sptr");
  ok = pro.set_output_types(output_types);
  if (!ok) return ok;
  return true;
}

//: Execute the process
bool vil_stretch_image_process(bprb_func_process& pro)
{
  // Sanity check
  if (pro.n_inputs()< 3) {
    vcl_cout << "vil_stretch_image_process: The input number should be 3" << vcl_endl;
    return false;
  }

  unsigned i=0;
  //Retrieve image from input
  vil_image_view_base_sptr image = pro.get_input<vil_image_view_base_sptr>(i++);

  //Retrieve limits
  float min_limit = pro.get_input<float>(i++);
  float max_limit = pro.get_input<float>(i++);

  // retrieve float image
  vil_image_view_base_sptr fimage = vil_convert_cast(float(), image);
  vil_image_view<float> fimg = *fimage;

  vil_image_view<vxl_byte>* temp = new vil_image_view<vxl_byte>;
  vil_convert_stretch_range_limited(fimg, *temp, min_limit, max_limit);

  pro.set_output_val<vil_image_view_base_sptr>(0, temp);
  return true;
}

