// This is brl/bpro/core/vil_pro/processes/vil_debayer_BGGR_to_RGB_process.cxx
#include <bprb/bprb_func_process.h>
//:
// \file

#include <bprb/bprb_parameters.h>
#include <vil/vil_image_view.h>
#include <vil/vil_rgb.h>
#include<bil/algo/bil_debayer_image.h>
//: Constructor
// this process takes 1 input:
// Debayer the image ( GRBG) to RGB
namespace vil_debayer_BGGR_to_RGB_process_globals
{
  constexpr unsigned n_inputs_ = 1;
  constexpr unsigned n_outputs_ = 1;
}

bool vil_debayer_BGGR_to_RGB_process_cons(bprb_func_process& pro)
{
  using namespace vil_debayer_BGGR_to_RGB_process_globals;

  //process takes 1 input
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "vil_image_view_base_sptr";   //scene to operate on

  // process has 1 output:
  std::vector<std::string>  output_types_(n_outputs_);
  output_types_[0] = "vil_image_view_base_sptr";   //scene to operate on

  return pro.set_input_types(input_types_)
      && pro.set_output_types(output_types_);
}
//: Execute the process
bool vil_debayer_BGGR_to_RGB_process(bprb_func_process& pro)
{
  // Sanity check
  if (pro.n_inputs()< 1) {
    std::cout << "vil_debayer_BGGR_to_RGB_process: The number of inputs should be 1" << std::endl;
    return false;
  }

  // get the inputs
  unsigned i=0;
  //Retrieve image from input
  vil_image_view_base_sptr in_img = pro.get_input<vil_image_view_base_sptr>(i++);
  auto * debayer_img = new vil_image_view<vil_rgb<vxl_byte> >(in_img->ni(),in_img->nj());
  bil_debayer_image::bil_debayer_BGGR(in_img,debayer_img);
  //bil_debayer_image::bil_debayer_GRBG(in_img,debayer_img);
  pro.set_output_val<vil_image_view_base_sptr>(0, debayer_img);
  return true;
}
