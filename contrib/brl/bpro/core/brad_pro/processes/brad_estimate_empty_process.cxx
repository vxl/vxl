// This is brl/bpro/core/brad_pro/processes/brad_estimate_empty_process.cxx
#include <iostream>
#include <algorithm>
#include <bprb/bprb_func_process.h>
#include <bpro/core/bbas_pro/bbas_1d_array_float.h>
#include <brad/brad_phongs_model_est.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vnl/algo/vnl_levenberg_marquardt.h>
#include <vnl/vnl_math.h>
//:
// \file
namespace brad_estimate_empty_process_globals
{
    constexpr unsigned n_inputs_ = 4;
    constexpr unsigned n_outputs_ = 1;
}


//: Constructor
bool brad_estimate_empty_process_cons(bprb_func_process& pro)
{
    using namespace brad_estimate_empty_process_globals;

    std::vector<std::string> input_types_(n_inputs_);
    input_types_[0] = "bbas_1d_array_float_sptr";
    input_types_[1] = "bbas_1d_array_float_sptr";
    input_types_[2] = "bbas_1d_array_float_sptr";
    input_types_[3] = "bbas_1d_array_float_sptr";

    std::vector<std::string>  output_types_(n_outputs_);

    output_types_[0] = "float";

    return pro.set_input_types(input_types_)
        && pro.set_output_types(output_types_);
}


//: Execute the process
bool brad_estimate_empty_process(bprb_func_process& pro)
{
    // Sanity check
    if (pro.n_inputs()< 4) {
        std::cout << "brip_extrema_process: The input number should be 6" << std::endl;
        return false;
    }

    // get the inputs
    unsigned i=0;
    bbas_1d_array_float_sptr intensities = pro.get_input<bbas_1d_array_float_sptr>(i++);
    bbas_1d_array_float_sptr visibilities = pro.get_input<bbas_1d_array_float_sptr>(i++);
    bbas_1d_array_float_sptr camera_elev_array = pro.get_input<bbas_1d_array_float_sptr>(i++);
    bbas_1d_array_float_sptr camera_azim_array = pro.get_input<bbas_1d_array_float_sptr>(i++);

    unsigned num_samples=intensities->data_array.size();
    vnl_vector<double> Iobs(num_samples);
    vnl_vector<double> vis(num_samples);
    vnl_vector<double> camera_elev(num_samples);
    vnl_vector<double> camera_azim(num_samples);

    float mean_intensities = 0.0f ;
    float sum_weights = 0.0f ;
    for (unsigned i=0;i<num_samples;i++)
    {
        camera_elev[i]    =camera_elev_array->data_array[i];
        camera_azim[i]    =camera_azim_array->data_array[i];

        Iobs[i]        =intensities->data_array[i];
        vis[i]=visibilities->data_array[i];
        if (Iobs[i] <0.0 || Iobs[i] > 1.0 )
            vis[i] = 0.0;
        mean_intensities += float(vis[i]* Iobs[i]);
        sum_weights      += float(vis[i]);
    }
    std::vector<float> temp_histogram(8,0.125f);

    float sum = 1.0;
    for (unsigned i=0;i<Iobs.size();i++)
    {
        unsigned index = i + 1;
        if (i == Iobs.size()-1)
            index =0;
        auto gradI = (float)std::fabs(Iobs[i]-Iobs[index]);

        int bin_index = (int) std::floor(gradI*8);
        bin_index = bin_index>7 ? 7:bin_index;
        temp_histogram[bin_index] += (float)std::min(vis[i],vis[index]);
        sum += (float)std::min(vis[i],vis[index]);
    }
    for (unsigned i =0; i < 8;i++) temp_histogram[i] /= sum;
    float entropy_histo  =0.0;
    for (unsigned int i = 0; i<8; ++i)
        entropy_histo += temp_histogram[i]*std::log(temp_histogram[i]);

    entropy_histo /= float(vnl_math::log2e);
    entropy_histo = std::exp(-entropy_histo);

    i = 0;
    pro.set_output_val<float>(i++, entropy_histo);

    return true;
}
