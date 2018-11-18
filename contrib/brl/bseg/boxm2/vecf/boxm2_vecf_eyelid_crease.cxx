#include <iostream>
#include <cmath>
#include "boxm2_vecf_eyelid_crease.h"
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vgl/vgl_distance.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_vector_3d.h>
#include <vgl/vgl_sphere_3d.h>
#include <vgl/vgl_plane_3d.h>
#include <vnl/vnl_math.h>
#include <vnl/vnl_vector_fixed.h>

boxm2_vecf_eyelid_crease::boxm2_vecf_eyelid_crease(): boxm2_vecf_lid_base(0.0, 1.0){
  el_ = boxm2_vecf_eyelid(boxm2_vecf_orbit_params());
}
boxm2_vecf_eyelid_crease::boxm2_vecf_eyelid_crease(double t_min, double t_max, double ct, vgl_vector_3d<double>  /*upper_socket_normal*/):
  boxm2_vecf_lid_base(t_min, t_max), ct_(ct){
  el_ = boxm2_vecf_eyelid(boxm2_vecf_orbit_params());
}

boxm2_vecf_eyelid_crease:: boxm2_vecf_eyelid_crease(boxm2_vecf_orbit_params const& params):
  boxm2_vecf_lid_base(params){
 dphi_rad_=params.dphi_rad_;
  el_ = boxm2_vecf_eyelid(params);
}

void boxm2_vecf_eyelid_crease::blended_2nd_order_coefs(double t, double& a0, double& a1, double& a2) const{
  vnl_vector_fixed<double, 5> c0 = opr_.crease_coefs_t0();
  vnl_vector_fixed<double, 5> c0e = opr_.eyelid_coefs_t0();
  a0 = (1-t)*c0[0] + t*c0e[0];
  a1 = (1-t)*c0[1] + t*c0e[1];
  a2 = (1-t)*c0[2] + t*c0e[2];
}

double boxm2_vecf_eyelid_crease::gi(double xp, double t) const{
  double a0, a1,a2;
  this->blended_2nd_order_coefs(t, a0, a1, a2);
  double xb = beta(xp, a0, a1, a2);
  double xs = xb/opr_.scale_x();

  vnl_vector_fixed<double, 5> c0 = opr_.crease_coefs_t0();
  vnl_vector_fixed<double, 5> m = opr_.m(xs);
  double gg = el_.gi(xp,0.0);
  double dmc = dot_product(m, c0)*opr_.eyelid_crease_scale_y();
  dmc = std::cos(dphi_rad_)*dmc + std::sin(dphi_rad_)*xs;
  return ((1.0-t)*dmc + t*gg);
}

// linearly interpolate the crease z position with respect to y
double boxm2_vecf_eyelid_crease::z_socket(double xp) const{
  double a0, a1, a2, sy = opr_.eyelid_crease_scale_y();
  double ct = opr_.eyelid_crease_ct_;
  this->blended_2nd_order_coefs(ct, a0, a1, a2);
  return lin_interp_z(xp, opr_.mid_eyelid_crease_z_, ct, sy, a0, a1, a2);
}

// the plane containing the local neighborhood of the crease curve at xp
vgl_plane_3d<double> boxm2_vecf_eyelid_crease::crease_plane(double xp) const{
  double xlat = opr_.lateral_socket_radius(), xmed = -opr_.medial_socket_radius();
  if(xp<=xmed || xp>=xlat)
    return {0.0, 0.0, 1.0, -z_socket(xp)};
  double ct = opr_.eyelid_crease_ct_;
  double dx = opr_.eye_radius_*0.01;
  vgl_point_3d<double> p0(xp, gi(xp,ct), z_socket(xp));
  vgl_point_3d<double> p1(xp+dx, gi(xp+dx,ct), z_socket(xp+dx));
  vgl_point_3d<double> p2(xp-dx, gi(xp-dx,ct), z_socket(xp-dx));
  return vgl_plane_3d<double>(p0, p1, p2);
}

// the direction of the normal to the brow as x varies (is along z at the extremes)
vgl_vector_3d<double> boxm2_vecf_eyelid_crease::socket_normal(double xp) const{
  double ct = opr_.eyelid_crease_ct_;
  // normal at crest of crease
  vgl_vector_3d<double> up_sock_normal = opr_.upper_socket_normal();
  double up_sock_ang = std::acos(std::fabs(up_sock_normal.z()));
  // normal at lateral and medial extremes
  double zang = 0.0;

  //linearly interpolate with respect to y
  double xlat = opr_.lateral_socket_radius(), xmed = -opr_.medial_socket_radius();
  double yz = gi(0.0, ct), ylat = gi(xlat, ct), ymed = gi(xmed, ct);
  double s = 0.0;
  if(xp>=0.0)
    s = (gi(xp,ct)-yz)/(ylat-yz);
  else
    s = (gi(xp,ct)-yz)/(ymed-yz);
  if(s<0.0) s = 0.0;
  if(s>1.0) s = 1.0;
  // note linear interpolation of the angles, not vectors
  double ang = (1-s)*up_sock_ang + s*zang;
  // assume brow normal is never along positive y
  vgl_vector_3d<double> vret(0.0, -std::sin(ang), std::cos(ang));
  return vret;
}

