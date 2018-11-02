// This is brl/bpro/core/vil_pro/processes/vil_image_range_process.cxx
#include <iostream>
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief Compute range (min/max) of an image

#include <vil/vil_math.h>
#include <vil/vil_image_view.h>
#include <vil/vil_convert.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif


//: Constructor
bool vil_image_range_process_cons(bprb_func_process& pro)
{
  //this process takes one input: the image
  std::vector<std::string> input_types;
  input_types.emplace_back("vil_image_view_base_sptr");
  std::vector<std::string> output_types;
  output_types.emplace_back("float");  // min
  output_types.emplace_back("float");  // max
  return pro.set_input_types(input_types)
     &&  pro.set_output_types(output_types);
}

//: Execute the process
bool vil_image_range_process(bprb_func_process& pro)
{
  // Sanity check
  if (pro.n_inputs()< 1) {
    std::cout << "vil_image_range_process: The input number should be 1" << std::endl;
    return false;
  }

  // get the input
  unsigned i=0;
  vil_image_view_base_sptr img_ptr_a = pro.get_input<vil_image_view_base_sptr>(i++);

  // for now assume input is a float image
  vil_image_view<float> view_a = *(vil_convert_cast(float(), img_ptr_a));

  float min, max;
  vil_math_value_range(view_a, min, max);
  pro.set_output_val<float>(0, min);
  pro.set_output_val<float>(1, max);
  return true;
}
