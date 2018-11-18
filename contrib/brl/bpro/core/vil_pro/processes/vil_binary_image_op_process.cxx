// This is brl/bpro/core/vil_pro/processes/vil_binary_image_op_process.cxx
#include <iostream>
#include <bprb/bprb_func_process.h>
//:
// \file
// \verbatim
//   Yi Dong --- Oct, 2014  add image difference, image ratio binary operation
// \endverbatim
#include <vil/vil_math.h>
#include <vil/vil_image_view.h>
#include <vil/vil_convert.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif


//: Constructor
bool vil_binary_image_op_process_cons(bprb_func_process& pro)
{
  //this process takes three inputs and has one output
  std::vector<std::string> input_types;
  input_types.emplace_back("vil_image_view_base_sptr");
  input_types.emplace_back("vil_image_view_base_sptr");
  input_types.emplace_back("vcl_string");

  std::vector<std::string> output_types;
  output_types.emplace_back("vil_image_view_base_sptr");  // label image

  return pro.set_input_types(input_types)
      && pro.set_output_types(output_types);
}

//: Execute the process
bool vil_binary_image_op_process(bprb_func_process& pro)
{
  // Sanity check
  if (pro.n_inputs()< 3) {
    std::cout << "vil_binary_image_op_process: The number of inputs should be 3" << std::endl;
    return false;
  }

  // get the inputs
  unsigned i=0;
  vil_image_view_base_sptr img_ptr_a = pro.get_input<vil_image_view_base_sptr>(i++);
  vil_image_view_base_sptr img_ptr_b = pro.get_input<vil_image_view_base_sptr>(i++);
  std::string operation = pro.get_input<std::string>(i++);

  vil_image_view<float> view_a = *(vil_convert_cast(float(), img_ptr_a));
  vil_image_view<float> view_b = *(vil_convert_cast(float(), img_ptr_b));
  vil_image_view<float> result;

  //test for operation
  if (operation=="product")
    vil_math_image_product(view_a, view_b, result);
  else if (operation=="max")
    vil_math_image_max(view_a, view_b, result);
  else if (operation=="sum")
    vil_math_image_sum(view_a, view_b, result);
  else if (operation=="diff")
    vil_math_image_difference(view_a, view_b, result);
  else if (operation=="ratio")
    vil_math_image_ratio(view_a, view_b, result);
  else
    {
    std::cerr << "In vil_binary_image_op_process::execute() -"
             << " unknown binary operation\n";
    return false;
    }

  pro.set_output_val<vil_image_view_base_sptr>(0, new vil_image_view<float>(result));
  return true;
}
