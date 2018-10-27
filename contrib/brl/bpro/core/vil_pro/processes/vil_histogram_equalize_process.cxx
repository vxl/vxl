// This is brl/bpro/core/vil_pro/processes/vil_histogram_equalize_process.cxx
#include <bprb/bprb_func_process.h>
//:
// \file  -- only works for byte images for now

#include <bprb/bprb_parameters.h>
#include <vil/vil_image_view.h>
#include <vil/algo/vil_histogram_equalise.h>

//: Constructor
bool vil_histogram_equalize_process_cons(bprb_func_process& pro)
{
  //input
  bool ok=false;
  std::vector<std::string> input_types(1);
  input_types[0] = "vil_image_view_base_sptr";
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
bool vil_histogram_equalize_process(bprb_func_process& pro)
{
  if(!pro.verify_inputs()){
    std::cerr << "vil_histogram_equalize_process: Invalid inputs" << std::endl;
    return false;
  }

  //Retrieve image from input
  vil_image_view_base_sptr image = pro.get_input<vil_image_view_base_sptr>(0);

  vil_image_view<vxl_byte> img(image);
  vil_image_view_base_sptr out_img = new vil_image_view<vxl_byte> (img.ni(), img.nj(), img.nplanes());
  auto& out_img_ref = static_cast<vil_image_view<vxl_byte> &>(*out_img);

  out_img_ref.deep_copy(img);
  vil_histogram_equalise(out_img_ref);

  pro.set_output_val<vil_image_view_base_sptr>(0, out_img);

  return true;
}
