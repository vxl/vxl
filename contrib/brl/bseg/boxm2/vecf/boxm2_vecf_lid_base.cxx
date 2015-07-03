#include "boxm2_vecf_lid_base.h"
#include <vcl_cmath.h>
#include <vgl/vgl_distance.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_vector_3d.h>
#include <vgl/vgl_sphere_3d.h>
#include <vgl/vgl_plane_3d.h>
#include <vnl/vnl_vector_fixed.h>

double boxm2_vecf_lid_base::zlim(double xp) const{
  double xlim = opr_.lateral_socket_radius();
  if(xp<0.0)
    xlim = opr_.medial_socket_radius();
  //double er = opr_.sph_.radius();//was eyelid radius (incorrect)
   double er = opr_.eyelid_radius();//was eyelid radius (incorrect)
  double arg = er*er - xlim*xlim;
  if(arg<0.0) return 0.0;
  return vcl_sqrt(arg);
}

double boxm2_vecf_lid_base::zu(double xp, double t) const{
  double er = opr_.eyelid_radius();
  if(t>=opr_.lower_eyelid_tmin_)
    er = opr_.lid_sph_.radius();
  double r02 = er*er;
  double g0 = gi(xp,t);
  double arg = r02-g0*g0-xp*xp;
  if(arg<0.0) arg =0.0;
  return vcl_sqrt(arg);
}
double boxm2_vecf_lid_base::r(double xp, double t) const{
  double zv = z(xp,t);
  double gv = gi(xp,t);
  return vcl_sqrt(zv*zv + gv*gv + xp*xp);
}

double boxm2_vecf_lid_base::theta(double xp, double t) const{
  double zv = z(xp,t);
  double rv = r(xp,t);
  return vcl_acos(zv/rv);
}
double boxm2_vecf_lid_base::phi(double xp, double t) const{
  return vcl_atan2(gi(xp,t), xp);
}

double boxm2_vecf_lid_base::X(double xp,double t) const{
  return (r(xp,t)*vcl_sin(theta(xp,t))*vcl_cos(phi(xp,t)));
}

double boxm2_vecf_lid_base::Y(double xp,double t) const{
  return (r(xp,t)*vcl_sin(theta(xp,t))*vcl_sin(phi(xp,t)));
}

double boxm2_vecf_lid_base::Z(double xp,double t) const{
  return (r(xp,t)*vcl_cos(theta(xp,t)));
}

vgl_vector_3d<double>  boxm2_vecf_lid_base::vf(double xp, double t, double dt) const{
  vgl_point_3d<double> X0(X(xp,t), Y(xp,t), Z(xp,t));
  vgl_point_3d<double> Xt(X(xp,t+dt), Y(xp,t+dt), Z(xp,t+dt));
  return (Xt-X0);
}

vgl_box_3d<double> boxm2_vecf_lid_base::bounding_box(double margin) const{
  double xmin = opr_.x_min(), xmax = opr_.x_max();
  vgl_point_3d<double> p0(X(xmin,0.0), Y(xmin,0.0), Z(xmin,0.0));
  vgl_point_3d<double> p1(X(xmax,0.0), Y(xmax,0.0), Z(xmax,0.0));
  vgl_point_3d<double> p2(X(0.0,t_min_), Y(0.0,t_min_), Z(0.0,t_min_));
  vgl_point_3d<double> p3(X(0.0,t_max_), Y(0.0,t_max_), Z(0.0,t_max_));
  vgl_point_3d<double> p4(X(0.0,0.5), Y(0.0,0.5), Z(0.0,0.5));
  vgl_box_3d<double> bb;
  bb.add(p0); bb.add(p1); bb.add(p2); bb.add(p3); bb.add(p4);
  //expand in each direction
  vgl_point_3d<double> minpt = bb.min_point();
  vgl_point_3d<double> maxpt = bb.max_point();
  vgl_vector_3d<double> v(margin, margin, margin);
  bb.set_min_point(minpt - v); bb.set_max_point(maxpt + v);
  return bb;
}

bool boxm2_vecf_lid_base::inside(vgl_point_3d<double> const& p, double tolerance) const{
  //for all ~x the point y must be less than or within tolerance of the y value of the tmin curve
  //AND all ~x the point y must be greater than or within tolerance of the y value of the tmax curve
  double y_top = Y(p.x(),t_min_);
  double y_bot = Y(p.x(),t_max_);
  double y = p.y();
  bool inside = y<=(y_top + tolerance) && y>=(y_bot-tolerance);
  return inside;
}
