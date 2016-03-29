#include "boxm2_vecf_mandible_articulation.h"

boxm2_vecf_mandible_articulation::boxm2_vecf_mandible_articulation(){

  //======================= default articulation =====================
  std::vector<boxm2_vecf_mandible_params> default_articulation;
  boxm2_vecf_mandible_params  params;
  params.jaw_opening_angle_rad_ = 0.3;
  default_articulation.push_back(params);
  params.jaw_opening_angle_rad_ = 0.0;
  default_articulation.push_back(params);
  play_sequence_map_["default"] = default_articulation;
  this->set_play_sequence("default") ;
}
