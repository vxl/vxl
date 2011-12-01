// This is brl/bpro/core/vil_pro/processes/vil_image_mean_process.cxx
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief Compute mean value in an image

#include <vil/vil_math.h>
#include <vil/vil_image_view.h>
#include <vil/vil_convert.h>
#include <vcl_iostream.h>


//: Constructor
bool vil_image_mean_process_cons(bprb_func_process& pro)
{
  //this process takes one input: the image
  vcl_vector<vcl_string> input_types;
  input_types.push_back("vil_image_view_base_sptr");
  vcl_vector<vcl_string> output_types;
  output_types.push_back("float");  // mean
  return pro.set_input_types(input_types)
     &&  pro.set_output_types(output_types);
}

//: Execute the process
bool vil_image_mean_process(bprb_func_process& pro)
{
  // Sanity check
  if (pro.n_inputs()< 1) {
    vcl_cout << "vil_image_mean_process: The input number should be 1" << vcl_endl;
    return false;
  }

  // get the input
  unsigned i=0;
  vil_image_view_base_sptr img_ptr_a = pro.get_input<vil_image_view_base_sptr>(i++);

  // for now assume input is a float image
  vil_image_view<float> view_a = *(vil_convert_cast(float(), img_ptr_a));

  float mean= 0.0f;
  vil_math_mean(mean, view_a, 0);
  pro.set_output_val<float>(0, mean);

  return true;
}

