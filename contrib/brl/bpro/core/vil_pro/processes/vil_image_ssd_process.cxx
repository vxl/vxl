// This is brl/bpro/core/vil_pro/processes/vil_image_ssd_process.cxx
#include <bprb/bprb_func_process.h>
//:
// \file Compute sum of square images between two images

#include <vil/vil_math.h>
#include <vil/vil_image_view.h>
#include <vil/vil_convert.h>
#include <vcl_iostream.h>


//: Constructor
bool vil_image_ssd_process_cons(bprb_func_process& pro)
{
  //this process takes one input: the filename
  bool ok=false;
  vcl_vector<vcl_string> input_types;
  input_types.push_back("vil_image_view_base_sptr"); 
  input_types.push_back("vil_image_view_base_sptr"); 
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;
  
  vcl_vector<vcl_string> output_types;
  output_types.push_back("float");  // ssd
  ok = pro.set_output_types(output_types);
  if (!ok) return ok;
  
  return true;
}

//: Execute the process
bool vil_image_ssd_process(bprb_func_process& pro)
{
  // Sanity check
  if (pro.n_inputs()< 2) {
    vcl_cout << "vil_image_ssd_process: The input number should be 2" << vcl_endl;
    return false;
  }
  
  // get the inputs
  unsigned i=0;
  vil_image_view_base_sptr img_ptr_a = pro.get_input<vil_image_view_base_sptr>(i++);
  vil_image_view_base_sptr img_ptr_b = pro.get_input<vil_image_view_base_sptr>(i++);
  
  // for now assume inputs are byte images
  vil_image_view<vxl_byte> view_a = *(vil_convert_cast(vxl_byte(), img_ptr_a));
  vil_image_view<vxl_byte> view_b = *(vil_convert_cast(vxl_byte(), img_ptr_b));
  
  float dummy;
  float result = vil_math_ssd(view_a, view_b, dummy );
  
  pro.set_output_val<float>(0, result);
  
  return true;
}




