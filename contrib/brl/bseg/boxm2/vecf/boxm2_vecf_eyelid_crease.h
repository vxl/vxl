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
#include <vgl/vgl_box_3d.h>
#include <vgl/vgl_vector_3d.h>
#include <vgl/vgl_plane_3d.h>
#include "boxm2_vecf_eyelid.h"
#include "boxm2_vecf_lid_base.h"
#include "boxm2_vecf_orbit_params.h"
#include <vcl_cmath.h>
class boxm2_vecf_eyelid_crease : public boxm2_vecf_lid_base{
 public:
 boxm2_vecf_eyelid_crease(): boxm2_vecf_lid_base(0.0, 1.0){el_ = boxm2_vecf_eyelid(boxm2_vecf_orbit_params());}
  //                                                                                                                    50 degrees
 boxm2_vecf_eyelid_crease(double t_min, double t_max, double ct, vgl_vector_3d<double> upper_socket_normal):
  boxm2_vecf_lid_base(t_min, t_max), ct_(ct){el_ = boxm2_vecf_eyelid(boxm2_vecf_orbit_params());}

 boxm2_vecf_eyelid_crease(boxm2_vecf_orbit_params const& params): boxm2_vecf_lid_base(params){el_ = boxm2_vecf_eyelid(params);}

  // internal functions
  //: crease curves projected onto a plane perpendicular to the zaxis (x horizontal , t vertical)
  virtual double gi(double xp, double t) const;

  //: z coordinate of bounded crease curve projected onto eyelid sphere with blend to planar regions
  double z(double xp, double t) const;

  //: parameter t as a function of xp and y
  double t(double xp, double y) const;

  //: distance to closest point on crease surface
  double surface_distance(vgl_point_3d<double> const& p) const;

  double ct_; //t value at crease, i.e. brow plane starts at crease

 private:
  boxm2_vecf_eyelid el_;
};
#endif// boxm2_vecf_eyelid_crease
