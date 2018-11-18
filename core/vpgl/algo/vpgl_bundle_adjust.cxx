// This is vpgl/algo/vpgl_bundle_adjust.cxx
#include <fstream>
#include <algorithm>
#include "vpgl_bundle_adjust.h"
//:
// \file


#include <vnl/algo/vnl_sparse_lm.h>
#include <vnl/vnl_double_3.h>

#include <vgl/vgl_plane_3d.h>
#include <vpgl/algo/vpgl_ba_fixed_k_lsqr.h>
#include <vpgl/algo/vpgl_ba_shared_k_lsqr.h>
#include <vgl/algo/vgl_rotation_3d.h>

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif


vpgl_bundle_adjust::vpgl_bundle_adjust()
  : ba_func_(nullptr),
    use_m_estimator_(false),
    m_estimator_scale_(1.0),
    use_gradient_(true),
    self_calibrate_(false),
    normalize_data_(true),
    verbose_(false),
    max_iterations_(1000),
    x_tol_(1e-8),
    g_tol_(1e-8),
    epsilon_(1e-3),
    start_error_(0.0),
    end_error_(0.0)
{
}

vpgl_bundle_adjust::~vpgl_bundle_adjust()
{
  delete ba_func_;
}

//: normalize image points to be mean centered with scale sqrt(2)
//  \return parameters such that original point are recovered as (ns*x+nx, ns*y+ny)
void
vpgl_bundle_adjust::normalize_points(std::vector<vgl_point_2d<double> >& image_points,
                                     double& nx, double& ny, double& ns)
{
  nx = ny = ns = 0.0;
  for (auto & image_point : image_points)
  {
    double x = image_point.x();
    double y = image_point.y();
    nx += x;
    ny += y;
    ns += x*x + y*y;
  }
  nx /= image_points.size();
  ny /= image_points.size();
  ns /= image_points.size();
  ns -= nx*nx + ny*ny;
  ns /= 2;
  ns = std::sqrt(ns);
  for (auto & image_point : image_points)
  {
    image_point.x() -= nx;
    image_point.y() -= ny;
    image_point.x() /= ns;
    image_point.y() /= ns;
  }
}


// reflect the points about a plane
void
vpgl_bundle_adjust::
reflect_points(const vgl_plane_3d<double>& plane,
               std::vector<vgl_point_3d<double> >& points)
{
  vgl_h_matrix_3d<double> H;
  H.set_reflection_plane(plane);
  for (auto & point : points)
  {
    point = H * vgl_homg_point_3d<double>(point);
  }
}


// rotate the cameras 180 degrees around an axis
void
vpgl_bundle_adjust::
rotate_cameras(const vgl_vector_3d<double>& axis,
               std::vector<vpgl_perspective_camera<double> >& cameras)
{
  vnl_double_3 r(axis.x(), axis.y(), axis.z());
  r.normalize();
  r *= vnl_math::pi;
  vgl_rotation_3d<double> R(r);
  vgl_rotation_3d<double> R2(0.0, 0.0, vnl_math::pi);
  for (auto & c : cameras)
  {
    c.set_camera_center(R*c.get_camera_center());
    c.set_rotation(R2*c.get_rotation()*R);
  }
}


//: Approximately depth invert the scene.
//  Apply this and re-optimize to get out of a common local minimum.
//  Find the mean axis between cameras and points, mirror the points about
//  a plane perpendicular to this axis, and rotate the cameras 180 degrees
//  around this axis
void
vpgl_bundle_adjust::
depth_reverse(std::vector<vpgl_perspective_camera<double> >& cameras,
              std::vector<vgl_point_3d<double> >& points)
{
  vnl_double_3 pc(0.0,0.0,0.0), cc(0.0,0.0,0.0);
  // compute the mean of the points
  for (auto & point : points)
  {
    pc += vnl_double_3(point.x(), point.y(), point.z());
  }
  pc /= points.size();
  vgl_point_3d<double> point_center(pc[0],pc[1],pc[2]);

  // compute the mean of the camera centers
  for (auto & camera : cameras)
  {
    vgl_point_3d<double> c = camera.get_camera_center();
    cc += vnl_double_3(c.x(), c.y(), c.z());
  }
  cc /= cameras.size();
  vgl_point_3d<double> camera_center(cc[0],cc[1],cc[2]);

  // define the plane of reflection
  vgl_vector_3d<double> axis(camera_center-point_center);
  normalize(axis);
  vgl_plane_3d<double> reflect_plane(axis, point_center);

  reflect_points(reflect_plane,points);
  rotate_cameras(axis, cameras);
}


