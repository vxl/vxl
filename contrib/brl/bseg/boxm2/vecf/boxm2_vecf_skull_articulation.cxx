#include "boxm2_vecf_skull_articulation.h"

boxm2_vecf_skull_articulation::boxm2_vecf_skull_articulation(){

  //======================= default articulation =====================
  std::vector<boxm2_vecf_skull_params> default_articulation;
  boxm2_vecf_skull_params  params;
  default_articulation.push_back(params);
  params.mandible_params_.jaw_opening_angle_rad_ = 0.25;
  default_articulation.push_back(params);
  play_sequence_map_["default"] = default_articulation;
  this->set_play_sequence("default") ;
}
