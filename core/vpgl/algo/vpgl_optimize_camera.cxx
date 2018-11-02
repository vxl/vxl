// This is core/vpgl/algo/vpgl_optimize_camera.cxx
#include <utility>
#include "vpgl_optimize_camera.h"
//:
// \file
#include <vnl/vnl_rotation_matrix.h>
#include <vnl/vnl_double_3.h>
#include <vnl/vnl_double_3x3.h>
#include <vnl/algo/vnl_levenberg_marquardt.h>
#include <vgl/vgl_homg_point_2d.h>
#if 0
#include <vgl/algo/vgl_h_matrix_3d.h>
#endif
#include <vgl/algo/vgl_rotation_3d.h>
#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif


//: Constructor
vpgl_orientation_lsqr::
  vpgl_orientation_lsqr(const vpgl_calibration_matrix<double>& K,
                        const vgl_point_3d<double>& c,
                        const std::vector<vgl_homg_point_3d<double> >& world_points,
                        std::vector<vgl_point_2d<double> >  image_points )
 : vnl_least_squares_function(3,static_cast<unsigned int>(2*world_points.size()),no_gradient),
   K_(K),
   c_(c),
   world_points_(world_points),
   image_points_(std::move(image_points))
{
  assert(world_points_.size() == image_points_.size());
}


//: The main function.
//  Given the parameter vector x, compute the vector of residuals fx.
//  Fx has been sized appropriately before the call.
//  The parameters in x are the {wx, wy, wz}
//  where w is the Rodrigues vector of the rotation.
void
vpgl_orientation_lsqr::f(vnl_vector<double> const& x, vnl_vector<double>& fx)
{
  vpgl_perspective_camera<double> cam(K_,c_,vgl_rotation_3d<double>(x));
  for (unsigned int i=0; i<world_points_.size(); ++i)
  {
    vgl_homg_point_2d<double> proj = cam(world_points_[i]);
    fx[2*i]   = image_points_[i].x() - proj.x()/proj.w();
    fx[2*i+1] = image_points_[i].y() - proj.y()/proj.w();
  }
}

//==============================================================================

//: Constructor
vpgl_orientation_position_lsqr::
  vpgl_orientation_position_lsqr(const vpgl_calibration_matrix<double>& K,
                                 const std::vector<vgl_homg_point_3d<double> >& world_points,
                                 std::vector<vgl_point_2d<double> >  image_points )
 : vnl_least_squares_function(6, static_cast<unsigned int>(2 * world_points.size()), no_gradient),
   K_(K),
   world_points_(world_points),
   image_points_(std::move(image_points))
{
  assert(world_points_.size() == image_points_.size());
}


//: The main function.
//  Given the parameter vector x, compute the vector of residuals fx.
//  Fx has been sized appropriately before the call.
//  The parameters in x are really two three component vectors {wx, wy, wz, tx, ty, tz}
//  where w is the Rodrigues vector of the rotation and t is the translation.
void
vpgl_orientation_position_lsqr::f(vnl_vector<double> const& x, vnl_vector<double>& fx)
{
  assert(x.size() == 6);
  vnl_double_3 w(x[0], x[1], x[2]);
  vgl_homg_point_3d<double> t(x[3], x[4], x[5]);
  vpgl_perspective_camera<double> cam(K_,t,vgl_rotation_3d<double>(w));
  for (unsigned int i=0; i<world_points_.size(); ++i)
  {
    vgl_homg_point_2d<double> proj = cam(world_points_[i]);
    fx[2*i]   = image_points_[i].x() - proj.x()/proj.w();
    fx[2*i+1] = image_points_[i].y() - proj.y()/proj.w();
  }
}

