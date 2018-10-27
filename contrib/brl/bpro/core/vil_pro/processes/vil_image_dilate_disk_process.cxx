//this is brl/bpro/core/vil_pro/processes/vil_image_dialate_disk_process.cxx
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief Dilate a binary image using a disk structural element

#include<vil/algo/vil_structuring_element.h>
#include<vil/algo/vil_binary_dilate.h>
#include<vil/vil_image_view.h>

namespace vil_image_dilate_disk_process_globals
{
    constexpr unsigned int n_inputs_ = 2;
    constexpr unsigned int n_outputs_ = 1;
}

bool vil_image_dilate_disk_process_cons(bprb_func_process& pro)
{
    using namespace vil_image_dilate_disk_process_globals;

    std::vector<std::string> input_types_(n_inputs_);
    std::vector<std::string> output_types_(n_outputs_);

    unsigned i = 0;
    input_types_[i++] = "vil_image_view_base_sptr";//original binary image
    input_types_[i++] = "float";//disk radius

    output_types_[0] = "vil_image_view_base_sptr";//output image

    if ( !pro.set_input_types(input_types_) )
        return false;

    if ( !pro.set_output_types(output_types_) )
        return false;

    //default value
    pro.set_input(1, brdb_value_sptr(new brdb_value_t<float>(1.0)));

    return true;
}

bool vil_image_dilate_disk_process(bprb_func_process& pro)
{
    using namespace vil_image_dilate_disk_process_globals;
    if (!pro.verify_inputs()) {
      std::cerr << pro.name() << ": Wrong Inputs!\n";
      return false;
    }

    unsigned i = 0;
    vil_image_view_base_sptr imgPtr
                 = pro.get_input<vil_image_view_base_sptr>(i++);
    auto radius = pro.get_input<float>(i++);

    if ( imgPtr->nplanes() > 1 )
    {
        std::cerr << pro.name()
                 << ":\tInput image must be a single plane.\n";
        return false;
    }

    // load the image
    auto* inView = dynamic_cast<vil_image_view<bool>*>(imgPtr.ptr());
    vil_image_view_base_sptr img_bool;
    if (!inView) { // try to generate a Boolean image from a byte image that has 0 and 255
      vil_image_view<bool> temp(imgPtr->ni(), imgPtr->nj());
      temp.fill(false);
      auto* in_img_byte = dynamic_cast<vil_image_view<vxl_byte>*>(imgPtr.ptr());
      if (!in_img_byte) {
        std::cerr << pro.name() << ": Unsupported image pixel format -- " << imgPtr->pixel_format() << ", only Boolean and Byte (0 and 255) are supported!\n";
        return false;
      }
      for (unsigned i = 0; i < imgPtr->ni(); i++) {
        for (unsigned j = 0; j < imgPtr->nj(); j++) {
          if ((*in_img_byte)(i,j) == 255)
            temp(i,j) = true;
        }
      }
      img_bool = new vil_image_view<bool>(temp);
      inView = dynamic_cast<vil_image_view<bool>*>(img_bool.ptr());
    }

    auto* oView = new vil_image_view<bool>(imgPtr->ni(), imgPtr->nj(), 1);

    vil_structuring_element disk;
    disk.set_to_disk(radius);

    vil_binary_dilate(*inView,*oView,disk);

    pro.set_output_val<vil_image_view_base_sptr>(0, oView);

    return true;
}
