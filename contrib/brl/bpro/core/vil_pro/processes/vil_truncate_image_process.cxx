// This is brl/bpro/core/vil_pro/processes/vil_truncate_image_process.cxx
#include <bprb/bprb_func_process.h>
//:
// \file

#include "../vil_math_functors.h"
#include <bprb/bprb_parameters.h>
#include <vil/vil_image_view.h>
#include <vil/vil_convert.h>
#include <vil/vil_math.h>

//: Constructor
bool vil_truncate_image_process_cons(bprb_func_process& pro)
{
  //input
  bool ok=false;
  vcl_vector<vcl_string> input_types(3);
  input_types[0] = "vil_image_view_base_sptr";    
  input_types[1] = "float";   // min value
  input_types[2] = "float";   // max value
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;

  //output
  vcl_vector<vcl_string> output_types;
  output_types.push_back("vil_image_view_base_sptr");  // returns a truncated float image!!
  ok = pro.set_output_types(output_types);
  if (!ok) return ok;
  return true;
}

//: Execute the process
bool vil_truncate_image_process(bprb_func_process& pro)
{

   // Sanity check
  if(!pro.verify_inputs()){
    vcl_cerr << "vil_stretch_image_process: Invalid inputs" << vcl_endl;
    return false;
  }

  //Retrieve image from input
  vil_image_view_base_sptr image = pro.get_input<vil_image_view_base_sptr>(0);

  //Retrieve limits
  float min_value = pro.get_input<float>(1);
  float max_value = pro.get_input<float>(2);
  if(min_value>=max_value){
    vcl_cerr << "vil_truncate_image_process: invalid truncate limits\n";
      return false;
  }
    
  // retrieve float image
  vil_image_view_base_sptr fimage = vil_convert_cast(float(), image);
  vil_image_view<float> fimg = *fimage;

  vil_math_truncate_range(fimg, min_value, max_value);

  pro.set_output_val<vil_image_view_base_sptr>(0, fimage);
  return true;
}
