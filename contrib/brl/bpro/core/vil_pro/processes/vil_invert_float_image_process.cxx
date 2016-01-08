// This is brl/bpro/core/vil_pro/processes/vil_invert_float_image_process.cxx
#include <bprb/bprb_func_process.h>
//:
// \file

#include <vil/vil_math.h>
#include <vil/vil_image_view.h>
#include <vil/vil_convert.h>
#include <vcl_iostream.h>


//: Constructor
bool vil_invert_float_image_process_cons(bprb_func_process& pro)
{
    //this process takes three inputs and has one output
    vcl_vector<vcl_string> input_types;
    input_types.push_back("vil_image_view_base_sptr");

    vcl_vector<vcl_string> output_types;
    output_types.push_back("vil_image_view_base_sptr");  // label image

    return pro.set_input_types(input_types)
        && pro.set_output_types(output_types);
}

//: Execute the process
bool vil_invert_float_image_process(bprb_func_process& pro)
{
    // Sanity check
    if (pro.n_inputs()< 1) {
        vcl_cout << "vil_invert_float_image_process: The number of inputs should be 3" << vcl_endl;
        return false;
    }
    // get the inputs
    unsigned i = 0;
    vil_image_view_base_sptr img_ptr = pro.get_input<vil_image_view_base_sptr>(i++);
    if (vil_image_view<float>* img = dynamic_cast<vil_image_view<float>*> (img_ptr.ptr()))
    {
        vil_image_view<float>* oimg = new vil_image_view<float>(img->ni(),img->nj());
        for (unsigned int i = 0; i < img->ni(); i++)
            for (unsigned int j = 0; j < img->nj(); j++)
                (*oimg)(i, j) = (*img)(i, j) < 1.0 ? 1 - (*img)(i, j) : 0.0;

        pro.set_output_val<vil_image_view_base_sptr>(0, oimg);
        return true;
    }
    vcl_cout << "The image is not a float image " << vcl_endl;
    return false;
}

