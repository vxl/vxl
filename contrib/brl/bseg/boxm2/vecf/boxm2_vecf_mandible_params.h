#ifndef boxm2_vecf_mandible_params_h_
#define boxm2_vecf_mandible_params_h_
//:
// \file
// \brief  Parameters for the mandible model
//
// \author J.L. Mundy
// \date   3 November 2015
//
#include <iostream>
#include "boxm2_vecf_articulated_params.h"
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vgl/vgl_vector_3d.h>

class boxm2_vecf_mandible_params : public boxm2_vecf_articulated_params{
 public:

 boxm2_vecf_mandible_params():  ramus_length_(1.0), mandible_intensity_(static_cast<unsigned char>(100)),left_ramus_intensity_(static_cast<unsigned char>(110)), offset_(vgl_vector_3d<double>(0.0, 0.0, 0.0)), jaw_opening_angle_rad_(0.0), max_jaw_opening_angle_rad_(0.6), planar_surface_dist_thresh_(2.0){
  }

 boxm2_vecf_mandible_params(double ramus_length):ramus_length_(ramus_length){
  }
  double planar_surface_dist_thresh_;
  double ramus_length_;
  unsigned char mandible_intensity_;
  unsigned char left_ramus_intensity_;
  double max_jaw_opening_angle_rad_;
  double jaw_opening_angle_rad_;
  vgl_vector_3d<double> offset_;
};
std::ostream&  operator << (std::ostream& s, boxm2_vecf_mandible_params const& pr);
std::istream&  operator >> (std::istream& s, boxm2_vecf_mandible_params& pr);

#endif// boxm2_vecf_mandible_params
