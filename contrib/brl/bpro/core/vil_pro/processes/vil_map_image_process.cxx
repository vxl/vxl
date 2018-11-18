// This is brl/bpro/core/vil_pro/processes/vil_map_image_process.cxx
#include <iostream>
#include <string>
#include <bprb/bprb_func_process.h>
//:
// \file

#include "../vil_math_functors.h"
#include <vil/vil_convert.h>
#include <bprb/bprb_parameters.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vil/vil_image_view_base.h>
#include <vil/vil_transform.h>
#include <vil/vil_math.h>

//: Constructor
bool vil_map_image_process_cons(bprb_func_process& pro)
{
  //input
  bool ok=false;
  std::vector<std::string> input_types;
  input_types.emplace_back("vil_image_view_base_sptr");
  input_types.emplace_back("vcl_string");
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;

  //output
  std::vector<std::string> output_types;
  output_types.emplace_back("vil_image_view_base_sptr");
  ok = pro.set_output_types(output_types);
  if (!ok) return ok;
  return true;
}

//: Execute the process
bool vil_map_image_process(bprb_func_process& pro)
{
  // Sanity check
  if (pro.n_inputs()< 2) {
    std::cout << "vil_map_image_process: The input number should be 2" << std::endl;
    return false;
  }

  unsigned i=0;
  //Retrieve image from input
  vil_image_view_base_sptr image = pro.get_input<vil_image_view_base_sptr>(i++);

  //Retrieve map functor name
  std::string functor = pro.get_input<std::string>(i++);

  // convert image to float
  vil_image_view_base_sptr fimage = vil_convert_cast(float(), image);
  vil_image_view<float>fimg = *fimage;
  // apply the functor
  if (functor == "log"){
    std::cout << "in log\n";
    vil_math_log_functor lg;
    vil_transform(fimg, lg);
  }
  else if (functor == "not") {
    std::cout << "in not\n";
    vil_math_not_functor nt;
    vil_transform(fimg, nt);
  }
  else if (functor == "log_not") {
    vil_math_log_not_functor nlg;
    vil_transform(fimg, nlg);
  }
  else{
    std::cerr << "In vil_map_image_process::execute() - unknown functor "
             << functor << '\n';
    return false;
  }
  //for now, only handle byte or float outputs
  vil_image_view_base_sptr map_image;
  if (image->pixel_format()==VIL_PIXEL_FORMAT_BYTE)
  {
    auto* temp = new vil_image_view<vxl_byte>;
    vil_convert_stretch_range(fimg, *temp);
    map_image = temp;
  }
  else //leave the image as float
    map_image = fimage;

  pro.set_output_val<vil_image_view_base_sptr>(0, map_image);
  return true;
}
