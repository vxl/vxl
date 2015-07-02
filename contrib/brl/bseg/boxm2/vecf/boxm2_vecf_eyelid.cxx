#include "boxm2_vecf_eyelid.h"
#include <vcl_cmath.h>
#include <vgl/vgl_distance.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_vector_3d.h>
#include <vgl/vgl_sphere_3d.h>
#include <vgl/vgl_plane_3d.h>
#include <vnl/vnl_vector_fixed.h>

double boxm2_vecf_eyelid::gi(double xp, double t) const {
  double xs = xp/opr_.scale_x();
  vnl_vector_fixed<double, 5> m = opr_.m(xs);
  vnl_vector_fixed<double, 5> c0 = opr_.eyelid_coefs_t0();
  vnl_vector_fixed<double, 5> c1 = opr_.eyelid_coefs_t1();
  double dp0 = dot_product(m, c0 )*opr_.scale_y(), dp1 = dot_product(m, c1 )*opr_.scale_y();
  dp0 = xs*vcl_sin(dphi_rad_) + dp0*vcl_cos(dphi_rad_);
  dp1 = xs*vcl_sin(dphi_rad_) + dp1*vcl_cos(dphi_rad_);
  double t0 = dp0*(1.0-t);
  double t1 = dp1*t;
  return (t0 + t1);
}

#if 0
double boxm2_vecf_eyelid::z(double xp, double t) const{
  double xlim= opr_.lateral_socket_radius();
  if(xp<0.0)
    xlim = opr_.medial_socket_radius();
  if((vcl_fabs(xp)<xlim)&&zu(xp,t)>=zlim(xp))
    return zu(xp,t);
  else
    return zlim(xp);
}
#endif
double boxm2_vecf_eyelid::z(double xp, double t) const{
  double zret = zu(xp,t);
  double zl = zlim(xp);
  if(zret<zl)
    zret = zl;
  return zret;
}
double boxm2_vecf_eyelid::t(double xp, double y) const{
  double xs = xp/opr_.scale_x();
  vnl_vector_fixed<double, 5> m = opr_.m(xs);
  double temp = dot_product(m, opr_.eyelid_coefs_t0())*opr_.scale_y();
  double temp1 = dot_product(m, opr_.eyelid_coefs_t1())*opr_.scale_y();
  temp = xs*vcl_sin(dphi_rad_) + temp*vcl_cos(dphi_rad_);
  temp1 = xs*vcl_sin(dphi_rad_) + temp1*vcl_cos(dphi_rad_);
  double ret = (-temp + y)/(temp1 -temp );
  return ret;
}
// theory of closest point - compute distance to eyelid sphere, compute distance to plane, take closest
// need to fix for inferior lid different thickness - later
double boxm2_vecf_eyelid::surface_distance(vgl_point_3d<double> const& p) const{
  vgl_vector_3d<double> normal(0.0,0.0,1.0);
  vgl_point_3d<double> pt(0.0, 0.0, zlim(p.x()));
  vgl_plane_3d<double> pl(normal, pt);
  double dp = vgl_distance(p, pl);
  //double er = opr_.eyelid_radius();
  //  vgl_sphere_3d<double> sp(0.0, -opr_.y_off_, 0.0, er);
  double ds = vgl_distance(p, opr_.lid_sph_);
  // several cases to consider
  double r = vcl_sqrt(p.x()*p.x() + p.y()*p.y() + p.z()*p.z());
  //if(r<(er-opr_.eyelid_radius_offset_))
  if(r<opr_.sph_.radius())
    return ds;
  double xminus = -opr_.medial_socket_radius();
  double xplus = opr_.lateral_socket_radius();
  if(p.x()<xminus || p.x()>xplus)
    return dp;
  return ds;
}