#if 0
//: Called after each LM iteration to print debugging etc.
void
vpgl_orientation_position_lsqr::trace(int iteration,
                                      vnl_vector<double> const& x,
                                      vnl_vector<double> const& fx)
{
  assert(x.size() == 6);
  vnl_double_3 w(x[0], x[1], x[2]);
  vgl_homg_point_3d<double> t(x[3], x[4], x[5]);
  vgl_h_matrix_3d<double> R(vnl_rotation_matrix(w), vnl_double_3(0.0,0.0,0.0));
  vpgl_perspective_camera<double> cam(K_,t,R);
#ifdef DEBUG
  std::cout << "camera =\n" << cam.get_matrix() << std::endl;
#endif
}
#endif

//==============================================================================

//: Constructor
vpgl_orientation_position_calibration_lsqr::
vpgl_orientation_position_calibration_lsqr(const std::vector<vgl_homg_point_3d<double> >& world_points,
                                           std::vector<vgl_point_2d<double> >  image_points )
 : vnl_least_squares_function(10,2*world_points.size(),no_gradient),
   world_points_(world_points),
   image_points_(std::move(image_points))
{
  assert(world_points_.size() == image_points_.size());
}


//: The main function.
//  Given the parameter vector x, compute the vector of residuals fx.
//  Fx has been sized appropriately before the call.
//  The parameters in x are really two three component vectors {wx, wy, wz, tx, ty, tz}
//  where w is the Rodrigues vector of the rotation and t is the translation.
void
vpgl_orientation_position_calibration_lsqr::f(vnl_vector<double> const& x, vnl_vector<double>& fx)
{
  assert(x.size() == 10);
  vnl_double_3 w(x[0], x[1], x[2]);
  vgl_rotation_3d<double> R(w);
  vgl_homg_point_3d<double> t(x[3], x[4], x[5]);
  vnl_double_3x3 kk;
  kk.fill(0);
  kk[0][0]=x[6]; kk[0][2]=x[7];
  kk[1][1]=x[8]; kk[1][2]=x[9]; kk[2][2]=1.0;

  // Check that it is a valid calibration matrix.
  if ( !(kk[0][0]>0) || !(kk[1][1]>0) ) {
    for (unsigned int i=0; i<world_points_.size(); ++i) {
      fx[2*i]   = 100000000;
      fx[2*i+1] = 100000000;
    }
    return;
  }

  vpgl_calibration_matrix<double> K(kk);
  vpgl_perspective_camera<double> cam(K, t, R);
  for (unsigned int i=0; i<world_points_.size(); ++i)
  {
    vgl_homg_point_2d<double> proj = cam(world_points_[i]);
    fx[2*i]   = image_points_[i].x() - proj.x()/proj.w();
    fx[2*i+1] = image_points_[i].y() - proj.y()/proj.w();
  }
}

//: Constructor
vpgl_orientation_position_focal_lsqr::
vpgl_orientation_position_focal_lsqr(const vpgl_calibration_matrix<double>& K_init,
                                     const std::vector<vgl_homg_point_3d<double> >& world_points,
                                     std::vector<vgl_point_2d<double> >  image_points )
 : vnl_least_squares_function(8,2*world_points.size(),use_gradient),
   K_init_(K_init),
   world_points_(world_points),
   image_points_(std::move(image_points))
{
  assert(world_points_.size() == image_points_.size());
}


//: The main function.
//  Given the parameter vector x, compute the vector of residuals fx.
//  Fx has been sized appropriately before the call.
//  The parameters in x are quaternion, translation, focal length {q: x, y, z, r | t: t1, t2, t3 | f}
void
vpgl_orientation_position_focal_lsqr::f(vnl_vector<double> const& x, vnl_vector<double>& fx)
{
  assert(x.size() == 8);
  vnl_quaternion<double> q(x[0], x[1], x[2], x[3]);
  vgl_rotation_3d<double> R(q);
  vgl_vector_3d<double> t(x[4], x[5], x[6]);

  // Check that it is a valid focal length
  if (x[7]<=0) {
    for (unsigned int i=0; i<world_points_.size(); ++i) {
      fx[2*i]   = 100000000;
      fx[2*i+1] = 100000000;
    }
    return;
  }

  vpgl_calibration_matrix<double> K(K_init_);
  K.set_focal_length(x[7]);
  vpgl_perspective_camera<double> cam(K, R, t);
  for (unsigned int i=0; i<world_points_.size(); ++i)
  {
    vgl_homg_point_2d<double> proj = cam(world_points_[i]);
    fx[2*i]   = image_points_[i].x() - proj.x()/proj.w();
    fx[2*i+1] = image_points_[i].y() - proj.y()/proj.w();
  }
}

