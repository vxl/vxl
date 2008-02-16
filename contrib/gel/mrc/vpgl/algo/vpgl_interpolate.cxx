#include "vpgl_interpolate.h"
#include <vnl/vnl_math.h>
#include <vnl/algo/vnl_complex_eigensystem.h>
#include <vcl_cmath.h>
#include <vcl_complex.h>
#include <vgl/vgl_point_3d.h>
vnl_double_3x3 vpgl_interpolate::logR(vnl_double_3x3 const& R)
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
  if(vcl_fabs(trace + 1.0)<tol)
    {
      vnl_double_3x3 I; I.fill(0.0);
      vnl_complex_eigensystem ce(R, I);
      for(unsigned i = 0; i<3; ++i){
        vcl_complex<double> eigenv = ce.eigen_value(i);
        
        if(vcl_fabs(eigenv.imag())<tol&&vcl_fabs(eigenv.real()-1.0)<tol)
          {
            vnl_vector<vcl_complex<double> > vr = 
              ce.right_eigen_vector(i);
            log_r[0][1] = -pi*vr[2].real(); log_r[0][2] =  pi*vr[1].real();
            log_r[1][0] =  pi*vr[2].real(); log_r[1][2] = -pi*vr[0].real();
            log_r[2][0] = -pi*vr[1].real(); log_r[2][1] =  pi*vr[0].real();;
            return log_r;
          }
      }
      vcl_cerr << "In vpgl_interpolate::log_r - shouldn't happen! FIX!!!!\n";
      return log_r;
    }
  //The usual case where a unique solution exists
  double cos_phi = 0.5*(trace -1);
  double sin_phi = vcl_sqrt(1.0 - cos_phi*cos_phi);
  double phi = vcl_asin(sin_phi);
  log_r = R - R.transpose();
  if(vcl_fabs(sin_phi)>tol)
    log_r *= (0.5*phi)/sin_phi;
  else log_r *= 0.5;
  return log_r;
}

vnl_double_3x3 vpgl_interpolate::expr(vnl_double_3x3 const& r)
{
  vnl_double_3x3 ex;
  double tol = 1.0e-10;
  double norm_r = vcl_sqrt(r[0][1]*r[0][1]+r[0][2]*r[0][2]+r[1][2]*r[1][2]);
  if(norm_r<tol)//the case of an identity rotation
    {
      ex.set_identity();
      return ex;
    }
  double sin_r = vcl_sin(norm_r), cos_r = vcl_cos(norm_r);
  vnl_double_3x3 I, rsq = r*r;
  I.set_identity();
  ex = I + (sin_r/norm_r)*r + ((1-cos_r)/(norm_r*norm_r))*rsq;
  return ex;
}

vnl_double_3x3 vpgl_interpolate::A(vnl_double_3x3 const& r)
{
  vnl_double_3x3 a;
  double tol = 1.0e-10;
  double norm_r = vcl_sqrt(r[0][1]*r[0][1]+r[0][2]*r[0][2]+r[1][2]*r[1][2]);
  double norm_r_sq = norm_r*norm_r;
  double norm_r_cu = norm_r*norm_r_sq;
  if(norm_r<tol)//the case of an identity rotation
    {
      a.set_identity();
      return a;
    }
  double sin_r = vcl_sin(norm_r), cos_r = vcl_cos(norm_r);
  vnl_double_3x3 I, rsq = r*r;
  I.set_identity();
  a = I + ((1-cos_r)/norm_r_sq)*r + ((norm_r - sin_r)/norm_r_cu)*rsq;
  return a;
}

vnl_double_3x3 vpgl_interpolate::Ainv(vnl_double_3x3 const& r)
{
  vnl_double_3x3 ainv;
  double tol = 1.0e-10;
  double norm_r = vcl_sqrt(r[0][1]*r[0][1]+r[0][2]*r[0][2]+r[1][2]*r[1][2]);
  double norm_r_sq = norm_r*norm_r;
  if(norm_r<tol)//the case of an identity rotation
    {
      ainv.set_identity();
      return ainv;
    }
  double sin_r = vcl_sin(norm_r), cos_r = vcl_cos(norm_r);
  vnl_double_3x3 I, rsq = r*r;
  I.set_identity();
  ainv = I + 0.5*r + ((2.0*sin_r-norm_r*(1+cos_r))/(2*norm_r_sq*sin_r))*rsq;
  return ainv;
}

