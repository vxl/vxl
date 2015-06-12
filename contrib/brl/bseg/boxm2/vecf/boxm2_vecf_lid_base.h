#ifndef boxm2_vecf_lid_base_h_
#define boxm2_vecf_lid_base_h_
//:
// \file
// \brief  The parametric base model for the upper and lower eyelid and brow
//
// \author J.L. Mundy
// \date   19 May 2015
//
#include <vgl/vgl_box_3d.h>
#include <vgl/vgl_vector_3d.h>
#include "boxm2_vecf_orbit_params.h"
class boxm2_vecf_lid_base{
 public:
 boxm2_vecf_lid_base(): t_min_(0.0), t_max_(1.0){}

 boxm2_vecf_lid_base(double t_min, double t_max):t_min_(t_min), t_max_(t_max){}

 boxm2_vecf_lid_base(boxm2_vecf_orbit_params const& params):t_min_(0.0), t_max_(1.0), opr_(params){}

  // limits for upper or lower lid contours
  void set_tmin(double t){t_min_ = t;}
  void set_tmax(double t){t_max_ = t;}

  //: lid_base curves projected onto a plane perpendicular to the zaxis (x horizontal , t vertical)
  virtual double gi(double xp, double t) const = 0; 

  //: z distance from eye sphere center where planar region starts
  virtual double zlim() const;

  //: z coordinate of lid_base curve projected onto lid_base sphere, could be imaginary in general (outside sphere bounds)
  virtual double zu(double xp, double t) const;

  //: z coordinate of bounded lid_base curve projected onto lid_base sphere with blend to planar regions
  virtual double z(double xp, double t) const = 0;

  //:spherical coordinates of lid_base curve
  //: spherical radius of point x,t on lid_base curve 
  virtual double r(double xp, double t) const;
  //: spherical elevation of point x,t on lid_base curve
  virtual double theta(double xp, double t) const;
  //: spherical azimuth of point x,t on lid_base curve
  virtual double phi(double xp, double t) const;

  //: Cartesian coordinates of a point on the lid_base curve
  virtual double X(double xp, double t) const;
  virtual double Y(double xp, double t) const;
  virtual double Z(double xp, double t) const;

  //: parameter t as a function of xp and y
  virtual double t(double xp, double y) const = 0;

  //: is t within range
  virtual bool valid_t(double t, double tolerance = 0.1) const{
    return (t>=(t_min_-tolerance) && t<=(t_max_+tolerance));
  }

  //: vector from point at (xp,t) to (xp, t+dt) (the vector field)
  virtual vgl_vector_3d<double>  vf(double xp, double t, double dt) const;

  //: bounding box for entire closed lid_base
  virtual vgl_box_3d<double> bounding_box(double margin = 1.0) const;

  //: distance to closest point on closed lid_base surface
  virtual double surface_distance(vgl_point_3d<double> const& p) const = 0;

  //: test if point is inside the tmin, tmax bounds
  virtual bool inside(vgl_point_3d<double> const& p, double tolerance=0.5) const;


  // assumed symmetrical for now
  double t_min_; // min value of t parameter (closest to eyebrow)
  double t_max_; // max value of t parameter (closest to cheek bone)
  boxm2_vecf_orbit_params opr_;
};
#endif// boxm2_vecf_lid_base