//: Gradients of the cost-function w.r.t. to the 8 free parameters of x
//  The eight parameters are quaternions(wx, wy, wz), translation(tx, ty, tz) and focal length (Fx)
void
vpgl_orientation_position_focal_lsqr::gradf(vnl_vector<double> const& xvec, vnl_matrix<double>& jacobian)
{
  // norm of unnormalized quaternion
  double norm = sqrt(xvec[0]*xvec[0] + xvec[1]*xvec[1] + xvec[2]*xvec[2] + xvec[3]*xvec[3]);
  vnl_quaternion<double> q(xvec[0], xvec[1], xvec[2], xvec[3]);
  vgl_rotation_3d<double> R(q);
  vnl_matrix_fixed<double, 3, 3> Rmat = R.as_matrix();
  // Quaternion gets normalized after getting passed to R
  q = R.as_quaternion();
  double x, y, z, r;
  x = q.x(); y = q.y(); z = q.z(); r = q.r();

  vgl_vector_3d<double> t(xvec[4], xvec[5], xvec[6]);

  vpgl_calibration_matrix<double> K(K_init_);
  K.set_focal_length(xvec[7]);
  vpgl_perspective_camera<double> cam(K, R, t);

  // The 3 rows of [R | t] matrix
  double A, B, C;
  // camera translation
  double t1, t2, t3;
  t1 = t.x(); t2 = t.y(); t3 = t.z();
  // 3D point coordinates
  double X, Y, Z, W;
  // Focal length
  double F = xvec[7];

  for (unsigned int idx=0; idx < world_points_.size(); ++idx)
  {
    X = world_points_[idx].x(); Y = world_points_[idx].y(); Z = world_points_[idx].z(); W = world_points_[idx].w();

    A = Rmat[0][0]*X + Rmat[0][1]*Y + Rmat[0][2]*Z + t1*W;
    B = Rmat[1][0]*X + Rmat[1][1]*Y + Rmat[1][2]*Z + t2*W;
    C = Rmat[2][0]*X + Rmat[2][1]*Y + Rmat[2][2]*Z + t3*W;

    double dex_dr1, dex_dr2, dex_dr3, dex_dr4, dey_dr1, dey_dr2, dey_dr3, dey_dr4;
    double dex_dt1, dey_dt1, dex_dt2, dey_dt2, dex_dt3, dey_dt3, dex_df, dey_df;

    dex_dr1 = 2*((W*t1*x + X*x + Y*y + Z*z)*C - (W*t3*x + X*z + Y*r - Z*x)*A)/(C*C);
    dex_dr2 = 2*((W*t1*y - X*y + Y*x + Z*r)*C - (W*t3*y - X*r + Y*z - Z*y)*A)/(C*C);
    dex_dr3 = 2*((W*t1*z - X*z - Y*r + Z*x)*C - (W*t3*z + X*x + Y*y + Z*z)*A)/(C*C);
    dex_dr4 = 2*((W*t1*r + X*r - Y*z + Z*y)*C - (W*t3*r - X*y + Y*x + Z*r)*A)/(C*C);

    dey_dr1 = 2*((W*t2*x + X*y - Y*x - Z*r)*C - (W*t3*x + X*z + Y*r - Z*x)*B)/(C*C);
    dey_dr2 = 2*((W*t2*y + X*x + Y*y + Z*z)*C - (W*t3*y - X*r + Y*z - Z*y)*B)/(C*C);
    dey_dr3 = 2*((W*t2*z + X*r - Y*z + Z*y)*C - (W*t3*z + X*x + Y*y + Z*z)*B)/(C*C);
    dey_dr4 = 2*((W*t2*r + X*z + Y*r - Z*x)*C - (W*t3*r - X*y + Y*x + Z*r)*B)/(C*C);

    dex_dr1 *= -F/norm; dex_dr2 *= -F/norm; dex_dr3 *= -F/norm; dex_dr4 *= -F/norm;
    dey_dr1 *= -F/norm; dey_dr2 *= -F/norm; dey_dr3 *= -F/norm; dey_dr4 *= -F/norm;

    dey_dt2 = dex_dt1 = -F/C;
    dex_dt2 = dey_dt1 = 0;
    dex_dt3 = F*A / (C*C);
    dey_dt3 = F*B / (C*C);
    dex_df = -A/C;
    dey_df = -B/C;

    jacobian(2*idx,0) = dex_dr1;
    jacobian(2*idx,1) = dex_dr2;
    jacobian(2*idx,2) = dex_dr3;
    jacobian(2*idx,3) = dex_dr4;

    jacobian(2*idx,4) = dex_dt1;
    jacobian(2*idx,5) = dex_dt2;
    jacobian(2*idx,6) = dex_dt3;
    jacobian(2*idx,7) = dex_df;

    jacobian(2*idx+1,0) = dey_dr1;
    jacobian(2*idx+1,1) = dey_dr2;
    jacobian(2*idx+1,2) = dey_dr3;
    jacobian(2*idx+1,3) = dey_dr4;

    jacobian(2*idx+1,4) = dey_dt1;
    jacobian(2*idx+1,5) = dey_dt2;
    jacobian(2*idx+1,6) = dey_dt3;
    jacobian(2*idx+1,7) = dey_df;
  }
}

