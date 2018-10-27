#ifndef boxm2_vecf_middle_fat_pocket_params_h_
#define boxm2_vecf_middle_fat_pocket_params_h_
//:
// \file
// \brief  Parameters for the middle_fat_pocket model
//
// \author J.L. Mundy
// \date   30 December 2015
//
#include <iostream>
#include "boxm2_vecf_articulated_params.h"
#include <vgl/vgl_vector_3d.h>
#include <vnl/vnl_vector_fixed.h>
class boxm2_vecf_middle_fat_pocket_params : public boxm2_vecf_articulated_params{
 public:
  // boxm2_vecf_middle_fat_pocket_params(): origin_(vgl_point_3d<double>(26.14, -7.45, 72.1)),//original
 boxm2_vecf_middle_fat_pocket_params(): origin_(vgl_point_3d<double>(36.14, (10.0-7.45), 62.1)),
  normal_(vgl_vector_3d<double>(0.73911, -0.2, 0.666008)), fit_to_subject_(true), su_(1.0), sv_(1.0), sw_(1.0),
    principal_eigenvector_(vgl_vector_3d<double>(0.55,0.68,-0.42)), lambda_(1.0), gamma_(0.2), principal_offset_(0.0), tolerance_(0.5),
    max_norm_distance_(25.0), scale_at_midpt_(0.85), scale_at_max_(0.5),middle_fat_pocket_intensity_(static_cast<unsigned char>(50)){
  }
  bool fit_to_subject_;
  // for anisotropic scaling (fit to subject) as opposed to mechanical deformation
  double su_;
  double sv_;
  double sw_;
  //mechanical deformation parameters (fit to expression)
  vgl_vector_3d<double> principal_eigenvector_;
  double principal_offset_;
  double lambda_;
  double gamma_;
  // basic shape parameters
  double tolerance_;
  vgl_point_3d<double> origin_;
  vgl_vector_3d<double> normal_;
  double max_norm_distance_;
  double scale_at_midpt_;
  double scale_at_max_;
  // appearance parameters
  unsigned char middle_fat_pocket_intensity_;
};
std::ostream&  operator << (std::ostream& s, boxm2_vecf_middle_fat_pocket_params const& pr);
std::istream&  operator >> (std::istream& s, boxm2_vecf_middle_fat_pocket_params& pr);

#endif// boxm2_vecf_middle_fat_pocket_params
