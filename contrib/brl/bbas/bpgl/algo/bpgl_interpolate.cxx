#include <iostream>
#include <cmath>
#include <complex>
#include "bpgl_interpolate.h"
#include <vnl/vnl_math.h>
#include <vnl/algo/vnl_complex_eigensystem.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vgl/vgl_point_3d.h>
#if 0
#include <vgl/vgl_vector_3d.h>
#endif
#include <vgl/algo/vgl_rotation_3d.h>

vnl_double_3x3 bpgl_interpolate::logR(vnl_double_3x3 const& R)
{
  vnl_double_3x3 log_r;
  log_r.fill(0.0);
  double pi = vnl_math::pi;
  double trace = R[0][0]+R[1][1]+R[2][2];
  double tol = 1.0e-10;
  //special case when the rotation is pi
  //In this case, the normal log formula is singular and there are two
  //solutions given by r = +-pi[v], where v is a unit length eigenvector of R
  //associated with the eigenvalue +1.
  if (std::fabs(trace + 1.0)<tol)
  {
    vnl_double_3x3 I; I.fill(0.0);
    vnl_complex_eigensystem ce(R.as_ref(), I.as_ref());
    for (unsigned i = 0; i<3; ++i) {
      std::complex<double> eigenv = ce.eigen_value(i);

      if (std::fabs(eigenv.imag())<tol&&std::fabs(eigenv.real()-1.0)<tol)
      {
        vnl_vector<std::complex<double> > vr = ce.right_eigen_vector(i);
        log_r[0][1] = -pi*vr[2].real(); log_r[0][2] =  pi*vr[1].real();
        log_r[1][0] =  pi*vr[2].real(); log_r[1][2] = -pi*vr[0].real();
        log_r[2][0] = -pi*vr[1].real(); log_r[2][1] =  pi*vr[0].real();
        return log_r;
      }
    }
    std::cerr << "In bpgl_interpolate::log_r - shouldn't happen! FIX!!!!\n";
    return log_r;
  }
  //The usual case where a unique solution exists
  double cos_phi = 0.5*(trace -1);
  double sin_phi = std::sqrt(1.0 - cos_phi*cos_phi);
  double phi = std::asin(sin_phi);
  log_r = R - R.transpose();
  if (std::fabs(sin_phi)>tol)
    log_r *= (0.5*phi)/sin_phi;
  else log_r *= 0.5;
  return log_r;
}

vnl_double_3x3 bpgl_interpolate::expr(vnl_double_3x3 const& r)
{
  vnl_double_3x3 ex;
  double tol = 1.0e-10;
  double norm_r = std::sqrt(r[0][1]*r[0][1]+r[0][2]*r[0][2]+r[1][2]*r[1][2]);
  if (norm_r<tol)//the case of an identity rotation
  {
    return ex.set_identity();
  }
  double sin_r = std::sin(norm_r), cos_r = std::cos(norm_r);
  vnl_double_3x3 I, rsq = r*r;
  I.set_identity();
  ex = I + (sin_r/norm_r)*r + ((1-cos_r)/(norm_r*norm_r))*rsq;
  return ex;
}

vnl_double_3x3 bpgl_interpolate::A(vnl_double_3x3 const& r)
{
  vnl_double_3x3 a;
  double tol = 1.0e-10;
  double norm_r = std::sqrt(r[0][1]*r[0][1]+r[0][2]*r[0][2]+r[1][2]*r[1][2]);
  double norm_r_sq = norm_r*norm_r;
  double norm_r_cu = norm_r*norm_r_sq;
  if (norm_r<tol)//the case of an identity rotation
  {
    return a.set_identity();
  }
  double sin_r = std::sin(norm_r), cos_r = std::cos(norm_r);
  vnl_double_3x3 I, rsq = r*r;
  I.set_identity();
  a = I + ((1-cos_r)/norm_r_sq)*r + ((norm_r - sin_r)/norm_r_cu)*rsq;
  return a;
}

vnl_double_3x3 bpgl_interpolate::Ainv(vnl_double_3x3 const& r)
{
  vnl_double_3x3 ainv;
  double tol = 1.0e-10;
  double norm_r = std::sqrt(r[0][1]*r[0][1]+r[0][2]*r[0][2]+r[1][2]*r[1][2]);
  double norm_r_sq = norm_r*norm_r;
  if (norm_r<tol)//the case of an identity rotation
  {
    return ainv.set_identity();
  }
  double sin_r = std::sin(norm_r), cos_r = std::cos(norm_r);
  vnl_double_3x3 I, rsq = r*r;
  I.set_identity();
  ainv = I + 0.5*r + ((2.0*sin_r-norm_r*(1+cos_r))/(2*norm_r_sq*sin_r))*rsq;
  return ainv;
}

std::vector<vnl_double_3x3> bpgl_interpolate::interpolateR(vnl_double_3x3 R0,
                                                          vnl_double_3x3 R1,
                                                          unsigned n_between)
{
  vnl_double_3x3 r = R0.transpose()*R1;
  vnl_double_3x3 log_r = bpgl_interpolate::logR(r);
  //the rotation interpolation parameter, s, step spacing
  double s_interval = 1.0/(n_between+1);
  std::vector<vnl_double_3x3> temp;
  for (double s = s_interval; s<1.0; s+=s_interval)
  {
    vnl_double_3x3 R = bpgl_interpolate::expr(log_r*s);
    temp.push_back(R0*R);
  }
  return temp;
}


vnl_double_3x3 bpgl_interpolate::interpolateR(double alpha, vnl_double_3x3 R0, vnl_double_3x3 R1)
{
  vnl_double_3x3 r = R0.transpose()*R1;
  vnl_double_3x3 log_r = bpgl_interpolate::logR(r);

  vnl_double_3x3 R = bpgl_interpolate::expr(log_r*alpha);

  return R0*R;
}