//===============================================================
// Static functions for vpgl_optimize_camera
//===============================================================


//: optimize orientation for a perspective camera
vpgl_perspective_camera<double>
vpgl_optimize_camera::opt_orient(const vpgl_perspective_camera<double>& camera,
                                 const std::vector<vgl_homg_point_3d<double> >& world_points,
                                 const std::vector<vgl_point_2d<double> >& image_points )
{
  const vpgl_calibration_matrix<double>& K = camera.get_calibration();
  const vgl_point_3d<double>& c = camera.get_camera_center();
  const vgl_rotation_3d<double>& R = camera.get_rotation();

  // compute the Rodrigues vector from the rotation
  vnl_double_3 w = R.as_rodrigues();

  vpgl_orientation_lsqr lsqr_func(K,c,world_points,image_points);
  vnl_levenberg_marquardt lm(lsqr_func);
  //lm.set_trace(true);
  lm.minimize(w);

  return vpgl_perspective_camera<double>(K, c, vgl_rotation_3d<double>(w) );
}


//: optimize orientation and position for a perspective camera
vpgl_perspective_camera<double>
vpgl_optimize_camera::opt_orient_pos(const vpgl_perspective_camera<double>& camera,
                                     const std::vector<vgl_homg_point_3d<double> >& world_points,
                                     const std::vector<vgl_point_2d<double> >& image_points )
{
  const vpgl_calibration_matrix<double>& K = camera.get_calibration();
  vgl_point_3d<double> c = camera.get_camera_center();
  const vgl_rotation_3d<double>& R = camera.get_rotation();

  // compute the Rodrigues vector from the rotation
  vnl_double_3 w = R.as_rodrigues();

  vpgl_orientation_position_lsqr lsqr_func(K,world_points,image_points);
  vnl_levenberg_marquardt lm(lsqr_func);
  vnl_vector<double> params(6);
  params[0]=w[0];  params[1]=w[1];  params[2]=w[2];
  params[3]=c.x();  params[4]=c.y();  params[5]=c.z();
  //lm.set_trace(true);
  lm.minimize(params);
  vnl_double_3 w_min(params[0],params[1],params[2]);
  vgl_homg_point_3d<double> c_min(params[3], params[4], params[5]);

  return vpgl_perspective_camera<double>(K, c_min, vgl_rotation_3d<double>(w_min) );
}

