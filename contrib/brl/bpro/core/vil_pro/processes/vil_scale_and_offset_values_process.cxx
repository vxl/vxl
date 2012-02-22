// This is brl/bpro/core/vil_pro/processes/vil_scale_and_offset_values_process.cxx
#include <bprb/bprb_func_process.h>
//:
// \file

#include <bprb/bprb_parameters.h>
#include <vil/vil_image_view.h>
#include <vil/vil_math.h>

//: Constructor
bool vil_scale_and_offset_values_process_cons(bprb_func_process& pro)
{
  //input
  bool ok=false;
  vcl_vector<vcl_string> input_types;
  input_types.push_back("vil_image_view_base_sptr");  //: image
  input_types.push_back("float");  // scale to apply to image
  input_types.push_back("float");  // offset to apply to image
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;

  // apply scale and offset in place: no output
  vcl_vector<vcl_string> output_types;
  ok = pro.set_output_types(output_types);
  if (!ok) return ok;
  return true;
}

//: Execute the process
bool vil_scale_and_offset_values_process(bprb_func_process& pro)
{
  // Sanity check
  if (pro.n_inputs()< pro.input_types().size()) {
    vcl_cout << "vil_scale_and_offset_values_process: The input number should be "<< pro.input_types().size() << vcl_endl;
    return false;
  }

  //Retrieve image from input
  vil_image_view_base_sptr image = pro.get_input<vil_image_view_base_sptr>(0);
  float scale = pro.get_input<float>(1);
  float offset = pro.get_input<float>(2);

  // cast to float
  vil_image_view<float> *image_float = dynamic_cast<vil_image_view<float>*>(image.ptr());
  if (!image_float) {
     vcl_cerr << "ERROR: vil_scale_and_offset_values_process: expecting vil_image_view<float> " << vcl_endl;
     return false;
  }

  vil_math_scale_and_offset_values(*image_float, scale, offset);

  return true;
}

