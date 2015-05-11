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
#include <vcl_cmath.h>
class boxm2_vecf_eyelid_crease{
 public:
 boxm2_vecf_eyelid_crease(): eyelid_radius_(13.0), x_mid_(11.88), y_off_(0.88), x_lim_(11.0),                   
    x_min_(-14.0), x_max_(11.5), t_min_(0.0), t_max_(1.0), ct_(0.25), upper_socket_normal_(vgl_vector_3d<double>(0.0,-0.766, 0.643)){}
  //                                                                                                                    50 degrees
 boxm2_vecf_eyelid_crease(double eyelid_radius, double x_mid, double y_off, double x_lim, double x_min, double x_max,
                          double t_min, double t_max, double ct, vgl_vector_3d<double> upper_socket_normal):
  eyelid_radius_(eyelid_radius), x_mid_(x_mid), y_off_(y_off), x_lim_(x_lim), x_min_(x_min), x_max_(x_max),
    t_min_(t_min), t_max_(t_max), ct_(ct), upper_socket_normal_(upper_socket_normal){}

  //: The mid point of the crease region (the 11.88 value derives from the powerpoint sketches on eye images)
  // y_off is the offset relative to the iris center.
  void set_x_mid(double x_mid){x_mid_ = x_mid;}
  void set_y_off(double y_off){y_off_ = y_off;}
  //: limits for crease region. upper bound at t==0, lower bound at t==1.
  void set_tmin(double t){t_min_ = t;}
  void set_tmax(double t){t_max_ = t;}
  //: t value where the brow region of the crease starts
  void set_ct(double t){ct_ = t;}
  //: the angle of the brow suface normal relative to z (e.g. zero means the brow surface is perpendicular to z)
  void set_brow_angle_rad( double brow_angle_rad)
  {upper_socket_normal_= vgl_vector_3d<double>(0.0,-vcl_sin(brow_angle_rad), vcl_cos(brow_angle_rad));}

  // internal functions
  //: crease curves projected onto a plane perpendicular to the zaxis (x horizontal , t vertical)
  double gi(double xp, double t) const;
  //: z distance from eye sphere center where planar region starts
  double zlim() const;
  //: z coordinate of crease curve projected onto eyelid sphere, could be imaginary in general (outside sphere bounds)
  double zu(double xp, double t) const;
  //: z coordinate of bounded crease curve projected onto eyelid sphere with blend to planar regions
  double z(double xp, double t) const;

  //:spherical coordinates of crease curve
  //: spherical radius of point x,t on crease curve 
  double r(double xp, double t) const;
  //: spherical elevation of point x,t on crease curve
  double theta(double xp, double t) const;
  //: spherical azimuth of point x,t on crease curve
  double phi(double xp, double t) const;

  //: Cartesian coordinates of a point on the crease curve
  double X(double xp, double t) const;
  double Y(double xp, double t) const;
  double Z(double xp, double t) const;

  //: parameter t as a function of xp and y
  double t(double xp, double y) const;

  //: is t within range
  bool valid_t(double t, double tolerance = 0.1) const{
    return (t>=(t_min_-tolerance) && t<=(t_max_+tolerance));
  }

  //: vector from point at (xp,t) to (xp, t+dt)
  vgl_vector_3d<double>  vf(double xp, double t, double dt) const;

  //: bounding box for entire closed eyelid
  vgl_box_3d<double> bounding_box(double margin = 1.0) const;

  //: distance to closest point on crease surface
  double surface_distance(vgl_point_3d<double> const& p) const;

  //: test if point is inside the tmin, tmax bounds
  bool inside(vgl_point_3d<double> const& p, double tolerance=0.5) const;

 //public members mm units
  double eyelid_radius_; // should be consistent with eye sphere radius (~1mm skin thickness)
  double x_mid_;// center of pupil from edge of eyelid opposite nose (left eye)
  double y_off_;// offset to eyelid edge (below pupil)
  double x_lim_;// distance from center of pupil to planar surfaces on each edge of eyelid
                // assumed symmetrical for now
  double x_min_; // min value of x parameter (next to nose )
  double x_max_; // max value of x parameter (a
  double t_min_; // min value of t parameter (closest to eyebrow)
  double t_max_; // max value of t parameter (closest to cheek)
  double ct_; //t value at crease, i.e. brow plane starts at crease
  vgl_vector_3d<double> upper_socket_normal_;//outward normal to planar brow region
 private:
  boxm2_vecf_eyelid el_;
};
#endif// boxm2_vecf_eyelid_crease
