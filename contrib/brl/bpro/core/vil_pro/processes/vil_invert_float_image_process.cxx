// This is brl/bpro/core/vil_pro/processes/vil_invert_float_image_process.cxx
#include <iostream>
#include <bprb/bprb_func_process.h>
//:
// \file

#include <vil/vil_math.h>
#include <vil/vil_image_view.h>
#include <vil/vil_convert.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif


//: Constructor
bool vil_invert_float_image_process_cons(bprb_func_process& pro)
{
    //this process takes three inputs and has one output
    std::vector<std::string> input_types;
    input_types.emplace_back("vil_image_view_base_sptr");

    std::vector<std::string> output_types;
    output_types.emplace_back("vil_image_view_base_sptr");  // label image

    return pro.set_input_types(input_types)
        && pro.set_output_types(output_types);
}

//: Execute the process
bool vil_invert_float_image_process(bprb_func_process& pro)
{
    // Sanity check
    if (pro.n_inputs()< 1) {
        std::cout << "vil_invert_float_image_process: The number of inputs should be 3" << std::endl;
        return false;
    }
    // get the inputs
    unsigned i = 0;
    vil_image_view_base_sptr img_ptr = pro.get_input<vil_image_view_base_sptr>(i++);
    if (auto* img = dynamic_cast<vil_image_view<float>*> (img_ptr.ptr()))
    {
        auto* oimg = new vil_image_view<float>(img->ni(),img->nj());
        for (unsigned int i = 0; i < img->ni(); i++)
            for (unsigned int j = 0; j < img->nj(); j++)
                (*oimg)(i, j) = (*img)(i, j) < 1.0 ? 1 - (*img)(i, j) : 0.0;

        pro.set_output_val<vil_image_view_base_sptr>(0, oimg);
        return true;
    }
    std::cout << "The image is not a float image " << std::endl;
    return false;
}
