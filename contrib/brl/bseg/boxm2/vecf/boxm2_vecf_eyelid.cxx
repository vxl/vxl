#include <iostream>
#include <cmath>
#include "boxm2_vecf_eyelid.h"
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vgl/vgl_distance.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_vector_3d.h>
#include <vgl/vgl_sphere_3d.h>
#include <vgl/vgl_plane_3d.h>
#include <vnl/vnl_vector_fixed.h>

void boxm2_vecf_eyelid::blended_2nd_order_coefs(double t, double& a0, double& a1, double& a2) const{
  vnl_vector_fixed<double, 5> c0 = opr_.eyelid_coefs_t0();
  vnl_vector_fixed<double, 5> c1 = opr_.eyelid_coefs_t1();
  a0 = (1-t)*c0[0] + t*c1[0];
  a1 = (1-t)*c0[1] + t*c1[1];
  a2 = (1-t)*c0[2] + t*c1[2];
}

double boxm2_vecf_eyelid::gi(double xp, double t) const {
  double a0, a1, a2;
  this->blended_2nd_order_coefs(t, a0, a1, a2);
  double xb = beta(xp, a0, a1, a2);
  double xs = xb/opr_.scale_x();
  vnl_vector_fixed<double, 5> m = opr_.m(xs);
  vnl_vector_fixed<double, 5> c0 = opr_.eyelid_coefs_t0();
  vnl_vector_fixed<double, 5> c1 = opr_.eyelid_coefs_t1();
  double dp0 = dot_product(m, c0 )*opr_.scale_y(), dp1 = dot_product(m, c1 )*opr_.scale_y();
  dp0 = xs*std::sin(dphi_rad_) + dp0*std::cos(dphi_rad_);
  dp1 = xs*std::sin(dphi_rad_) + dp1*std::cos(dphi_rad_);
  double t0 = dp0*(1.0-t);
  double t1 = dp1*t;
  return (t0 + t1);
}

double boxm2_vecf_eyelid::z(double xp, double t) const{
  double a0, a1, a2, sy = opr_.scale_y();
  this->blended_2nd_order_coefs(t, a0, a1, a2);
  double xlim= opr_.lateral_socket_radius();
  if(xp<0.0)
    xlim = opr_.medial_socket_radius();
  if((std::fabs(xp)<xlim))
    if(is_superior_)
      return lin_interp_z(xp, opr_.mid_superior_margin_z_, t, sy, a0, a1, a2);
    else
      return lin_interp_z(xp, opr_.mid_inferior_margin_z_, t, sy, a0, a1, a2);
  else
    return zlim(xp);
}

double boxm2_vecf_eyelid::t0(double xp, double y) const{
  vnl_vector_fixed<double, 5> c0 = opr_.eyelid_coefs_t0();
  vnl_vector_fixed<double, 5> c1 = opr_.eyelid_coefs_t1();
  double xs = xp/opr_.scale_x();
  vnl_vector_fixed<double, 5> m = opr_.m(xs);
  double temp = dot_product(m, c0)*opr_.scale_y();
  double temp1 = dot_product(m, c1)*opr_.scale_y();
  temp = xs*std::sin(dphi_rad_) + temp*std::cos(dphi_rad_);
  temp1 = xs*std::sin(dphi_rad_) + temp1*std::cos(dphi_rad_);
  double ret = (-temp + y)/(temp1 -temp );
  return ret;
}

double boxm2_vecf_eyelid::t(double xp, double y) const{
  double ts = t0(xp, y);
  double a0, a1, a2;
  this->blended_2nd_order_coefs(ts, a0, a1, a2);
  double xb = beta(xp,a0,a1,a2);
  double xs = xb/opr_.scale_x();
  vnl_vector_fixed<double, 5> m = opr_.m(xs);
  vnl_vector_fixed<double, 5> c0 = opr_.eyelid_coefs_t0();
  vnl_vector_fixed<double, 5> c1 = opr_.eyelid_coefs_t1();
  double temp = dot_product(m, c0)*opr_.scale_y();
  double temp1 = dot_product(m, c1)*opr_.scale_y();
  temp = xs*std::sin(dphi_rad_) + temp*std::cos(dphi_rad_);
  temp1 = xs*std::sin(dphi_rad_) + temp1*std::cos(dphi_rad_);
  double ret = (-temp + y)/(temp1 -temp );
  return ret;
}
// theory of closest point - compute distance to eyelid sphere, compute distance to plane, take closest
// need to fix for inferior lid different thickness - later
double boxm2_vecf_eyelid::distance(vgl_point_3d<double> const& p) const{
  vgl_vector_3d<double> normal(0.0,0.0,1.0);
  vgl_point_3d<double> pt(0.0, 0.0, zlim(p.x()));
  vgl_plane_3d<double> pl(normal, pt);
  double dp = vgl_distance(p, pl);
  //double er = opr_.eyelid_radius();
  //  vgl_sphere_3d<double> sp(0.0, -opr_.y_off_, 0.0, er);
  double ds = vgl_distance(p, opr_.lid_sph_);
  // several cases to consider
  double r = std::sqrt(p.x()*p.x() + p.y()*p.y() + p.z()*p.z());
  //if(r<(er-opr_.eyelid_radius_offset_))
  if(r<opr_.sph_.radius())
    return ds;
  double xminus = -opr_.medial_socket_radius();
  double xplus = opr_.lateral_socket_radius();
  if(p.x()<xminus || p.x()>xplus)
    return dp;
  return ds;
}
