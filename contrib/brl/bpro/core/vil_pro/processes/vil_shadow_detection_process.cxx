// This is brl/bpro/core/vil_pro/processes/vil_shadow_detection_process.cxx
#include <bprb/bprb_func_process.h>
//:
// \file

#include <bprb/bprb_parameters.h>
#include <vil/vil_image_view.h>
#include <vil/vil_image_view_base.h>
#include <vil/algo/vil_binary_closing.h>
#include <vil/algo/vil_binary_opening.h>
#include <vil/algo/vil_blob.h>
#include <brip/brip_vil_float_ops.h>

namespace vil_shadow_detection_process_globals
{
  constexpr unsigned n_inputs_ = 2;
  constexpr unsigned n_outputs_ = 1;
}
//: Init function
bool vil_shadow_detection_process_cons(bprb_func_process& pro)
{
  using namespace vil_shadow_detection_process_globals;

  //process takes 2 inputs
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "vil_image_view_base_sptr";
  input_types_[1] = "float"; // threshold

  // process has 1 output:
  std::vector<std::string>  output_types_(n_outputs_);
  output_types_[0] = "vil_image_view_base_sptr"; //X

  return pro.set_input_types(input_types_) &&
         pro.set_output_types(output_types_);
}

//: Execute the process
bool vil_shadow_detection_process(bprb_func_process& pro)
{
  using namespace vil_shadow_detection_process_globals;
  // Sanity check
  if (pro.n_inputs()< 2) {
    std::cout << "vil_shadow_detection_process: The number of inputs should be 2" << std::endl;
    return false;
  }
  // get the inputs
  unsigned i=0;
  vil_image_view_base_sptr in_img = pro.get_input<vil_image_view_base_sptr>(i++);
  auto threshold = pro.get_input<float>(i++);

  if (in_img->nplanes() != 3)
  {
    std::cout<<"Input needs to be a color image" << std::endl;
    return false;
  }
  auto * out_img = new vil_image_view<bool>(in_img->ni(),in_img->nj());
  auto * I = new vil_image_view<float>(in_img->ni(),in_img->nj());
  if (auto * byte_image = dynamic_cast<vil_image_view<vxl_byte>* > (in_img.ptr()))
  {
    vil_image_view<float> H,S;
    brip_vil_float_ops::convert_to_IHS(*byte_image,*I,H,S);
    for (unsigned i = 0 ; i < I->ni(); i++)
    {
      for (unsigned j = 0 ; j < I->nj(); j++)
      {
        if ( (1+H(i,j))/(1+(*I)(i,j)) > threshold )
          (*out_img)(i,j) = true;
      }
    }
  }
  pro.set_output_val<vil_image_view_base_sptr>(0, out_img);
  return true;
}
