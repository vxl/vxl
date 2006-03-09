// This is gel/mrc/vpgl/algo/vpgl_optimize_camera.cxx

//:
// \file

#include "vpgl_optimize_camera.h"
#include <vnl/vnl_rotation_matrix.h>
#include <vnl/vnl_quaternion.h>
#include <vnl/vnl_math.h>
#include <vnl/algo/vnl_levenberg_marquardt.h>

#include <vnl/vnl_det.h>


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
  vgl_h_matrix_3d<double> R(vnl_rotation_matrix(x), vnl_vector_fixed< double, 3 >(0.0));
  vpgl_perspective_camera<double> cam(K_,c_,R);
  for(unsigned int i=0; i<world_points_.size(); ++i)
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
  vnl_vector<double> w(x.data_block(),3);
  vgl_homg_point_3d<double> t(x[3], x[4], x[5]);
  vgl_h_matrix_3d<double> R(vnl_rotation_matrix(w), vnl_vector_fixed< double, 3 >(0.0));
  vpgl_perspective_camera<double> cam(K_,t,R);
  for(unsigned int i=0; i<world_points_.size(); ++i)
  {
    vgl_homg_point_2d<double> proj = cam(world_points_[i]);
    fx[2*i]   = image_points_[i].x() - proj.x()/proj.w();
    fx[2*i+1] = image_points_[i].y() - proj.y()/proj.w();
  }
}

/*
//: Called after each LM iteration to print debugging etc.
void
vpgl_orientation_position_lsqr::trace(int iteration,
                                       vnl_vector<double> const& x,
                                       vnl_vector<double> const& fx)
{
  vnl_vector<double> w(x.data_block(),3);
  vgl_homg_point_3d<double> t(x[3], x[4], x[5]);
  vgl_h_matrix_3d<double> R(vnl_rotation_matrix(w), vnl_vector_fixed< double, 3 >(0.0));
  vpgl_perspective_camera<double> cam(K_,t,R);
  //vcl_cout << "camera = \n" << cam.get_matrix() << vcl_endl; 
}  */


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
  vnl_matrix<double> R(camera.get_rotation_matrix().get_matrix().extract(3,3));
  
  // compute the Rodrigues vector from the rotation
  vnl_vector<double> w(3,0.0);
  w[0] = R(2,1)-R(1,2);
  w[1] = R(0,2)-R(2,0);
  w[2] = R(1,0)-R(0,1);
  w.normalize();
  double a = vcl_acos(vcl_sqrt( R(0,0)+R(1,1)+R(2,2)+1.0)/2.0)*2.0;
  w *=a;

  vpgl_orientation_lsqr lsqr_func(K,c,world_points,image_points);
  vnl_levenberg_marquardt lm(lsqr_func);
  lm.set_trace(true);
  lm.minimize(w);

  return vpgl_perspective_camera<double>(K,c,vgl_h_matrix_3d<double>(vnl_rotation_matrix(w),
                                                                      vnl_vector_fixed< double, 3 >(0.0)));
}


//: optimize orientation and position for a perspective camera
vpgl_perspective_camera<double>
vpgl_optimize_camera::opt_orient_pos(const vpgl_perspective_camera<double>& camera,
                                      const vcl_vector<vgl_homg_point_3d<double> >& world_points,
                                      const vcl_vector<vgl_point_2d<double> >& image_points )
{
  const vpgl_calibration_matrix<double>& K = camera.get_calibration();
  vnl_matrix<double> R(camera.get_rotation_matrix().get_matrix().extract(3,3));
  vgl_point_3d<double> c = camera.get_camera_center();

  // compute the Rodrigues vector from the rotation
  vnl_vector<double> w(3,0.0);
  w[0] = R(2,1)-R(1,2);
  w[1] = R(0,2)-R(2,0);
  w[2] = R(1,0)-R(0,1);
  w.normalize();
  double a = vcl_acos(vcl_sqrt( R(0,0)+R(1,1)+R(2,2)+1.0)/2.0)*2.0;
  w *=a;
  
  vpgl_orientation_position_lsqr lsqr_func(K,world_points,image_points);
  vnl_levenberg_marquardt lm(lsqr_func);
  vnl_vector<double> params(6);
  params[0]=w[0];  params[1]=w[1];  params[2]=w[2];
  params[3]=c.x();  params[4]=c.y();  params[5]=c.z();
  lm.set_trace(true);
  lm.minimize(params);
  vnl_vector<double> w_min(params.data_block(),3);
  vgl_homg_point_3d<double> c_min(params[3], params[4], params[5]);
  
  return vpgl_perspective_camera<double>(K,c_min,vgl_h_matrix_3d<double>(vnl_rotation_matrix(w_min),
                                                                          vnl_vector_fixed< double, 3 >(0.0)));
}



