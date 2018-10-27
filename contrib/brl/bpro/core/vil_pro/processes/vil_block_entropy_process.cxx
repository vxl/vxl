// This is brl/bpro/core/vil_pro/processes/vil_block_entropy_process.cxx
#include <bprb/bprb_func_process.h>
//:
// \file

#include <bprb/bprb_parameters.h>
#include <vil/vil_image_view.h>
#include <vil/vil_new.h>
#include <brip/brip_vil_float_ops.h>

//: Constructor
bool vil_block_entropy_process_cons(bprb_func_process& pro)
{
  //input
  bool ok=false;
  std::vector<std::string> input_types;
  input_types.emplace_back("vil_image_view_base_sptr");  //: original image
  input_types.emplace_back("unsigned"); // block size
  input_types.emplace_back("unsigned"); // number of bins
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;

  //output
  std::vector<std::string> output_types;
  output_types.emplace_back("vil_image_view_base_sptr");  // output image
  ok = pro.set_output_types(output_types);
  if (!ok) return ok;
  return true;
}

//: Execute the process
bool vil_block_entropy_process(bprb_func_process& pro)
{
  // Sanity check
  if (pro.n_inputs()< 1) {
    std::cout << "vil_block_entropy_process: The input number should be 1" << std::endl;
    return false;
  }

  unsigned i=0;
  vil_image_view_base_sptr image = pro.get_input<vil_image_view_base_sptr>(i++);
  auto size = pro.get_input<unsigned>(i++);
  auto bins = pro.get_input<unsigned>(i++);

  //int medfilt_halfsize = 1;
  vil_image_resource_sptr image_res = vil_new_image_resource_of_view(*image);
  //float sigma = 1.0f;
  float sigma = 0.01f;
  vil_image_view<float> out_img = brip_vil_float_ops::entropy(size, size, 1, image_res, sigma, bins, true, false, false);

  pro.set_output_val<vil_image_view_base_sptr>(0, new vil_image_view<float>(out_img));
  return true;
}
