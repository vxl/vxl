#ifndef boxm2_vecf_composite_head_model_articulation_h_
#define boxm2_vecf_composite_head_model_articulation_h_
//:
// \file
// \brief  Parameters for the composite_head_model model
//
// \author Octi Biris
// \date   9 Jul 2015
//
#include "boxm2_vecf_scene_articulation.h"
#include "boxm2_vecf_composite_head_parameters.h"

class boxm2_vecf_composite_head_model_articulation : public boxm2_vecf_scene_articulation{
 public:
  boxm2_vecf_composite_head_model_articulation();
  ~boxm2_vecf_composite_head_model_articulation(){
    params_.clear();
  }

 unsigned size(){ return params_.size(); }
 boxm2_vecf_articulated_params& operator [] (unsigned i) {
   return params_[i] ; }
 private:
  vcl_vector<boxm2_vecf_composite_head_parameters> params_;


};
#endif// boxm2_vecf_orbit_articulation
