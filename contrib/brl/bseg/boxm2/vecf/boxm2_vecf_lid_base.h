#ifndef boxm2_vecf_lid_base_h_
#define boxm2_vecf_lid_base_h_
//:
// \file
// \brief  The parametric base model for the upper and lower eyelid and brow
//
// \author J.L. Mundy
// \date   19 May 2015
//
#include <utility>
#include <vgl/vgl_box_3d.h>
#include <vgl/vgl_vector_3d.h>
#include <vgl/vgl_sphere_3d.h>

#include "boxm2_vecf_orbit_params.h"
class boxm2_vecf_lid_base{
 public:
 boxm2_vecf_lid_base(): t_min_(0.0), t_max_(1.0), dphi_rad_(0.0){}

 boxm2_vecf_lid_base(double t_min, double t_max):t_min_(t_min), t_max_(t_max),dphi_rad_(0.0){}

 boxm2_vecf_lid_base(boxm2_vecf_orbit_params  params):t_min_(0.0), t_max_(1.0), opr_(std::move(params)),dphi_rad_(0.0){}


  // limits for upper or lower lid contours
  void set_tmin(double t){t_min_ = t;}
  void set_tmax(double t){t_max_ = t;}

  //: lid_base curves projected onto a plane perpendicular to the zaxis (x horizontal , t vertical)
  virtual double gi(double xp, double t) const = 0;

  //: z distance from eye sphere center where planar region starts
  virtual double zlim(double xp) const;

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

  void set_phi_rotation_rad(double dphi_rad){dphi_rad_ = dphi_rad;}
  double phi_rotation_rad() const{return dphi_rad_;}

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
  virtual double distance(vgl_point_3d<double> const& p) const = 0;

  //: test if point is inside the tmin, tmax bounds
  virtual bool inside(vgl_point_3d<double> const& p, double tolerance=0.5) const;

  //: interpolate z as a linear function of y from midpoint to socket limits.
  // t is the parameter defining the y(x) shape of the curve
  double lin_interp_z(double xp, double mid_z, double t, double sy, double a0, double a1, double a2) const;

  // return a linear combination of polynomial coefficients depending on margin or crease curves, thus pure virtual
  virtual void blended_2nd_order_coefs(double t, double& a0, double& a1, double& a2) const = 0;

  // obsolete - linear is a much better fit
  //  double  quad_interp_z(double xp, double mid_z, double t) const;

  //: find the value of the parameter, beta, that defines the x and y polynomials under rotation
  // That is if,
  //  x'(beta) = Cos phi x(beta) - Sin phi y(beta)
  //  y'(beta) = Sin phi x(beta) + Cos phi y(beta)
  // then find beta such that x'(beta) = xp ( a measured or specified x coordinate)
  // note that the angle phi is specified as a member, dphi_rad_.
  //
  double beta(double xp, double a0, double a1, double a2) const;

  // when a margin or crease polynomial is rotated, the extreme value in y occurs at a different
  // x position than for the un-rotated polynomial. This function finds the extremal y and x for a
  // given set of 2nd order approximation coeficients and y scale factor.
  void extrema(double sy, double a0, double a1, double a2, double& yext, double& xext) const;

  //: distance to margin or crease curve
  double curve_distance(double t, double x, double y) const;
  double curve_distance(double t, double x, double y, double z) const;
  double curve_distance(double t, vgl_point_3d<double> const& p) const;

  // assumed symmetrical for now
  double t_min_; // min value of t parameter (closest to eyebrow)
  double t_max_; // max value of t parameter (closest to cheek bone)
  boxm2_vecf_orbit_params opr_;
  double dphi_rad_;
};
#endif// boxm2_vecf_lid_base
