// This is brl/bpro/core/vil_pro/processes/vil_binary_image_op_process.cxx
#include <bprb/bprb_func_process.h>
//:
// \file

#include <vil/vil_math.h>
#include <vil/vil_image_view.h>
#include <vil/vil_convert.h>
#include <vcl_iostream.h>


//: Constructor
bool vil_binary_image_op_process_cons(bprb_func_process& pro)
{
  //this process takes three inputs and has one output
  vcl_vector<vcl_string> input_types;
  input_types.push_back("vil_image_view_base_sptr"); 
  input_types.push_back("vil_image_view_base_sptr"); 
  input_types.push_back("vcl_string"); 

  vcl_vector<vcl_string> output_types;
  output_types.push_back("vil_image_view_base_sptr");  // label image

  return pro.set_input_types(input_types)
      && pro.set_output_types(output_types);
}

//: Execute the process
bool vil_binary_image_op_process(bprb_func_process& pro)
{
  // Sanity check
  if (pro.n_inputs()< 3) {
    vcl_cout << "vil_binary_image_op_process: The number of inputs should be 3" << vcl_endl;
    return false;
  }

  // get the inputs
  unsigned i=0;
  vil_image_view_base_sptr img_ptr_a = pro.get_input<vil_image_view_base_sptr>(i++);
  vil_image_view_base_sptr img_ptr_b = pro.get_input<vil_image_view_base_sptr>(i++);
  vcl_string operation = pro.get_input<vcl_string>(i++);

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
  else
    {
    vcl_cerr << "In vil_binary_image_op_process::execute() -"
             << " unknown binary operation\n";
    return false;
    }

  pro.set_output_val<vil_image_view_base_sptr>(0, new vil_image_view<float>(result));
  return true;
}

