// This is contrib/brl/bseg/boxm2/vecf/boxm2_vecf_orbicularis_oris_params.h
#ifndef boxm2_vecf_orbicularis_oris_params_h_
#define boxm2_vecf_orbicularis_oris_params_h_
//:
// \file
// \brief  Parameters for the orbicularis oris
//
// \author Yi Dong
// \date   January, 2016
//
#include <iostream>
#include "boxm2_vecf_articulated_params.h"
#include <vgl/vgl_vector_3d.h>
#include <vnl/vnl_vector_fixed.h>
class boxm2_vecf_orbicularis_oris_params : public boxm2_vecf_articulated_params
{
 public:
  boxm2_vecf_orbicularis_oris_params(): principal_eigenvector_1_(vgl_vector_3d<double>(1.0, 0.0, 0.0)),
    principal_eigenvector_2_(vgl_vector_3d<double>(0.0, 1.0, 0.0)),
    lambda_(1.0), gamma_(0.2),
    planar_surface_dist_thresh_(2.0),
    tilt_angle_in_deg_(0.0),
    scale_factor_(1.0)
  {}

  ~boxm2_vecf_orbicularis_oris_params() override = default;
  //: principal stretches are applied on two principal direction axes, L1 and L2.  In general, deformation of orbicularis oris happens
  //  symmetrically in its circular plane such that L1 == x and L2 == y by default (no rotation).  Specifying L1 and L2 accordingly
  //  gives the rotation orbicularis oris needs, i.e., tilt, heading, etc., to align the global coordinates (x,y,z) to principal directions (L1,L2,L1xL2)
  vgl_vector_3d<double> principal_eigenvector_1_;
  vgl_vector_3d<double> principal_eigenvector_2_;

  //: lambda is the stretch magnitude along L1 direction.  lambda < 1 -- contraction along L1; lambda == 1 -- undeform;  lambda > 1 -- stretch
  double lambda_;

  //: gamma is a factor controlling deformation along L2 and L3 (L1xL2).  Assuming the orbicularis oris is incompressible, deformation
  //  along L2, lambda2, is defined as lambda^(-gamma) and deformation along L3, lambda3, is defined as lambda^(gamma-1)
  //  such that lambda*lambda2*lambda3 == 1
  double gamma_;

  //: the threshold a closest point on the normal plane can be away from the closest point in the cross-section pointset
  double planar_surface_dist_thresh_;

  //: the tilt angle that rotates the lower part of the muscle (to to mimic mouth open).  default = 0.0 degree ( range from -90 to 90 deg)
  double tilt_angle_in_deg_;

  //: scale factor that controls the size of muscle relative to the initial point cloud structure.  default = 1.0
  double scale_factor_;

};
std::ostream& operator << (std::ostream& os, boxm2_vecf_orbicularis_oris_params const& pr);
std::istream& operator >> (std::istream& is, boxm2_vecf_orbicularis_oris_params& pr);

#endif // boxm2_vecf_orbicularis_oris_params_h_
