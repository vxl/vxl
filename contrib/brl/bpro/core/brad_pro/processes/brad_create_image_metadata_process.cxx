// This is brl/bpro/core/brad_pro/processes/brad_create_image_metadata_process.cxx
#include <iostream>
#include <fstream>
#include <bprb/bprb_func_process.h>
#include <brad/brad_image_metadata.h>
//:
// \file

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: Constructor
bool brad_create_image_metadata_process_cons(bprb_func_process& pro)
{
  //input
  bool ok=false;
  std::vector<std::string> input_types;
  input_types.emplace_back("float"); // input 0: absolute calibration parameter
  input_types.emplace_back("float"); // input 1: effect band width
  input_types.emplace_back("float"); // input 2: view azimuth
  input_types.emplace_back("float"); // input 3: view elevation
  input_types.emplace_back("float"); // input 4: sun azimuth
  input_types.emplace_back("float"); // input 5: sun elevation
  input_types.emplace_back("float"); // input 6: sun irradiance
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;

  // set default values
  pro.set_input(6, new brdb_value_t<float>(1381.79f)); // band-averaged value for Quickbird panchromatic sensor

  //output
  std::vector<std::string> output_types;
  output_types.emplace_back("brad_image_metadata_sptr");
  ok = pro.set_output_types(output_types);
  if (!ok) return ok;
  return true;
}

//: Execute the process
bool brad_create_image_metadata_process(bprb_func_process& pro)
{
  // Sanity check
  if (pro.n_inputs() != pro.input_types().size()) {
    std::cout << "brad_create_image_metadata_process: The input number should be " << pro.input_types().size() << std::endl;
    return false;
  }

  // get the inputs
  auto abscal = pro.get_input<float>(0);
  auto effect_band_width = pro.get_input<float>(1);
  auto view_azimuth = pro.get_input<float>(2);
  auto view_elevation = pro.get_input<float>(3);
  auto sun_azimuth = pro.get_input<float>(4);
  auto sun_elevation = pro.get_input<float>(5);
  auto sun_irradiance = pro.get_input<float>(6);

  brad_image_metadata_sptr metadata = new brad_image_metadata();
  metadata->n_bands_ = 1;
  metadata->abscal_.push_back(abscal);
  metadata->effect_band_width_.push_back(effect_band_width);
  metadata->view_azimuth_ = view_azimuth;
  metadata->view_elevation_ = view_elevation;
  metadata->sun_azimuth_ = sun_azimuth;
  metadata->sun_elevation_ = sun_elevation;
  metadata->sun_irradiance_ = sun_irradiance;

  metadata->read_band_dependent_gain_offset();
  metadata->read_band_dependent_solar_irradiance();

  pro.set_output_val<brad_image_metadata_sptr>(0,metadata);

  return true;
}
