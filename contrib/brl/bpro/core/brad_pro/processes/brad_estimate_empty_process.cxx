// This is brl/bpro/core/brad_pro/processes/brad_estimate_empty_process.cxx
#include <bprb/bprb_func_process.h>
#include <bpro/core/bbas_pro/bbas_1d_array_float.h>
#include <brad/brad_phongs_model_est.h>
#include <vcl_algorithm.h>
#include <vnl/algo/vnl_levenberg_marquardt.h>
#include <vnl/vnl_math.h>
//:
// \file
namespace brad_estimate_empty_process_globals
{
    const unsigned n_inputs_  = 4;
    const unsigned n_outputs_ = 1;
}


//: Constructor
bool brad_estimate_empty_process_cons(bprb_func_process& pro)
{
    using namespace brad_estimate_empty_process_globals;

    vcl_vector<vcl_string> input_types_(n_inputs_);
    input_types_[0] = "bbas_1d_array_float_sptr";
    input_types_[1] = "bbas_1d_array_float_sptr";
    input_types_[2] = "bbas_1d_array_float_sptr";
    input_types_[3] = "bbas_1d_array_float_sptr";

    vcl_vector<vcl_string>  output_types_(n_outputs_);

    output_types_[0] = "float";

    return pro.set_input_types(input_types_)
        && pro.set_output_types(output_types_);
}


//: Execute the process
bool brad_estimate_empty_process(bprb_func_process& pro)
{
    // Sanity check
    if (pro.n_inputs()< 4) {
        vcl_cout << "brip_extrema_process: The input number should be 6" << vcl_endl;
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
    vcl_vector<float> temp_histogram(8,0.125f);

    float sum = 1.0;
    for (unsigned i=0;i<Iobs.size();i++)
    {
        unsigned index = i;
        if (i == Iobs.size()-1)
            index =0;
        else
            index = i+1;
        float gradI = (float)vcl_fabs(Iobs[i]-Iobs[index]);

        int bin_index  = (int) vcl_floor(gradI*8);
        bin_index = bin_index>7 ? 7:bin_index;
        temp_histogram[bin_index] += (float)vcl_min(vis[i],vis[index]);
        sum += (float)vcl_min(vis[i],vis[index]);
    }
    for (unsigned i =0; i < 8;i++) temp_histogram[i] /= sum;
    float entropy_histo  =0.0;
    for (unsigned int i = 0; i<8; ++i)
        entropy_histo += temp_histogram[i]*vcl_log(temp_histogram[i]);

    entropy_histo /= float(vnl_math::log2e);
    entropy_histo = vcl_exp(-entropy_histo);

    i = 0;
    pro.set_output_val<float>(i++, entropy_histo);

    return true;
}

