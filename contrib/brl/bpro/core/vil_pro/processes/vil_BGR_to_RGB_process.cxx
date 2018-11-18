// This is brl/bpro/core/vil_pro/processes/vil_BGR_to_RGB_process.cxx
#include <bprb/bprb_func_process.h>
//:
// \file

#include <bprb/bprb_parameters.h>
#include <vil/vil_image_view.h>
#include <vil/vil_rgb.h>
#include<bil/algo/bil_debayer_image.h>
//: Constructor
// this process takes 1 input:
// Rearrange the layers BGR to RGB
namespace vil_BGR_to_RGB_process_globals
{
    constexpr unsigned n_inputs_ = 1;
    constexpr unsigned n_outputs_ = 1;
}

bool vil_BGR_to_RGB_process_cons(bprb_func_process& pro)
{
    using namespace vil_BGR_to_RGB_process_globals;

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
bool vil_BGR_to_RGB_process(bprb_func_process& pro)
{
    // Sanity check
    if (pro.n_inputs()< 1) {
        std::cout << "vil_BGR_to_RGB_process: The number of inputs should be 1" << std::endl;
        return false;
    }

    // get the inputs
    unsigned i=0;
    //Retrieve image from input
    vil_image_view_base_sptr in_img = pro.get_input<vil_image_view_base_sptr>(i++);
    //vil_image_view<vil_rgb<vxl_byte> > * bgr_img = new vil_image_view<vil_rgb<vxl_byte> >(in_img->ni(),in_img->nj());
    std::cout<<"format : "<<in_img->pixel_format()<<" "<<in_img->nplanes()<<std::endl;
    if(auto * bgr_img= dynamic_cast<vil_image_view<vil_rgb<vxl_byte> > *>(in_img.ptr()))
    {
        for(unsigned k = 0; k <bgr_img->ni(); k++)
        {
            for(unsigned j = 0; j <bgr_img->nj(); j++)
            {
                vil_rgb<vxl_byte> BGR = (*bgr_img)(k,j);
                vil_rgb<vxl_byte> RGB(BGR.B(),BGR.G(),BGR.R());
                (*bgr_img)(k,j) = RGB;
            }
        }
        pro.set_output_val<vil_image_view_base_sptr>(0, bgr_img);
        return true;
    }
    else if (in_img->nplanes() == 4)
    {
        auto * rgb_img= new vil_image_view<vxl_byte>(in_img->ni(), in_img->nj(),3);
        if (auto * bgr_img= dynamic_cast<vil_image_view<vxl_byte>  *>(in_img.ptr()))
        {

            for(unsigned k = 0; k <bgr_img->ni(); k++)
            {
                for(unsigned j = 0; j <bgr_img->nj(); j++)
                {
                    //vxl_byte B = (*bgr_img)(k,j,0);
                    (*rgb_img)(k,j,0) = (*bgr_img)(k,j,2);
                    (*rgb_img)(k,j,1) = (*bgr_img)(k,j,1);
                    (*rgb_img)(k,j,2) = (*bgr_img)(k,j,0);
                }
            }
            pro.set_output_val<vil_image_view_base_sptr>(0, rgb_img);
            return true;
        }
        else
            return false;
    }
    else
        return false;

}
