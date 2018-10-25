#ifndef boxm2_vecf_eyelid_h_
#define boxm2_vecf_eyelid_h_
//:
// \file
// \brief  The parametric upper eyelid model (need to transform the surface (scale, orientation -- later)
//
// \author J.L. Mundy
// \date   28 Mar 2015
//
#include <vgl/vgl_box_3d.h>
#include <vgl/vgl_vector_3d.h>
#include "boxm2_vecf_lid_base.h"

class boxm2_vecf_eyelid : public boxm2_vecf_lid_base{
 public:
 boxm2_vecf_eyelid(): boxm2_vecf_lid_base(0.0, 1.0), is_superior_(true){}

 boxm2_vecf_eyelid(double t_min, double t_max): boxm2_vecf_lid_base(t_min, t_max), is_superior_(true){}
 boxm2_vecf_eyelid(boxm2_vecf_orbit_params const& params, bool is_superior = true):
  boxm2_vecf_lid_base(params), is_superior_(is_superior)
  {dphi_rad_=params.dphi_rad_;}
  // limits for crease
  void set_ctmin(double t){ct_min_ = t;}
  void set_ctmax(double t){ct_max_ = t;}


  // internal functions
  //: eyelid curves projected onto a plane perpendicular to the zaxis (x horizontal , t vertical)
  double gi(double xp, double t) const override;

  //: z coordinate of bounded eyelid curve projected onto eyelid sphere with blend to planar regions
  double z(double xp, double t) const override;

  //: parameter t as a function of xp and y
  // initial approximate value - exact if no rotation
  double t0(double xp, double y) const;
  // value after one iteration of beta substitution
  double t(double xp, double y) const override;

  //: distance to closest point on closed eyelid surface
  double distance(vgl_point_3d<double> const& p) const override;


  //: return 2nd order coefficients weighted by t
  void blended_2nd_order_coefs(double t, double& a0, double& a1, double& a2) const override;


  // ct is blending parameter for the crease
  double ct_min_; // min value of ct parameter (closest to eyebrow)
  double ct_max_; // max value of ct parameter (closest to cheek)
  // is this an inferior or superior eyelid
  bool is_superior_;
};
#endif// boxm2_vecf_eyelid
