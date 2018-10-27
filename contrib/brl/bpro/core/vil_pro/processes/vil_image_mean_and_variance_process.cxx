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
    constexpr unsigned int n_inputs_ = 2;
    constexpr unsigned int n_outputs_ = 2;
}

bool vil_image_mean_and_variance_process_cons(bprb_func_process& pro)
{
    using namespace vil_image_mean_and_variance_process_globals;

    std::vector<std::string> input_types_(n_inputs_);
    std::vector<std::string> output_types_(n_outputs_);

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
        std::cout << "vil_set_float_image_pixel_process: "
                 << "The number of inputs should be " << n_inputs_ << std::endl;
        return false;
    }

    vil_image_view_base_sptr baseSptr =
        pro.get_input<vil_image_view_base_sptr>(0);

    auto targetPlane = pro.get_input<unsigned>(1);

    if( targetPlane > baseSptr->nplanes() )
    {
        std::cerr << "Specified plane not valid: " << std::endl
                 << "target plane = " << targetPlane << std::endl
                 << "img.nplanes() = " << baseSptr->nplanes() << std::endl;

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

// compute mean and variance image using a specified nxn mask
namespace vil_mean_and_variance_image_process_globals
{
    constexpr unsigned int n_inputs_ = 2;
    constexpr unsigned int n_outputs_ = 2;
}

bool vil_mean_and_variance_image_process_cons(bprb_func_process& pro)
{
    using namespace vil_mean_and_variance_image_process_globals;

    std::vector<std::string> input_types_(n_inputs_);
    std::vector<std::string> output_types_(n_outputs_);

    input_types_[0] = "vil_image_view_base_sptr";
    input_types_[1] = "unsigned";//neighborhood size, e.g. pass 5 for a 5x5 neighborhood around each pixel

    output_types_[0] = "vil_image_view_base_sptr";
    output_types_[1] = "vil_image_view_base_sptr";

    return pro.set_input_types(input_types_)
       &&  pro.set_output_types(output_types_)
       &&  pro.set_input(1,brdb_value_sptr(new brdb_value_t<unsigned>(0)));
}

bool vil_mean_and_variance_image_process(bprb_func_process& pro)
{
    using namespace vil_mean_and_variance_image_process_globals;

    if (pro.n_inputs() < n_inputs_) {
        std::cout << "vil_set_float_image_pixel_process: "
                 << "The number of inputs should be " << n_inputs_ << std::endl;
        return false;
    }

    vil_image_view_base_sptr baseSptr = pro.get_input<vil_image_view_base_sptr>(0);
    auto n = pro.get_input<unsigned>(1);
    int n_half = int((double)n/2.0);
    std::cout << "using n_half: " << n_half << std::endl;

    //assume can convert to float
    vil_image_view<float> fview = *vil_convert_cast(float(), baseSptr);
    auto* mean_view = new vil_image_view<float>(fview.ni(), fview.nj());
    auto* var_view = new vil_image_view<float>(fview.ni(), fview.nj());
    mean_view->fill(0.0f);
    var_view->fill(0.0f);

    double mean = 0.0, var = 0.0;

    for (int i = n_half; i < fview.ni()-n_half; i++)
      for (int j = n_half; j < fview.nj()-n_half; j++) {
        mean = 0.0;
        var = 0.0;
        unsigned cnt = 0;
        for (int k = i-n_half; k < i+n_half; k++)
          for (int m = j-n_half; m < j+n_half; m++) {
            mean += fview(k,m);
            cnt++;
          }
        mean /= cnt;
        cnt = 0;
        for (int k = i-n_half; k < i+n_half; k++)
          for (int m = j-n_half; m < j+n_half; m++) {
            var += (fview(k,m)-mean)*(fview(k,m)-mean);
            cnt++;
          }
        var /= (cnt-1);
        (*mean_view)(i,j) = mean;
        (*var_view)(i,j) = var;
      }

    pro.set_output_val<vil_image_view_base_sptr>(0, vil_image_view_base_sptr(mean_view));
    pro.set_output_val<vil_image_view_base_sptr>(1, vil_image_view_base_sptr(var_view));

    return true;
}
