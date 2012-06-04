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
    const unsigned int n_inputs_ = 2;
    const unsigned int n_outputs_ = 1;
}

bool vil_image_dilate_disk_process_cons(bprb_func_process& pro)
{
    using namespace vil_image_dilate_disk_process_globals;

    vcl_vector<vcl_string> input_types_(n_inputs_);
    vcl_vector<vcl_string> output_types_(n_outputs_);

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

    if ( pro.n_inputs() < n_inputs_)
    {
        vcl_cerr << "In vil_image_dilate_disk_process(.)\n"
                 << "\tMust provide at least 1 input\n";
        return false;
    }

    unsigned i = 0;
    vil_image_view_base_sptr imgPtr
                 = pro.get_input<vil_image_view_base_sptr>(i++);
    float radius = pro.get_input<float>(i++);

    if ( imgPtr->pixel_format() != VIL_PIXEL_FORMAT_BOOL )
    {
        vcl_cerr << "In vil_image_dilate_disk_process(.)\n"
                 << "\tMust provide boolean image.\n";
        return false;
    }

    if ( imgPtr->nplanes() > 1 )
    {
        vcl_cerr << "In vil_image_dilate_disk_process(.)\n"
                 << "\tInput image must be a single plane.\n";
        return false;
    }
    vil_image_view<bool> inView(imgPtr);

    vil_image_view<bool>* oView = new vil_image_view<bool>(imgPtr->ni(), imgPtr->nj(), 1);

    vil_structuring_element disk;
    disk.set_to_disk(radius);

    vil_binary_dilate(inView,*oView,disk);

    pro.set_output_val<vil_image_view_base_sptr>(0, oView);

    return true;
}