double boxm2_vecf_eyelid_crease::z(double xp, double t) const{
  double ct = opr_.eyelid_crease_ct_, z_ret = 0.0;
  if(t == ct)
    return z_socket(xp);

  // if t is above the crease then return a slanted plane merging with the brow
  //                       dz
  // z(x,t) = zu(x, ct) + ----(y(xp,t)-y(xp, ct))
  //                       dy
  if(t<ct){
    vgl_vector_3d<double> usn = this->socket_normal(xp);
    double c = usn.z();
    if(c == 0.0)
      return z_socket(xp); // z is undefined
    double dz_dy = -usn.y()/c;
    double zv = z_socket(xp) + dz_dy*(gi(xp, t)-gi(xp,ct));
    double zm = z_socket(xp) + opr_.brow_z_limit_*opr_.eye_radius_;

    z_ret = zv;
    if(zv>zm)
      z_ret = zm;
    return z_ret;

  }
  // the region below the crease clip at z_socket
  z_ret = zu(xp, t);
  double zl = z_socket(xp);
  if(z_ret<zl)
    z_ret =zl;
  return z_ret;
}

double boxm2_vecf_eyelid_crease::t0(double xp, double y) const{
  vnl_vector_fixed<double, 5> c0 = opr_.crease_coefs_t0();
  double xs = xp/opr_.scale_x();
  vnl_vector_fixed<double, 5> m = opr_.m(xs);
  double temp = dot_product(m, c0)*opr_.eyelid_crease_scale_y();
  temp = std::cos(dphi_rad_)*temp + std::sin(dphi_rad_)*xs;
  // note that the top of the upper eyelid must match bottom of the crease region (el_.gi)
  double ret = (-temp + y)/( -temp + el_.gi(xp, 0.0));
  return ret;
}

double boxm2_vecf_eyelid_crease::t(double xp, double y) const{
  double ts = t0(xp, y);
  double a0, a1, a2;
  this->blended_2nd_order_coefs(ts, a0, a1, a2);
  double xb = beta(xp,a0,a1,a2);
  double xs = xb/opr_.scale_x();
  vnl_vector_fixed<double, 5> m = opr_.m(xs);
  vnl_vector_fixed<double, 5> c0 = opr_.crease_coefs_t0();
  double temp = dot_product(m, c0)*opr_.eyelid_crease_scale_y();
  temp = std::cos(dphi_rad_)*temp + std::sin(dphi_rad_)*xs;
  // note that the top of the upper eyelid must match bottom of the crease region (el_.gi)
  double ret = (-temp + y)/( -temp + el_.gi(xp, 0.0));

  return ret;
}

// theory of closest point - compute distance to eyelid sphere, compute distance to plane, take closest
double boxm2_vecf_eyelid_crease::distance(vgl_point_3d<double> const& p) const{
  double ct = opr_.eyelid_crease_ct_;
  double t = this->t(p.x(), p.y());
  // if in the lower part of the crease region
  if(t>ct){
    // distance to the plane containing the crease curve in a neighborhood around p
    vgl_plane_3d<double> cr_pl = this->crease_plane(p.x());
    double dp = vgl_distance(p, cr_pl);

    // eyelid surface distance (includes skin thickness)
    double r = std::sqrt(p.x()*p.x() + p.y()*p.y() + p.z()*p.z());
    double ds = vgl_distance(p, opr_.lid_sph_);

    //if  inside eye sphere
    if(r<opr_.sph_.radius())
      return ds;
    // if outside spherical domain return planar distance
    double xminus = -opr_.medial_socket_radius();
    double xplus = opr_.lateral_socket_radius();
    if(p.x()<xminus || p.x()>xplus)
      return dp;
    return ds;
  }
  // the upper (planar) part of the crease region
  // a point on the brow plane
  vgl_point_3d<double> ptc(p.x(), gi(p.x(),ct), z(p.x(), ct));
  // the brow normal plane, varies with position along the crease
  vgl_vector_3d<double> n = socket_normal(p.x());
  vgl_plane_3d<double> plc(n, ptc);

  // the distance to the plane
  return vgl_distance(p, plc);
}
