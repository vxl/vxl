#ifndef boxm2_vecf_orbit_articulation_h_
#define boxm2_vecf_orbit_articulation_h_
//:
// \file
// \brief  Parameters for the orbit model
//
// \author J.L. Mundy
// \date   14 Apr 2015
//
#include "boxm2_vecf_orbit_params.h"
class boxm2_vecf_orbit_articulation{
 public:
  boxm2_vecf_orbit_articulation();
  //: Iterators
  typedef vcl_vector<boxm2_vecf_orbit_params>::iterator iterator;
  iterator begin() { return params_.begin(); }
  iterator end()   { return params_.end(); }
 private:
  vcl_vector<boxm2_vecf_orbit_params> params_;
};
#endif// boxm2_vecf_orbit_articulation
