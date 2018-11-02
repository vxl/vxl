#include <limits>
#include <iostream>
#include <cmath>
#include "boxm2_vecf_lid_base.h"
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vgl/vgl_distance.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_vector_3d.h>
#include <vgl/vgl_sphere_3d.h>
#include <vgl/vgl_plane_3d.h>
#include <vnl/vnl_vector_fixed.h>
// the rotated x position, xp, is input and a 2nd order approximation to the margin or crease polynomials is used to
// recover the polynomial sweeping parameter, beta.
double boxm2_vecf_lid_base::beta(double xp, double a0, double a1, double a2) const{
  if(std::fabs(dphi_rad_)<0.001)
    return xp;
  double sy = opr_.scale_y();
  double ss = std::sin(dphi_rad_);
  double cs = std::cos(dphi_rad_);
  double temp0 = a0*sy*ss, temp1 = a1*sy*ss, temp2 = a2*sy*ss;
  double temp3 = 1.0-(temp1/cs);
  double temp4 = -(4.0*temp2*(xp + temp0)/(cs*cs))+temp3*temp3;
  if(temp4<0.0)
    temp4 = 0.0;
  double neu = temp1 + cs*(-1.0 + std::sqrt(temp4));
  double ret = -neu/(2.0*temp2);
  return ret;
}
// find the extreme value in y for a 2nd order polynomial approximation of the margin and crease polynomials
// also report the x position where the extremum happens
void boxm2_vecf_lid_base::extrema(double sy, double a0, double a1, double a2, double& yext, double& xext) const{
  double ss = std::sin(dphi_rad_);
  double cs = std::cos(dphi_rad_);
  double tan = ss/cs;
  double temp0 = (a1*a1 -4.0*a0*a2)*sy*sy*cs;
  double temp1 = ss*(2.0*a1*sy + tan);
  yext = -(temp0 + temp1)/(4.0*a2*sy);
  double beta_ext = -(a1*sy + tan)/(2.0*a2*sy);
  double temp2 = a0 + a1*beta_ext + a2*beta_ext*beta_ext;
  xext = cs*beta_ext -ss*sy*temp2;
}

//gave much worse approximation than the linear fit
static double q(double s, double z0, double z1, double z2){
 double L0 = -(s-1)*(s+1);
 double  L1 = s*(s+1)/2.0;
 double  L2 = s*(s-1)/2.0;
 double ret = z0*L0 + z1*L1 +z2*L2;
  return ret;
}
#if 0
double  boxm2_vecf_lid_base::quad_interp_z(double xp, double mid_z, double t) const{
  double xlat = opr_.lateral_socket_radius(), xmed = -opr_.medial_socket_radius();
  double yz = gi(0.0, t), ylat = gi(xlat, t), ymed = gi(xmed, t);
  double ret_z = 0.0, s = 0.0;
  double z1 = zlim(xlat),  z2 = zlim(xmed);
  double z0 = mid_z;
  if(xp>=0.0){
    s = (gi(xp,t)-yz)/(ylat-yz);
    if(s<0.0) s = 0.0;
    if(s>1.0) s = 1.0;
    ret_z = q(s,z0,z1,z2);
  }else{
    s = (gi(xp,t)-yz)/(ymed-yz);
    if(s<0.0) s = 0.0;
    if(s>1.0) s = 1.0;
    ret_z = q(-s,z0,z1,z2);
  }
  return ret_z;
}
#endif

// the interpolation of z is bifurcated about the x position, xext, corresponding to the extreme value of y for a given margin or crease
// in each domain x<xext , x>=xext the z value is linearly interpolated according to the parameter s. The value zlim is the z of the
// lateral and medial planes as the margin or crease curves leave the surface of the eye sphere. Any interpolated z that is less than
// the z of these planes is clipped to the appropriate plane according to the x domain.
double boxm2_vecf_lid_base::lin_interp_z(double xp, double mid_z, double t, double sy, double a0, double a1, double a2) const{
  // find extrema in the y excursion and the x at which it happens
  // uses 2nd order polynomial coeficients
  double yext = 0.0, xext = 0.0;
  extrema(sy, a0, a1, a2, yext, xext);
  double xlat = opr_.lateral_socket_radius(), xmed = -opr_.medial_socket_radius();
  double ylat = gi(xlat, t), ymed = gi(xmed, t), yx =gi(xp,t);
  double ret_z = 0.0, s = 0.0;

  double zl = zlim(xp-xext);//switch zlim values at xp == xext, not xp == 0

  double zlp = zlim(1.0);   // zlimit value when xp is plus
  double zlm = zlim(-1.0);  // zlimit value when xp is minus

  bool less_p = mid_z<zlp;  // the mid z value is less than the plus z limit
  bool less_m = mid_z<zlm;  // the mid z value is less than the minus z limit

  bool not_less = !less_p && !less_m; // the mid z value is greater than either z limit
  bool plus = xp>=xext;               // x is greater than or equal to the extremum  x

  // compute the interpolating weight and interpolated z
  if(plus)
    s = (yx-yext)/(ylat-yext);
  else
    s = (yx-yext)/(ymed-yext);
   ret_z = mid_z*(1-s) + zl*s;

   // cases where the interpolated z value is clamped
   // Case I - zmid is greater than either limit
   if(not_less&&(ret_z<zl))
     ret_z = zl;

   // Case II - zmid is less than the plus z limit
   if(plus && less_p && (ret_z>zl))
     ret_z = zl;

   // Case III - zmid is less than the minus z limit
   if(!plus && less_m && (ret_z>zl))
     ret_z = zl;
  return ret_z;
}

