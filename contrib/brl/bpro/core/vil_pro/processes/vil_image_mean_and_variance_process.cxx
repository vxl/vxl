//this is /brl/bpro/core/vil_pro/processes/vil_image_mean_and_variance_process.cxx
#include <bprb/bprb_func_process.h>
//:
// \file
// Compute the mean and variance of a specific image plane
// The default plane is 0.
#include<vil/vil_convert.h>
#include<vil/vil_image_resource.h>
#include<vil/vil_math.h>



namespace vil_image_mean_and_variance_process_globals
{
    const unsigned int n_inputs_ = 2;
    const unsigned int n_outputs_ = 2;
}

bool vil_image_mean_and_variance_process_cons(bprb_func_process& pro)
{
    using namespace vil_image_mean_and_variance_process_globals;

    vcl_vector<vcl_string> input_types_(n_inputs_);
    vcl_vector<vcl_string> output_types_(n_outputs_);

    input_types_[0] = "vil_image_view_base_sptr";
    input_types_[1] = "unsigned";//the image plane

    output_types_[0] = "double";
    output_types_[1] = "double";

    return pro.set_input_types(input_types_)
       &&  pro.set_output_types(output_types_)
       &&  pro.set_input(1,brdb_value_sptr(new brdb_value_t<unsigned>(0)));
}

bool vil_image_mean_and_variance_process(bprb_func_process& pro)
{
    using namespace vil_image_mean_and_variance_process_globals;

    if (pro.n_inputs() < n_inputs_) {
        vcl_cout << "vil_set_float_image_pixel_process: "
                 << "The number of inputs should be " << n_inputs_ << vcl_endl;
        return false;
    }

    vil_image_view_base_sptr baseSptr = 
        pro.get_input<vil_image_view_base_sptr>(0);

    unsigned targetPlane = pro.get_input<unsigned>(1);

    if( targetPlane > baseSptr->nplanes() )
    {
        vcl_cerr << "Specified plane not valid: " << vcl_endl
                 << "target plane = " << targetPlane << vcl_endl
                 << "img.nplanes() = " << baseSptr->nplanes() << vcl_endl;

        return false;
    }

    //assume can convert to float
    vil_image_view<float> fview = *vil_convert_cast(float(), baseSptr);

    double mean = 0.0, var = 0.0;

    vil_math_mean_and_variance(mean,var,fview,targetPlane);

    pro.set_output_val(0,mean);
    pro.set_output_val(1,var);

    return true;
}
