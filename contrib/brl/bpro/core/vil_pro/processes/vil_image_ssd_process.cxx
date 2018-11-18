// This is brl/bpro/core/vil_pro/processes/vil_image_ssd_process.cxx
#include <iostream>
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief Compute sum of square differences between two images

#include <vil/vil_math.h>
#include <vil/vil_image_view.h>
#include <vil/vil_convert.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif


//: Constructor
bool vil_image_ssd_process_cons(bprb_func_process& pro)
{
  //this process takes two input images and one output value
  std::vector<std::string> input_types;
  input_types.emplace_back("vil_image_view_base_sptr");
  input_types.emplace_back("vil_image_view_base_sptr");
  std::vector<std::string> output_types;
  output_types.emplace_back("float");  // ssd
  return pro.set_input_types(input_types)
     &&  pro.set_output_types(output_types);
}

//: Execute the process
bool vil_image_ssd_process(bprb_func_process& pro)
{
  // Sanity check
  if (pro.n_inputs()< 2) {
    std::cout << "vil_image_ssd_process: The number of inputs should be 2" << std::endl;
    return false;
  }

  // get the inputs
  unsigned i=0;
  vil_image_view_base_sptr img_ptr_a = pro.get_input<vil_image_view_base_sptr>(i++);
  vil_image_view_base_sptr img_ptr_b = pro.get_input<vil_image_view_base_sptr>(i++);

  // for now assume inputs are byte images
  vil_image_view<vxl_byte> view_a = *(vil_convert_cast(vxl_byte(), img_ptr_a));
  vil_image_view<vxl_byte> view_b = *(vil_convert_cast(vxl_byte(), img_ptr_b));

  float dummy = 0.0f;
  float result = vil_math_ssd(view_a, view_b, dummy);

  pro.set_output_val<float>(0, result);

  return true;
}