void bpgl_interpolate::interpolateRt(vnl_double_3x3 R0,
                                     vnl_double_3 t0,
                                     vnl_double_3x3 R1,
                                     vnl_double_3 t1,
                                     unsigned n_between,
                                     std::vector<vnl_double_3x3>& Rintrp,
                                     std::vector<vnl_double_3>& tintrp)
{
  Rintrp.clear();
  tintrp.clear();
  vnl_double_3x3 r = R0.transpose()*R1;
  vnl_double_3 dt = t1 - t0;
  vnl_double_3x3 log_r = bpgl_interpolate::logR(r);
  //the rotation interpolation parameter, s, step spacing
  double s_interval = 1.0/(n_between+1);
  std::vector<vnl_double_3x3> temp;
  for (double s = s_interval; s<1.0; s+=s_interval)
  {
    vnl_double_3x3 d_log_r = log_r*s;
    vnl_double_3x3 R = bpgl_interpolate::expr(log_r*s);
    Rintrp.push_back(R0*R);
    vnl_double_3x3 a = bpgl_interpolate::A(d_log_r);
    vnl_double_3x3 ainv = bpgl_interpolate::Ainv(d_log_r);
    vnl_double_3 sadt = ainv*(s*dt);
    vnl_double_3 dlt = a*sadt;
    tintrp.emplace_back(t0+dlt);
  }
}

bool bpgl_interpolate::
interpolate(vpgl_perspective_camera<double> const& cam0,
            vpgl_perspective_camera<double> const& cam1,
            unsigned n_between,
            std::vector<vpgl_perspective_camera<double> >& cams)
{
  cams.clear();
  if (!n_between)
    return false;
  const vpgl_calibration_matrix<double>& K0 = cam0.get_calibration();
  const vpgl_calibration_matrix<double>& K1 = cam1.get_calibration();
  if (K0 != K1)
    return false;
  //interpolate camera center
  vgl_point_3d<double> c0 = cam0.get_camera_center();
  vgl_point_3d<double> c1 = cam1.get_camera_center();
  vnl_double_3 t0(c0.x(), c0.y(), c0.z());
  vnl_double_3 t1(c1.x(), c1.y(), c1.z());
#if 0
  vgl_vector_3d<double> v = (c1 - c0)/(1 + n_between);
  std::vector<vgl_point_3d<double> > centers;
  for (unsigned i = 0; i<n_between; ++i)
  {
    vgl_point_3d<double> cs = c0 + (i+1)*v;
    centers.push_back(cs);
  }
#endif
  //interpolate rotation
  const vgl_rotation_3d<double>& rot0 = cam0.get_rotation();
  const vgl_rotation_3d<double>& rot1 = cam1.get_rotation();
  vnl_double_3x3 R0 = rot0.as_matrix();
  vnl_double_3x3 R1 = rot1.as_matrix();
  std::vector<vnl_double_3x3> Rmats;
  std::vector<vnl_double_3> tvecs;
  bpgl_interpolate::interpolateRt(R0, t0, R1, t1, n_between, Rmats, tvecs);
  //  Rmats = bpgl_interpolate::interpolateR(R0,R1,n_between);
  // return the interpolated cameras
  for (unsigned i = 0; i<n_between; ++i)
  {
    vgl_rotation_3d<double> rot(Rmats[i]);
    vnl_double_3 t = tvecs[i];
    vgl_point_3d<double> p(t[0],t[1],t[2]);
    vpgl_perspective_camera<double> cam(K0, p, rot);
    cams.push_back(cam);
  }
  return true;
}

bool bpgl_interpolate::
interpolate_next(vpgl_perspective_camera<double> const& cam_prev,
                 vpgl_perspective_camera<double> const& cam_curr,
                 double const& rel_step_size,
                 vpgl_perspective_camera<double>& cam_next)
{
  const vpgl_calibration_matrix<double>& K_prev = cam_prev.get_calibration();
  const vpgl_calibration_matrix<double>& K_curr = cam_curr.get_calibration();
  if (K_prev != K_curr)
    return false;

  vgl_point_3d<double> c_prev = cam_prev.get_camera_center();
  vgl_point_3d<double> c_curr = cam_curr.get_camera_center();

  vnl_double_3 t_prev(c_prev.x(), c_prev.y(), c_prev.z());
  vnl_double_3 t_curr(c_curr.x(), c_curr.y(), c_curr.z());

  vnl_double_3x3 R_prev = cam_prev.get_rotation().as_matrix();
  vnl_double_3x3 R_curr = cam_curr.get_rotation().as_matrix();

  // change in rotation
  vnl_double_3x3 dR = R_prev.transpose()*R_curr;
  // change in translation
  vnl_double_3 dt = t_curr - t_prev;

  vnl_double_3x3 log_dR = bpgl_interpolate::logR(dR);
  vnl_double_3x3 dR_step = bpgl_interpolate::expr(log_dR*rel_step_size);

  vnl_double_3x3 R_next = R_curr*dR_step;

  vnl_double_3x3 a = bpgl_interpolate::A(log_dR*rel_step_size);
  vnl_double_3x3 ainv = bpgl_interpolate::Ainv(log_dR*rel_step_size);
  vnl_double_3 sadt = ainv*(rel_step_size*dt);
  vnl_double_3 dlt = a*sadt;
  vnl_double_3 t_next = t_curr + dlt;

  vgl_rotation_3d<double> rot_next(R_next);
  vgl_point_3d<double> p_next(t_next[0],t_next[1],t_next[2]);

  cam_next.set_calibration(K_curr);
  cam_next.set_camera_center(p_next);
  cam_next.set_rotation(rot_next);

  return true;
}
