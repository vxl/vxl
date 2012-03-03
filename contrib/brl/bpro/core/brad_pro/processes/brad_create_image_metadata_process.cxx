// This is brl/bpro/core/brad_pro/processes/brad_create_image_metadata_process.cxx
#include <bprb/bprb_func_process.h>
#include <brad/brad_image_metadata.h>
//:
// \file

#include <vcl_fstream.h>

//: Constructor
bool brad_create_image_metadata_process_cons(bprb_func_process& pro)
{
  //input
  bool ok=false;
  vcl_vector<vcl_string> input_types;
  input_types.push_back("float"); // input 0: gain
  input_types.push_back("float"); // input 1: offset
  input_types.push_back("float"); // input 2: view azimuth
  input_types.push_back("float"); // input 3: view elevation
  input_types.push_back("float"); // input 4: sun azimuth
  input_types.push_back("float"); // input 5: sun elevation
  input_types.push_back("float"); // input 6: sun irradiance
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;

  // set default values
  pro.set_input(6, new brdb_value_t<float>(1381.79f)); // band-averaged value for Quickbird panchromatic sensor

  //output
  vcl_vector<vcl_string> output_types; 
  output_types.push_back("brad_image_metadata_sptr"); 
  ok = pro.set_output_types(output_types);
  if (!ok) return ok;
  return true;
}

//: Execute the process
bool brad_create_image_metadata_process(bprb_func_process& pro)
{
  // Sanity check
  if (pro.n_inputs() != pro.input_types().size()) {
    vcl_cout << "brad_create_image_metadata_process: The input number should be " << pro.input_types().size() << vcl_endl;
    return false;
  }

  // get the inputs
  float gain = pro.get_input<float>(0);
  float offset = pro.get_input<float>(1);
  float view_azimuth = pro.get_input<float>(2);
  float view_elevation = pro.get_input<float>(3);
  float sun_azimuth = pro.get_input<float>(4);
  float sun_elevation = pro.get_input<float>(5);
  float sun_irradiance = pro.get_input<float>(6);

  brad_image_metadata_sptr metadata = new brad_image_metadata();
  metadata->gain_ = gain;
  metadata->offset_ = offset;
  metadata->view_azimuth_ = view_azimuth;
  metadata->view_elevation_ = view_elevation;
  metadata->sun_azimuth_ = sun_azimuth;
  metadata->sun_elevation_ = sun_elevation;
  metadata->sun_irradiance_ = sun_irradiance;

  pro.set_output_val<brad_image_metadata_sptr>(0,metadata);

  return true;
}

