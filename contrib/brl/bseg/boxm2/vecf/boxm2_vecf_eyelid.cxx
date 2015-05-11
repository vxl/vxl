#include "boxm2_vecf_eyelid.h"
#include <vcl_cmath.h>
#include <vgl/vgl_distance.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_vector_3d.h>
#include <vgl/vgl_sphere_3d.h>
#include <vgl/vgl_plane_3d.h>

double boxm2_vecf_eyelid::gi(double xp, double t) const {
  double xt = x_mid_-xp;
  double xt2 = xt*xt;
  double xt3 = xt*xt2;
  double xt4 = xt*xt3;
  return ((1.454*xt-0.1251*xt2+0.004689*xt3-0.00008886*xt4)*(1.0-t)+
          (-0.92364*xt+0.07374*xt2-0.00225*xt3+0.00002418*xt4)*t);
}
double boxm2_vecf_eyelid::zlim() const{
  return vcl_sqrt(eyelid_radius_*eyelid_radius_ - x_lim_*x_lim_);
}
double boxm2_vecf_eyelid::zu(double xp, double t) const{
  double r02 = eyelid_radius_*eyelid_radius_;
  double g0 = gi(xp,t)-y_off_;
  return vcl_sqrt(r02-g0*g0-xp*xp);
}
double boxm2_vecf_eyelid::z(double xp, double t) const{
  if((vcl_fabs(xp)<x_lim_)&&zu(xp,t)>=zlim())
    return zu(xp,t);
  else
    return zlim();
}
double boxm2_vecf_eyelid::r(double xp, double t) const{
  double zv = z(xp,t);
  double gv = gi(xp,t)-y_off_;
  return vcl_sqrt(zv*zv + gv*gv + xp*xp);
}

double boxm2_vecf_eyelid::theta(double xp, double t) const{
  double zv = z(xp,t);
  double rv = r(xp,t);
  return vcl_acos(zv/rv);
}

double boxm2_vecf_eyelid::phi(double xp, double t) const{
  return vcl_atan2((gi(xp,t)-y_off_), xp);
}

double boxm2_vecf_eyelid::X(double xp,double t) const{
  return (r(xp,t)*vcl_sin(theta(xp,t))*vcl_cos(phi(xp,t)));
}

double boxm2_vecf_eyelid::Y(double xp,double t) const{
  return (r(xp,t)*vcl_sin(theta(xp,t))*vcl_sin(phi(xp,t)));
}

double boxm2_vecf_eyelid::Z(double xp,double t) const{
  return (r(xp,t)*vcl_cos(theta(xp,t)));
}

double boxm2_vecf_eyelid::t(double xp, double y) const{
  double xt = (x_mid_-xp);
  double xt2 = xt*xt;
  double xt3 = xt*xt2;
  double xt4 = xt*xt3;
  double temp = (1.454*xt -0.1251*xt2 + 0.004689*xt3 -0.00008886*xt4);
  double ret = (-temp + y+ y_off_)/( -temp -0.92364*xt+ 0.07374*xt2 -0.00225*xt3 + 0.00002418*xt4);
  return ret;
}
vgl_vector_3d<double>  boxm2_vecf_eyelid::vf(double xp, double t, double dt) const{
  vgl_point_3d<double> X0(X(xp,t), Y(xp,t), Z(xp,t));
  vgl_point_3d<double> Xt(X(xp,t+dt), Y(xp,t+dt), Z(xp,t+dt));
  return (Xt-X0);
}

vgl_box_3d<double> boxm2_vecf_eyelid::bounding_box(double margin) const{
  vgl_point_3d<double> p0(X(x_min_,0.0), Y(x_min_,0.0), Z(x_min_,0.0));
  vgl_point_3d<double> p1(X(x_max_,0.0), Y(x_max_,0.0), Z(x_max_,0.0));
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
// theory of closest point - compute distance to eyelid sphere, compute distance to plane, take closest
double boxm2_vecf_eyelid::surface_distance(vgl_point_3d<double> const& p) const{
  vgl_vector_3d<double> normal(0.0,0.0,1.0);
  vgl_point_3d<double> pt(0.0, 0.0, zlim());
  vgl_plane_3d<double> pl(normal, pt);
  double dp = vgl_distance(p, pl);
  vgl_sphere_3d<double> sp(0.0, 0.0, 0.0, eyelid_radius_);
  double ds = vgl_distance(p, sp);
  // several cases to consider
  double r = vcl_sqrt(p.x()*p.x() + p.y()*p.y() + p.z()*p.z());
  // 1) point is outside sphere
  if(r>=eyelid_radius_)
    if(ds<dp) return ds;
    else return dp;
  // 2) point is inside sphere
  return ds;
}

bool boxm2_vecf_eyelid::inside(vgl_point_3d<double> const& p, double tolerance) const{
  //for all ~x the point y must be less than or within tolerance of the y value of the tmin curve
  //AND all ~x the point y must be greater than or within tolerance of the y value of the tmax curve
  double y_top = Y(p.x(),t_min_);
  double y_bot = Y(p.x(),t_max_);
  double y = p.y();
  bool inside = y<=(y_top + tolerance) && y>=(y_bot-tolerance);
  return inside;
}
