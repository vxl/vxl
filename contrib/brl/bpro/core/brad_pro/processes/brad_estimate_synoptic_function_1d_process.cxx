// This is brl/bpro/core/brad_pro/processes/brad_estimate_synoptic_function_1d_process.cxx
#include <bprb/bprb_func_process.h>
//:
// \file
#include <bpro/core/bbas_pro/bbas_1d_array_float.h>
#include <brad/brad_synoptic_function_1d.h>
#include <vcl_algorithm.h>

namespace brad_estimate_synoptic_function_1d_process_globals
{
  const unsigned n_inputs_  = 5;
  const unsigned n_outputs_ = 2;
}


//: Constructor
bool brad_estimate_synoptic_function_1d_process_cons(bprb_func_process& pro)
{
  using namespace brad_estimate_synoptic_function_1d_process_globals;

  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0] = "bbas_1d_array_float_sptr";
  input_types_[1] = "bbas_1d_array_float_sptr";
  input_types_[2] = "bbas_1d_array_float_sptr";
  input_types_[3] = "bbas_1d_array_float_sptr";
  input_types_[4] = "bool";  // 1 for surface 0 for air

  vcl_vector<vcl_string>  output_types_(n_outputs_);
  output_types_[0] = "bbas_1d_array_float_sptr";
  output_types_[1] = "float";

  return pro.set_input_types(input_types_) &&
         pro.set_output_types(output_types_);
}


//: Execute the process
bool brad_estimate_synoptic_function_1d_process(bprb_func_process& pro)
{
  // get the inputs
  unsigned i=0;
  bbas_1d_array_float_sptr intensities = pro.get_input<bbas_1d_array_float_sptr>(i++);
  bbas_1d_array_float_sptr visibilities = pro.get_input<bbas_1d_array_float_sptr>(i++);
  bbas_1d_array_float_sptr camera_elev_array = pro.get_input<bbas_1d_array_float_sptr>(i++);
  bbas_1d_array_float_sptr camera_azim_array = pro.get_input<bbas_1d_array_float_sptr>(i++);
  bool surface = pro.get_input<bool>(i++);

  unsigned num_samples=(unsigned)intensities->data_array.size();
  vcl_vector<double> samples(num_samples,0.0f);
  vcl_vector<double> vis(num_samples,0.0f);
  vcl_vector<double> camera_elev(num_samples,0.0f);
  vcl_vector<double> camera_azim(num_samples,0.0f);

  float mean_intensities = 0.0f ;
  float sum_weights = 0.0f ;
  for (unsigned i=0;i<num_samples;i++)
  {
    camera_elev[i]    =camera_elev_array->data_array[i];
    camera_azim[i]    =camera_azim_array->data_array[i];

    samples[i]        =intensities->data_array[i];
    vis[i]            =visibilities->data_array[i];
    if (samples[i] <0.0 || samples[i] > 1.0 ) vis[i] = 0.0;

    mean_intensities += float(vis[i]* samples[i]);
    sum_weights      += float(vis[i]);
  }
  brad_synoptic_function_1d f(camera_elev,camera_azim,vis,samples);

  if (surface)
  {
    f.fit_intensity_cubic();
    vcl_cout<<"Cubic Interpolation Model: "<<f.cubic_coef_int()<<" prob density "<<f.cubic_fit_prob_density()<<vcl_endl;
    bbas_1d_array_float_sptr new_obs = new bbas_1d_array_float(num_samples);
    for (unsigned i=0;i<num_samples;++i)
      new_obs->data_array[i]=float(f.cubic_interp_inten(f.arc_length(i)));
    int i=0;
    pro.set_output_val<bbas_1d_array_float_sptr>(i++, new_obs);
    pro.set_output_val<float>(i++, float(f.cubic_fit_prob_density()));
  }
  else
  {
    f.compute_auto_correlation();
    vcl_vector<double> amps;
    f.auto_corr_freq_amplitudes(amps);
    bbas_1d_array_float_sptr new_obs = new bbas_1d_array_float(amps.size());
    for (unsigned i=0;i<amps.size();++i)
      new_obs->data_array[i]=float(amps[i]);
    int i=0;
    pro.set_output_val<bbas_1d_array_float_sptr>(i++, new_obs);
    pro.set_output_val<float>(i++, float(f.max_frequency_prob_density()));
  }
  return true;
}

