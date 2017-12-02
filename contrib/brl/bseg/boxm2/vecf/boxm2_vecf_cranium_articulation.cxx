#include "boxm2_vecf_cranium_articulation.h"

boxm2_vecf_cranium_articulation::boxm2_vecf_cranium_articulation(){

  //======================= default articulation =====================
  std::vector<boxm2_vecf_cranium_params> default_articulation;
  boxm2_vecf_cranium_params  params;
  default_articulation.push_back(params);
  play_sequence_map_["default"] = default_articulation;
  this->set_play_sequence("default") ;
}
