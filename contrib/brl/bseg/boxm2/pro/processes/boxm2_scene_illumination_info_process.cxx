// This is brl/bseg/boxm2/pro/processes/boxm2_scene_illumination_info_process.cxx
//:
// \file
// \brief  A process for accessing scene illumination information, for now lat, lon and num_illumination bins
//
// \author J. L. Mundy
// \date May 19, 2011

#include <iostream>
#include <fstream>
#include <bprb/bprb_func_process.h>

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <boxm2/boxm2_scene.h>

namespace boxm2_scene_illumination_info_process_globals
{
  constexpr unsigned n_inputs_ = 1;
  constexpr unsigned n_outputs_ = 3;
}
bool boxm2_scene_illumination_info_process_cons(bprb_func_process& pro)
{
  using namespace boxm2_scene_illumination_info_process_globals;

  //process takes 1 input, the scene
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "boxm2_scene_sptr";

  // process has 3 outputs:
  std::vector<std::string>  output_types_(n_outputs_);
  output_types_[0] = "float";// longitude
  output_types_[1] = "float";// latitude
  output_types_[2] = "int";//number of illumination bins, should be an odd number

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool boxm2_scene_illumination_info_process(bprb_func_process& pro)
{
  using namespace boxm2_scene_illumination_info_process_globals;

  if ( pro.n_inputs() < n_inputs_ ){
    std::cout << pro.name() << ": The input number should be " << n_inputs_<< std::endl;
    return false;
  }
  //get the inputs
  boxm2_scene_sptr scene = pro.get_input<boxm2_scene_sptr>(0);
  vpgl_lvcs vcs = scene->lvcs();
  double latitude, longitude, elevation;
  vcs.get_origin(latitude, longitude, elevation);
  int num_illum_bins = scene->num_illumination_bins();
  int i=0;
  // store outputs
  pro.set_output_val<float>(i++, static_cast<float>(longitude));
  pro.set_output_val<float>(i++, static_cast<float>(latitude));
  pro.set_output_val<int>(i++, num_illum_bins);
  return true;
}