vcl_vector<vnl_double_3x3> vpgl_interpolate::interpolateR(vnl_double_3x3 R0,
                                                          vnl_double_3x3 R1,
                                                          unsigned n_between)
{
  vnl_double_3x3 r = R0.transpose()*R1;
  vnl_double_3x3 log_r = vpgl_interpolate::logR(r);
  //the rotation interpolation parameter, s, step spacing
  double s_interval = 1.0/(n_between+1);
  vcl_vector<vnl_double_3x3> temp;
  for(double s = s_interval; s<1.0; s+=s_interval)
    {
      vnl_double_3x3 R = vpgl_interpolate::expr(log_r*s);
      temp.push_back(R0*R);
    }
  return temp;
}

void vpgl_interpolate::interpolateRt(vnl_double_3x3 R0,
                                     vnl_double_3 t0,
                                     vnl_double_3x3 R1,
                                     vnl_double_3 t1,
                                     unsigned n_between,
                                     vcl_vector<vnl_double_3x3>& Rintrp,
                                     vcl_vector<vnl_double_3>& tintrp)
{
  Rintrp.clear();
  tintrp.clear();
  vnl_double_3x3 r = R0.transpose()*R1;
  vnl_double_3 dt = t1 - t0;
  vnl_double_3x3 log_r = vpgl_interpolate::logR(r);
  //the rotation interpolation parameter, s, step spacing
  double s_interval = 1.0/(n_between+1);
  vcl_vector<vnl_double_3x3> temp;
  for(double s = s_interval; s<1.0; s+=s_interval)
    {
      vnl_double_3x3 d_log_r = log_r*s;
      vnl_double_3x3 R = vpgl_interpolate::expr(log_r*s);
      Rintrp.push_back(R0*R);
       vnl_double_3x3 a = vpgl_interpolate::A(d_log_r);
       vnl_double_3x3 ainv = vpgl_interpolate::Ainv(d_log_r);
       vnl_double_3 sadt = ainv*(s*dt);
       vnl_double_3 dlt = a*sadt;
       tintrp.push_back(t0+dlt);
    }
}

bool vpgl_interpolate::
interpolate(vpgl_perspective_camera<double> const& cam0,
            vpgl_perspective_camera<double> const& cam1,
            unsigned n_between,
            vcl_vector<vpgl_perspective_camera<double> >& cams)
{
  cams.clear();
  if(!n_between)
    return false;
  vpgl_calibration_matrix<double> K0 = cam0.get_calibration();
  vpgl_calibration_matrix<double> K1 = cam1.get_calibration();
  if(K0 != K1)
    return false;
  //interpolate camera center
  vgl_point_3d<double> c0 = cam0.get_camera_center();
  vgl_point_3d<double> c1 = cam1.get_camera_center();
  vnl_double_3 t0(c0.x(), c0.y(), c0.z());
  vnl_double_3 t1(c1.x(), c1.y(), c1.z());
#if 0
  vgl_vector_3d<double> v = (c1 - c0)/(1 + n_between);
  vcl_vector<vgl_point_3d<double> > centers;
  for(unsigned i = 0; i<n_between; ++i)
    {
      vgl_point_3d<double> cs = c0 + (i+1)*v;
      centers.push_back(cs);
    }
#endif
  //interpolate rotation
  vgl_rotation_3d<double> rot0 = cam0.get_rotation();
  vgl_rotation_3d<double> rot1 = cam1.get_rotation();
  vnl_double_3x3 R0 = rot0.as_matrix();
  vnl_double_3x3 R1 = rot1.as_matrix();
  vcl_vector<vnl_double_3x3> Rmats;
  vcl_vector<vnl_double_3> tvecs;
  vpgl_interpolate::interpolateRt(R0, t0, R1, t1, n_between, Rmats, tvecs);
  //  Rmats = vpgl_interpolate::interpolateR(R0,R1,n_between);
  // return the interpolated cameras
  for(unsigned i = 0; i<n_between; ++i)
    {
      vgl_rotation_3d<double> rot(Rmats[i]);
      vnl_double_3 t = tvecs[i];
      vgl_point_3d<double> p(t[0],t[1],t[2]);
      vpgl_perspective_camera<double> cam(K0, p, rot);
      cams.push_back(cam);
    }
  return true;
}
