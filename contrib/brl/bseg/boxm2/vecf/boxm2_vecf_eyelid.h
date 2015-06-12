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
 boxm2_vecf_eyelid(): boxm2_vecf_lid_base(0.0, 1.0){}

 boxm2_vecf_eyelid(double t_min, double t_max): boxm2_vecf_lid_base(t_min, t_max){}
 boxm2_vecf_eyelid(boxm2_vecf_orbit_params const& params): boxm2_vecf_lid_base(params){}

  // limits for crease
  void set_ctmin(double t){ct_min_ = t;}
  void set_ctmax(double t){ct_max_ = t;}


  // internal functions
  //: eyelid curves projected onto a plane perpendicular to the zaxis (x horizontal , t vertical)
  double gi(double xp, double t) const;

  //: z coordinate of bounded eyelid curve projected onto eyelid sphere with blend to planar regions
  double z(double xp, double t) const;

  //: parameter t as a function of xp and y
  double t(double xp, double y) const;

  //: distance to closest point on closed eyelid surface
  double surface_distance(vgl_point_3d<double> const& p) const;


  // ct is blending parameter for the crease 
  double ct_min_; // min value of ct parameter (closest to eyebrow)
  double ct_max_; // max value of ct parameter (closest to cheek)

};
#endif// boxm2_vecf_eyelid