//: Bundle Adjust
bool
vpgl_bundle_adjust::optimize(std::vector<vpgl_perspective_camera<double> >& cameras,
                             std::vector<vgl_point_3d<double> >& world_points,
                             const std::vector<vgl_point_2d<double> >& image_points,
                             const std::vector<std::vector<bool> >& mask)
{
  delete ba_func_;

  double nx=0.0, ny=0.0, ns=1.0;
  std::vector<vgl_point_2d<double> > norm_image_points(image_points);
  if (normalize_data_)
    normalize_points(norm_image_points,nx,ny,ns);

  // construct the bundle adjustment function
  if (self_calibrate_)
  {
    // Extract the camera and point parameters
    vpgl_ba_shared_k_lsqr::create_param_vector(cameras,a_,c_);
    c_[0] /= ns;
    b_ = vpgl_ba_shared_k_lsqr::create_param_vector(world_points);
    // Compute the average calibration matrix
    vnl_vector<double> K_vals(5,0.0);
    for (auto & camera : cameras){
      const vpgl_calibration_matrix<double>& Ki = camera.get_calibration();
      K_vals[0] += Ki.focal_length()*Ki.x_scale();
      K_vals[1] += Ki.y_scale() / Ki.x_scale();
      K_vals[2] += Ki.principal_point().x();
      K_vals[3] += Ki.principal_point().y();
      K_vals[4] += Ki.skew();
    }
    K_vals /= cameras.size();
    vpgl_calibration_matrix<double> K(K_vals[0]/ns,
                                      vgl_point_2d<double>((K_vals[2]-nx)/ns,(K_vals[3]-ny)/ns),
                                      1.0,
                                      K_vals[1],
                                      K_vals[4]);
    ba_func_ = new vpgl_ba_shared_k_lsqr(K,norm_image_points,mask);
  }
  else
  {
    // Extract the camera and point parameters
    std::vector<vpgl_calibration_matrix<double> > K;
    a_ = vpgl_ba_fixed_k_lsqr::create_param_vector(cameras);
    b_ = vpgl_ba_fixed_k_lsqr::create_param_vector(world_points);
    for (auto & camera : cameras){
      vpgl_calibration_matrix<double> Ktmp = camera.get_calibration();
      if (normalize_data_)
      {
        Ktmp.set_focal_length(Ktmp.focal_length()/ns);
        vgl_point_2d<double> pp = Ktmp.principal_point();
        pp.x() = (pp.x()-nx)/ns;
        pp.y() = (pp.y()-ny)/ns;
        Ktmp.set_principal_point(pp);
      }
      K.push_back(Ktmp);
    }
    ba_func_ = new vpgl_ba_fixed_k_lsqr(K,norm_image_points,mask);
  }

  // apply normalization to the scale of residuals
  ba_func_->set_residual_scale(m_estimator_scale_/ns);

  // do the bundle adjustment
  vnl_sparse_lm lm(*ba_func_);
  lm.set_trace(true);
  lm.set_verbose(verbose_);

  lm.set_max_function_evals(max_iterations_);
  lm.set_x_tolerance(x_tol_);
  lm.set_g_tolerance(g_tol_);
  lm.set_epsilon_function(epsilon_);
  if (!lm.minimize(a_,b_,c_,use_gradient_,use_m_estimator_) &&
      lm.get_num_iterations() < int(max_iterations_))
  {
    return false;
  }

  if (use_m_estimator_)
  {
    weights_ = std::vector<double>(lm.get_weights().begin(), lm.get_weights().end());
  }
  else
  {
    weights_.clear();
    weights_.resize(image_points.size(),1.0);
  }

  if (self_calibrate_ && verbose_)
    std::cout << "final focal length = "<<c_[0]*ns<<std::endl;

  start_error_ = lm.get_start_error()*ns;
  end_error_ = lm.get_end_error()*ns;
  num_iterations_ = lm.get_num_iterations();

  // Update the camera parameters
  for (unsigned int i=0; i<cameras.size(); ++i)
  {
    cameras[i] = ba_func_->param_to_cam(i,a_,c_);
    if (normalize_data_)
    {
      // undo the normalization in the camera calibration
      vpgl_calibration_matrix<double> K = cameras[i].get_calibration();
      K.set_focal_length(K.focal_length()*ns);
      vgl_point_2d<double> pp = K.principal_point();
      pp.x() = ns*pp.x() + nx;
      pp.y() = ns*pp.y() + ny;
      K.set_principal_point(pp);
      cameras[i].set_calibration(K);
    }
  }
  // Update the point locations
  for (unsigned int j=0; j<world_points.size(); ++j)
    world_points[j] = ba_func_->param_to_point(j,b_,c_);

  return true;
}


//: Write cameras and points to a file in VRML 2.0 for debugging
void
vpgl_bundle_adjust::write_vrml(const std::string& filename,
                               const std::vector<vpgl_perspective_camera<double> >& cameras,
                               const std::vector<vgl_point_3d<double> >& world_points)
{
  std::ofstream os(filename.c_str());
  os << "#VRML V2.0 utf8\n\n";

  // vrml views are rotated 180 degrees around the X axis
  vgl_rotation_3d<double> rot180x(vnl_math::pi, 0.0, 0.0);

  for (unsigned int i=0; i<cameras.size(); ++i){
    vnl_double_3x3 K = cameras[i].get_calibration().get_matrix();

    vgl_rotation_3d<double> R = (rot180x*cameras[i].get_rotation()).inverse();
    vgl_point_3d<double> ctr = cameras[i].get_camera_center();
    double fov = 2.0*std::max(std::atan(K[1][2]/K[1][1]), std::atan(K[0][2]/K[0][0]));
    os  << "Viewpoint {\n"
        << "  position    "<< ctr.x() << ' ' << ctr.y() << ' ' << ctr.z() << '\n'
        << "  orientation "<< R.axis() << ' '<< R.angle() << '\n'
        << "  fieldOfView "<< fov << '\n'
        << "  description \"Camera" << i << "\"\n}\n";
  }

  os << "Shape {\n  appearance NULL\n    geometry PointSet {\n"
     << "      color Color { color [1 0 0] }\n      coord Coordinate{\n"
     << "       point[\n";

  for (const auto & world_point : world_points){
    os  << world_point.x() << ' '
        << world_point.y() << ' '
        << world_point.z() << '\n';
  }
  os << "   ]\n  }\n }\n}\n";

  os.close();
}
