// This is brl/bpro/core/vil_pro/processes/vil_rgb_to_grey_process.cxx
#include <bprb/bprb_func_process.h>
//:
// \file

#include <bprb/bprb_parameters.h>
#include <vil/vil_convert.h>
#include <vil/vil_image_view_base.h>

//:global variables
namespace vil_rgb_to_grey_process_globals
{
  constexpr unsigned n_inputs_ = 1;
  constexpr unsigned n_outputs_ = 1;
}

//: Constructor
bool vil_rgb_to_grey_process_cons(bprb_func_process& pro)
{
  using namespace vil_rgb_to_grey_process_globals;

  //this process takes one input:
  //input (0): the vil_image_view_base_sptr
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "vil_image_view_base_sptr";

  //this process takes one output:
  //output (0): the vil_image_view_base_sptr
  std::vector<std::string> output_types_(n_outputs_);
  output_types_[0] = "vil_image_view_base_sptr";

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool vil_rgb_to_grey_process(bprb_func_process& pro)
{
  using namespace vil_rgb_to_grey_process_globals;

  if ( pro.n_inputs() < n_inputs_ ){
    std::cout << pro.name() << ": The input number should be " << n_inputs_<< std::endl;
    return false;
  }

  //get the inputs
  vil_image_view_base_sptr image = pro.get_input<vil_image_view_base_sptr>(0);

  if (image->pixel_format() == VIL_PIXEL_FORMAT_BYTE) {
    if (auto *img_view = dynamic_cast<vil_image_view<unsigned char>*>(image.ptr())) {
      if (img_view->nplanes() > 1) {
        auto *img_view_grey = new vil_image_view<vxl_byte>(img_view->ni(),img_view->nj(),1);
        vil_convert_planes_to_grey(*img_view,*img_view_grey);
        pro.set_output_val<vil_image_view_base_sptr>(0,img_view_grey);
        return true;
      }
    }
  }
  else if (image->pixel_format() == VIL_PIXEL_FORMAT_RGB_BYTE) {
    if (auto *img_view_rgb = dynamic_cast<vil_image_view<vil_rgb<unsigned char> >*>(image.ptr())) {
      auto *img_view_grey= new vil_image_view<vxl_byte>(img_view_rgb->ni(),img_view_rgb->nj(),1);
      if (img_view_rgb->nplanes() == 1) {
        vil_image_view<vxl_byte> img_view_plane = vil_view_as_planes(*img_view_rgb);
        vil_convert_planes_to_grey(img_view_plane,*img_view_grey);
        pro.set_output_val<vil_image_view_base_sptr>(0,img_view_grey);
        return true;
      }
    }
  }
  else {
      std::cerr << "Error in vil_rgb_to_grey_process: Unsupported input image\n";
      return false;
  }

  return false;
}
