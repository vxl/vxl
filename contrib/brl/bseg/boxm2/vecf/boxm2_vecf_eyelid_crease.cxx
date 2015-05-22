#include "boxm2_vecf_eyelid_crease.h"
#include <vcl_cmath.h>
#include <vgl/vgl_distance.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_vector_3d.h>
#include <vgl/vgl_sphere_3d.h>
#include <vgl/vgl_plane_3d.h>
#include <vnl/vnl_vector_fixed.h>

double boxm2_vecf_eyelid_crease::gi(double xp, double t) const{
  vnl_vector_fixed<double, 5> m = opr_.m(xp);
  vnl_vector_fixed<double, 5> c0 = opr_.crease_coefs_t0();
  return ((1.0-t)*dot_product(m, c0) + t*el_.gi(xp, 0.0));
}

double boxm2_vecf_eyelid_crease::z(double xp, double t) const{
  vgl_vector_3d<double> usn = opr_.upper_socket_normal();
  double xlim = opr_.socket_radius();
  if(t>ct_) // lower part of crease region
    if((vcl_fabs(xp)<xlim)&&zu(xp,t)>=zlim())
      return zu(xp,t);
    else
      return zlim();
  // in upper part of the crease region
  // the underlying geometry is a plane. 
  //                       dz
  // z(x,t) = zu(x, ct) + ----(y(xp,t)-y(xp, ct))
  //                       dy
  if(vcl_fabs(xp)<xlim){
    double c = usn.z();
    if(c == 0.0)
      return zlim(); // z is undefined
    double dz_dy = -usn.y()/c;
    double zv = zu(xp, ct_) + dz_dy*(gi(xp, t)-gi(xp,ct_));
    double zm = zlim() + opr_.brow_z_limit_*opr_.eye_radius_;
    if(zv>zm)
      return zm;
    return zv;
  }
  return zlim();
}

double boxm2_vecf_eyelid_crease::t(double xp, double y) const{
  vnl_vector_fixed<double, 5> m = opr_.m(xp);
  double temp = dot_product(m, opr_.crease_coefs_t0());
  // note that the top of the upper eyelid must match bottom of the crease region (el_.gi)
  double ret = (-temp + y)/( -temp + el_.gi(xp, 0.0)); 

  return ret;
}

// theory of closest point - compute distance to eyelid sphere, compute distance to plane, take closest
double boxm2_vecf_eyelid_crease::surface_distance(vgl_point_3d<double> const& p) const{
  vgl_vector_3d<double> normal(0.0,0.0,1.0);
  vgl_point_3d<double> pt(0.0, 0.0, zlim());
  vgl_plane_3d<double> pl(normal, pt);
  double dp = vgl_distance(p, pl);
  double er = opr_.eyelid_radius();
  vgl_sphere_3d<double> sp(0.0, 0.0, 0.0, er);
  double ds = vgl_distance(p, sp);
  double t = this->t(p.x(), p.y());
  // in the lower part of the crease region
  if(t>ct_){
  // several cases to consider
  double r = vcl_sqrt(p.x()*p.x() + p.y()*p.y() + p.z()*p.z());
  // 1) point is outside sphere
  if(r>=er)
    if(ds<dp) return ds;
    else return dp;
  // 2) point is inside sphere
  return ds;
  }

  // the upper (planar) part of the crease region
  // a point on the brow plane
  vgl_point_3d<double> ptc(0.0, Y(p.x(),t), z(p.x(), t));
  // the brow plane
  vgl_plane_3d<double> plc(opr_.upper_socket_normal(), ptc);
  // the distance to the plane
  return vgl_distance(p, plc);
}

