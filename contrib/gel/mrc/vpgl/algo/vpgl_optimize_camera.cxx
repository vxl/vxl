// This is gel/mrc/vpgl/algo/vpgl_optimize_camera.cxx
#include "vpgl_optimize_camera.h"
//:
// \file
#include <vnl/vnl_rotation_matrix.h>
#include <vnl/vnl_double_3.h>
#include <vnl/vnl_double_3x3.h>
#include <vnl/algo/vnl_levenberg_marquardt.h>
#include <vcl_cassert.h>

//: Constructor
vpgl_orientation_lsqr::
  vpgl_orientation_lsqr(const vpgl_calibration_matrix<double>& K,
                        const vgl_point_3d<double>& c,
                        const vcl_vector<vgl_homg_point_3d<double> >& world_points,
                        const vcl_vector<vgl_point_2d<double> >& image_points )
 : vnl_least_squares_function(3,2*world_points.size(),no_gradient),
   K_(K),
   c_(c),
   world_points_(world_points),
   image_points_(image_points)
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
                                 const vcl_vector<vgl_homg_point_3d<double> >& world_points,
                                 const vcl_vector<vgl_point_2d<double> >& image_points )
 : vnl_least_squares_function(6,2*world_points.size(),no_gradient),
   K_(K),
   world_points_(world_points),
   image_points_(image_points)
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
  vcl_cout << "camera =\n" << cam.get_matrix() << vcl_endl;
#endif
}
#endif

//==============================================================================

//: Constructor
vpgl_orientation_position_calibration_lsqr::
vpgl_orientation_position_calibration_lsqr(const vcl_vector<vgl_homg_point_3d<double> >& world_points,
                                           const vcl_vector<vgl_point_2d<double> >& image_points )
 : vnl_least_squares_function(10,2*world_points.size(),no_gradient),
   world_points_(world_points),
   image_points_(image_points)
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
  vpgl_calibration_matrix<double> K(kk);
  vpgl_perspective_camera<double> cam(K, t, R);
  for (unsigned int i=0; i<world_points_.size(); ++i)
  {
    vgl_homg_point_2d<double> proj = cam(world_points_[i]);
    fx[2*i]   = image_points_[i].x() - proj.x()/proj.w();
    fx[2*i+1] = image_points_[i].y() - proj.y()/proj.w();
  }
}

//===============================================================
// Static functions for vpgl_optimize_camera
//===============================================================


//: optimize orientation for a perspective camera
vpgl_perspective_camera<double>
vpgl_optimize_camera::opt_orient(const vpgl_perspective_camera<double>& camera,
                                 const vcl_vector<vgl_homg_point_3d<double> >& world_points,
                                 const vcl_vector<vgl_point_2d<double> >& image_points )
{
  const vpgl_calibration_matrix<double>& K = camera.get_calibration();
  const vgl_point_3d<double>& c = camera.get_camera_center();
  const vgl_rotation_3d<double>& R = camera.get_rotation();

  // compute the Rodrigues vector from the rotation
  vnl_double_3 w = R.as_rodrigues();

  vpgl_orientation_lsqr lsqr_func(K,c,world_points,image_points);
  vnl_levenberg_marquardt lm(lsqr_func);
  lm.set_trace(true);
  lm.minimize(w);

  return vpgl_perspective_camera<double>(K, c, vgl_rotation_3d<double>(w) );
}


//: optimize orientation and position for a perspective camera
vpgl_perspective_camera<double>
vpgl_optimize_camera::opt_orient_pos(const vpgl_perspective_camera<double>& camera,
                                     const vcl_vector<vgl_homg_point_3d<double> >& world_points,
                                     const vcl_vector<vgl_point_2d<double> >& image_points )
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
  lm.set_trace(true);
  lm.minimize(params);
  vnl_double_3 w_min(params[0],params[1],params[2]);
  vgl_homg_point_3d<double> c_min(params[3], params[4], params[5]);

  return vpgl_perspective_camera<double>(K, c_min, vgl_rotation_3d<double>(w_min) );
}
// optimize all the parameters except internal skew
vpgl_perspective_camera<double>
vpgl_optimize_camera::opt_orient_pos_cal(const vpgl_perspective_camera<double>& camera,
                       const vcl_vector<vgl_homg_point_3d<double> >& world_points,
                       const vcl_vector<vgl_point_2d<double> >& image_points,
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
