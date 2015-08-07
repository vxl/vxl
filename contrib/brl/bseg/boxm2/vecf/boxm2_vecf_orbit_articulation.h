#ifndef boxm2_vecf_orbit_articulation_h_
#define boxm2_vecf_orbit_articulation_h_
//:
// \file
// \brief  Parameters for the orbit model
//
// \author J.L. Mundy
// \date   14 Apr 2015
//
#include "boxm2_vecf_scene_articulation.h"
#include "boxm2_vecf_orbit_params.h"

class boxm2_vecf_orbit_articulation : public boxm2_vecf_scene_articulation{
 public:
  boxm2_vecf_orbit_articulation();
  ~boxm2_vecf_orbit_articulation(){
    params_.clear();
  }

 unsigned size(){ return static_cast<unsigned>(params_.size()); }
 boxm2_vecf_articulated_params& operator [] (unsigned i) {
   vcl_cout<<" method has been called for "<<i<<vcl_endl;
   return params_[i] ; }
 private:
  vcl_vector<boxm2_vecf_orbit_params> params_;


};
#endif// boxm2_vecf_orbit_articulation
