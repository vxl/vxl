//this is /brl/bpro/core/vil_pro/processes/vil_image_sum_process.cxx
#include<bprb/bprb_func_process.h>
//:
// \file

#include<vil/vil_convert.h>
#include<vil/vil_image_view.h>
#include<vil/vil_math.h>

namespace vil_image_sum_process_globals
{
    constexpr unsigned int n_inputs_ = 2;
    constexpr unsigned int n_outputs_ = 1;
}

bool vil_image_sum_process_cons( bprb_func_process& pro )
{
    using namespace vil_image_sum_process_globals;

    std::vector<std::string> input_types_(n_inputs_);
    std::vector<std::string> output_types_(n_outputs_);

    unsigned i = 0;
    input_types_[i++] = "vil_image_view_base_sptr"; // img
    input_types_[i++] = "unsigned"; // plane index

    output_types_[0] = "double";

    return pro.set_input_types(input_types_)
       &&  pro.set_output_types(output_types_)
       &&  pro.set_input(1, brdb_value_sptr(new brdb_value_t<unsigned>(0)));
           //default value
}

bool vil_image_sum_process( bprb_func_process& pro )
{
    using namespace vil_image_sum_process_globals;

    if (pro.n_inputs() != n_inputs_) {
        std::cout << "vil_set_float_image_pixel_process: "
                 << "The number of inputs should be " << n_inputs_ << std::endl;
        return false;
    }

    unsigned i = 0;
    vil_image_view_base_sptr imgBaseSptr =
        pro.get_input<vil_image_view_base_sptr>(i++);
    auto p = pro.get_input<unsigned>(i++);

    // convert to float image
    vil_image_view<float> fimage;
    if (imgBaseSptr->pixel_format() == VIL_PIXEL_FORMAT_RGBA_BYTE ) {
        vil_image_view_base_sptr plane_image = vil_convert_to_n_planes(4, imgBaseSptr);
        fimage = *vil_convert_cast(float(), plane_image);
        vil_math_scale_values(fimage, 1.0/255.0);
    }
    else {
        //can use convert cast
        fimage = *vil_convert_cast(float(), imgBaseSptr);
    }

    double sum = 0.0;
    vil_math_sum(sum, fimage, p);

    pro.set_output_val(0,sum);

    return true;
}
