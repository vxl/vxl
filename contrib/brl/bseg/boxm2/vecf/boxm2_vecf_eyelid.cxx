#include "boxm2_vecf_eyelid.h"
#include <vcl_cmath.h>
#include <vgl/vgl_distance.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_vector_3d.h>
#include <vgl/vgl_sphere_3d.h>
#include <vgl/vgl_plane_3d.h>
#include <vnl/vnl_vector_fixed.h>

double boxm2_vecf_eyelid::gi(double xp, double t) const {
  vnl_vector_fixed<double, 5> m = opr_.m(xp);
  vnl_vector_fixed<double, 5> c0 = opr_.eyelid_coefs_t0();
  vnl_vector_fixed<double, 5> c1 = opr_.eyelid_coefs_t1();
  double t0 = dot_product(m, c0 )*(1.0-t);
  double t1 = dot_product(m, c1)*t;
  return (t0 + t1 );
}

double boxm2_vecf_eyelid::z(double xp, double t) const{
  if((vcl_fabs(xp)<opr_.socket_radius())&&zu(xp,t)>=zlim())
    return zu(xp,t);
  else
    return zlim();
}

double boxm2_vecf_eyelid::t(double xp, double y) const{
  vnl_vector_fixed<double, 5> m = opr_.m(xp);
  double temp = dot_product(m, opr_.eyelid_coefs_t0());
  double temp1 = dot_product(m, opr_.eyelid_coefs_t1());
  double ret = (-temp + y)/(temp1 -temp );
  return ret;
}
// theory of closest point - compute distance to eyelid sphere, compute distance to plane, take closest
double boxm2_vecf_eyelid::surface_distance(vgl_point_3d<double> const& p) const{
  vgl_vector_3d<double> normal(0.0,0.0,1.0);
  vgl_point_3d<double> pt(0.0, 0.0, zlim());
  vgl_plane_3d<double> pl(normal, pt);
  double dp = vgl_distance(p, pl);
  double er = opr_.eyelid_radius();
  vgl_sphere_3d<double> sp(0.0, 0.0, 0.0, er);
  double ds = vgl_distance(p, sp);
  // several cases to consider
  double r = vcl_sqrt(p.x()*p.x() + p.y()*p.y() + p.z()*p.z());
  // 1) point is outside sphere
  if(r>=er)
    if(ds<dp) return ds;
    else return dp;
  // 2) point is inside sphere
  return ds;
}