double boxm2_vecf_lid_base::zlim(double xp) const{
  double xlim = opr_.lateral_socket_radius();
  if(xp<0.0)
    xlim = opr_.medial_socket_radius();
  double er = opr_.eyelid_radius(); ////JLM
  //double er = opr_.lid_sph_.radius();
  double arg = er*er - xlim*xlim;
  if(arg<0.0) return 0.0;
  double zr = std::sqrt(arg);
  return zr;
}

double boxm2_vecf_lid_base::zu(double xp, double t) const{
  double er = opr_.eyelid_radius(); //reconsider
  if(t>=opr_.lower_eyelid_tmin_)
    er = opr_.lid_sph_.radius();
  double r02 = er*er;
  double g0 = gi(xp,t);
  double arg = r02-g0*g0-xp*xp;
  if(arg<0.0) arg =0.0;
  return std::sqrt(arg);
}
double boxm2_vecf_lid_base::r(double xp, double t) const{
  double zv = z(xp,t);
  double gv = gi(xp,t);
  return std::sqrt(zv*zv + gv*gv + xp*xp);
}

double boxm2_vecf_lid_base::theta(double xp, double t) const{
  double zv = z(xp,t);
  double rv = r(xp,t);
  return std::acos(zv/rv);
}
double boxm2_vecf_lid_base::phi(double xp, double t) const{
  return std::atan2(gi(xp,t), xp);
}

double boxm2_vecf_lid_base::X(double xp,double t) const{
  return (r(xp,t)*std::sin(theta(xp,t))*std::cos(phi(xp,t)));
}

double boxm2_vecf_lid_base::Y(double xp,double t) const{
  return (r(xp,t)*std::sin(theta(xp,t))*std::sin(phi(xp,t)));
}

double boxm2_vecf_lid_base::Z(double xp,double t) const{
  return (r(xp,t)*std::cos(theta(xp,t)));
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
  vgl_point_3d<double> p5(0.0, 0.0, opr_.eyelid_radius());

  vgl_box_3d<double> bb;
  bb.add(p0); bb.add(p1); bb.add(p2); bb.add(p3); bb.add(p4); bb.add(p5);
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
double boxm2_vecf_lid_base::curve_distance(double t, double x, double y) const{
  double margin  = 1.5;
  double xminus = -opr_.medial_socket_radius()*margin;
  double xplus = opr_.lateral_socket_radius()*margin;
  double dmin = std::numeric_limits<double>::max();
  for(double xp = xminus; xp<=xplus; xp+=0.1){
    double dx = xp-x, dy = y-gi(xp,t);
    double d = std::sqrt(dx*dx + dy*dy);
    if(d<dmin)
      dmin = d;
  }
  return dmin;
}
double boxm2_vecf_lid_base::curve_distance(double t, double x, double y, double z) const{
  double margin  = 1.5;
  double xminus = -opr_.medial_socket_radius()*margin;
  double xplus = opr_.lateral_socket_radius()*margin;
  double dmin = std::numeric_limits<double>::max();
  double xmin, ymin, zmin;// for debug purposes
  for(double xp = xminus; xp<=xplus; xp+=0.1){
    double dx = xp-x, dy = y-gi(xp,t), dz = z-Z(xp, t);
    double d = std::sqrt(dx*dx + dy*dy + dz*dz);
    if(d<dmin){
      dmin = d;
    }
  }
  return dmin;
}
double boxm2_vecf_lid_base::curve_distance(double t, vgl_point_3d<double> const& p) const{
  return curve_distance(t, p.x(), p.y(), p.z());
}
