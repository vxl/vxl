#ifndef boxm2_vecf_skin_params_h_
#define boxm2_vecf_skin_params_h_
//:
// \file
// \brief  Parameters for the skin model
//
// \author J.L. Mundy
// \date   3 November 2015
//
#include <iostream>
#include "boxm2_vecf_articulated_params.h"
#include <vgl/vgl_vector_3d.h>
class boxm2_vecf_skin_params : public boxm2_vecf_articulated_params{
 public:

 boxm2_vecf_skin_params(): skin_is_transparent_(false), skin_transparency_factor_(0.001f),offset_(vgl_vector_3d<double>(0.0, 0.0, 0.0)), skin_intensity_(static_cast<unsigned char>(130)){
  }

 boxm2_vecf_skin_params(unsigned char skin_intensity):skin_intensity_(skin_intensity){
  }
  unsigned char skin_intensity_;
  bool skin_is_transparent_;
  float skin_transparency_factor_;
  vgl_vector_3d<double> offset_;
};
std::ostream&  operator << (std::ostream& s, boxm2_vecf_skin_params const& pr);
std::istream&  operator >> (std::istream& s, boxm2_vecf_skin_params& pr);

#endif// boxm2_vecf_skin_params
