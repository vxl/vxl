// This is brl/bpro/core/brad_pro/processes/brad_estimate_shadow_model_process.cxx
#include <bprb/bprb_func_process.h>
//:
// \file
#include <bpro/core/bbas_pro/bbas_1d_array_float.h>
#include <brad/brad_phongs_model_est.h>

namespace brad_estimate_shadow_model_process_globals
{
  constexpr unsigned n_inputs_ = 3;
  constexpr unsigned n_outputs_ = 1;
}


//: Constructor
bool brad_estimate_shadow_model_process_cons(bprb_func_process& pro)
{
  using namespace brad_estimate_shadow_model_process_globals;

  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "bbas_1d_array_float_sptr";
  input_types_[1] = "bbas_1d_array_float_sptr";
  input_types_[2] = "float";

  std::vector<std::string>  output_types_(n_outputs_);
  output_types_[0] = "float";

  return pro.set_input_types(input_types_) &&
         pro.set_output_types(output_types_);
}


//: Execute the process
bool brad_estimate_shadow_model_process(bprb_func_process& pro)
{
  // Sanity check
        using namespace brad_estimate_shadow_model_process_globals;

  if (pro.n_inputs()< n_inputs_) {
    std::cout << "brad_estimate_shadow_model_process: The input number should be 3" << std::endl;
    return false;
  }

  // get the inputs
  unsigned i=0;
  bbas_1d_array_float_sptr intensities = pro.get_input<bbas_1d_array_float_sptr>(i++);
  bbas_1d_array_float_sptr visibilities = pro.get_input<bbas_1d_array_float_sptr>(i++);
  auto ambient_light = pro.get_input<float>(i++);

  unsigned num_samples=intensities->data_array.size();

  float cum_vis=0.0;
  float cum_intensities=0.0;
  for (unsigned i=0;i<num_samples;i++)
  {
    if (intensities->data_array[i]>=0.0)
    {
      cum_intensities  +=visibilities->data_array[i] * intensities->data_array[i];
      cum_vis          +=visibilities->data_array[i];
    }
  }
  float reflectance=0.0;
  if (cum_vis>1e-10f)
    reflectance = cum_intensities / cum_vis /ambient_light;

  pro.set_output_val<float>(0, reflectance);
  return true;
}
