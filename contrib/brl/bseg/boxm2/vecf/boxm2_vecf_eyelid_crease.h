#ifndef boxm2_vecf_eyelid_crease_h_
#define boxm2_vecf_eyelid_crease_h_
//:
// \file
// \brief  The parametric eyelid crease model. The region above the upper eyelid including the brow.
//
// \author J.L. Mundy
// \date   14 Apr 2015
//
// TODO - limit t_min so that the brow region doesn't jut out too far when the brow angle is increased.
//
#include <iostream>
#include <cmath>
#include <vgl/vgl_box_3d.h>
#include <vgl/vgl_vector_3d.h>
#include <vgl/vgl_plane_3d.h>
#include "boxm2_vecf_eyelid.h"
#include "boxm2_vecf_lid_base.h"
#include "boxm2_vecf_orbit_params.h"
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
class boxm2_vecf_eyelid_crease : public boxm2_vecf_lid_base{
 public:
  boxm2_vecf_eyelid_crease();
  boxm2_vecf_eyelid_crease(double t_min, double t_max, double ct, vgl_vector_3d<double> upper_socket_normal);

 boxm2_vecf_eyelid_crease(boxm2_vecf_orbit_params const& params);

  // internal functions
  //: crease curves projected onto a plane perpendicular to the zaxis (x horizontal , t vertical)
  double gi(double xp, double t) const override;

  //: z coordinate of bounded crease curve projected onto eyelid sphere with blend to planar regions
  double z(double xp, double t) const override;

    //: parameter t as a function of xp and y
  // initial approximate value - exact if no rotation
  double t0(double xp, double y) const;
  // value after one iteration of beta substitution
  double t(double xp, double y) const override;

  //: distance to closest point on crease surface
  double distance(vgl_point_3d<double> const& p) const override;

  //: socket surface tapers from brow (params.mid_eyelid_crease_z_) to medial and lateral socket planes
  // matches z values at t = params.eyelid_crease_ct_
  double z_socket(double xp) const;

  //: return 2nd order coefffiients weighted by t
  void blended_2nd_order_coefs(double t, double& a0, double& a1, double& a2) const override;

  double ct_; //t value at crease, i.e. brow plane starts at crease

 private:
  vgl_plane_3d<double> crease_plane(double xp) const;
  vgl_vector_3d<double> socket_normal(double xp) const;
  boxm2_vecf_eyelid el_;
};
#endif// boxm2_vecf_eyelid_crease