// optimize orientation, position, and focal length
vpgl_perspective_camera<double>
vpgl_optimize_camera::opt_orient_pos_f(const vpgl_perspective_camera<double>& camera,
                                       const std::vector<vgl_homg_point_3d<double> >& world_points,
                                       const std::vector<vgl_point_2d<double> >& image_points,
                                       const double xtol, const unsigned nevals)
{
  const vpgl_calibration_matrix<double>& K = camera.get_calibration();
  vgl_vector_3d<double> t= camera.get_translation();
  const vgl_rotation_3d<double>& R = camera.get_rotation();
  vnl_quaternion<double> q = R.as_quaternion();

  vpgl_orientation_position_focal_lsqr lsqr_func(K, world_points,image_points);
  vnl_levenberg_marquardt lm(lsqr_func);

  vnl_vector<double> params(8);
  params[0] = q.x(); params[1] = q.y(); params[2] = q.z(); params[3] = q.r();
  params[4]=t.x();  params[5]=t.y();  params[6]=t.z();
  params[7]=K.focal_length();

  //  lm.set_trace(true);
  lm.set_x_tolerance(xtol);
  lm.set_f_tolerance(1e-2);
  lm.set_max_function_evals(nevals);
  lm.minimize(params);

  vnl_quaternion<double> q_min(params[0], params[1], params[2], params[3]);
  vgl_vector_3d<double> t_min(params[4], params[5], params[6]);
  double f_min = params[7];
  vpgl_calibration_matrix<double> K_min(K);
  K_min.set_focal_length(f_min);
  return vpgl_perspective_camera<double>(K_min, vgl_rotation_3d<double>(q_min), t_min);
}

// optimize all the parameters except internal skew
vpgl_perspective_camera<double>
vpgl_optimize_camera::opt_orient_pos_cal(const vpgl_perspective_camera<double>& camera,
                                         const std::vector<vgl_homg_point_3d<double> >& world_points,
                                         const std::vector<vgl_point_2d<double> >& image_points,
                                         const double xtol, const unsigned nevals)
{
  const vpgl_calibration_matrix<double>& K = camera.get_calibration();
  vgl_point_3d<double> c = camera.get_camera_center();
  const vgl_rotation_3d<double>& R = camera.get_rotation();
  vnl_double_3 w = R.as_rodrigues();

  vnl_double_3x3 kk = K.get_matrix();
  vpgl_orientation_position_calibration_lsqr lsqr_func(world_points,image_points);
  vnl_levenberg_marquardt lm(lsqr_func);
  vnl_vector<double> params(10);
  params[0]=w[0];  params[1]=w[1];  params[2]=w[2];
  params[3]=c.x();  params[4]=c.y();  params[5]=c.z();
  params[6]=kk[0][0]; params[7]=kk[0][2];
  params[8]=kk[1][1]; params[9]=kk[1][2];
  //  lm.set_trace(true);
  lm.set_x_tolerance(xtol);
  lm.set_max_function_evals(nevals);
  lm.minimize(params);
  vnl_double_3 w_min(params[0],params[1],params[2]);
  vgl_homg_point_3d<double> c_min(params[3], params[4], params[5]);
  vnl_double_3x3 kk_min;
  kk_min.fill(0); kk_min[2][2]=1.0;
   kk_min[0][0]=params[6];  kk_min[0][2]=params[7];
   kk_min[1][1]=params[8]; kk_min[1][2]=params[9];
   vpgl_calibration_matrix<double> K_min(kk_min);
  return vpgl_perspective_camera<double>(K_min, c_min, vgl_rotation_3d<double>(w_min));
}
