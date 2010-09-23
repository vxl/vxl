#include "vpgl_camera_bounds.h"
#include <vpgl/algo/vpgl_project.h>
#include <vpgl/algo/vpgl_ray.h>
#include <vpgl/vpgl_calibration_matrix.h>
#include <vgl/algo/vgl_rotation_3d.h>
#include <vcl_cmath.h>
#include <vnl/vnl_math.h>
  ///
static double mod_two_pi(double angle)
{
  double two_pi = 2.0*vnl_math::pi;
  unsigned nmod = static_cast<unsigned>(angle/two_pi);
  double mod_ang = nmod*two_pi;
  return angle - mod_ang;
}
principal_ray_scan::principal_ray_scan(double cone_half_angle,
                                       unsigned& n_samples)
{
  //temporary array for samples
  vcl_vector<double> theta(n_samples+1);
  vcl_vector<double> phi(n_samples+1);
  //find the fraction of the sphere defined by the cone half angle
  double cone_solid_angle = 2.0*vnl_math::pi*(1.0-vcl_cos(cone_half_angle));
  double fraction_of_sphere = cone_solid_angle/(4.0*vnl_math::pi);
  //number of points that would cover the entire sphere
  unsigned limit = 
    static_cast<unsigned>(n_samples/fraction_of_sphere);
  //the following algorithm assumes that the entire sphere is uniformly
  //sampled
  // ======   sphere sampling with the spiral algorithm =======
  double p = 0.5;
  double ns = static_cast<double>(limit);
  double a = 1.0 - 2.0*p/(limit-3);
  double b = p*(limit+1)/(limit-3);
  double rkm1 = 0;
  // spiral starts at South pole of sphere
  unsigned k = 1;
  theta[k-1] = vnl_math::pi;
  phi[k-1]  = 0;
  double elv = 0;
  for(k = 2; k<=(limit-1)&&elv<=cone_half_angle; ++k){
    double kp       = a*k + b;
    double h   = -1.0 + 2*(kp-1.0)/(limit-1);
    double rk     = vcl_sqrt(1-h*h);
    theta[k-1] = vcl_acos(h);
    // cut off the scan when the elevation reaches the cone half angle
    elv = vnl_math::pi-theta[k-1];
    double temp  = phi[k-2] + 3.6/vcl_sqrt(ns)*2/(rkm1+rk);
    phi[k-1] = mod_two_pi(temp);
    rkm1 = rk;
  }
  //if the entire sphere is covered then add the North pole
  if(limit == n_samples){
    theta[n_samples-1] = 0.0;
    phi[n_samples-1]   = 0.0;
  }
  //adjust for the actual number of samples 
  if((k-1)<n_samples)
    n_samples = k-1;
  theta_.resize(n_samples);
  phi_.resize(n_samples);
  //move samples to the North pole so the scan is about the identity rotation
  for(unsigned i = 0; i<n_samples; ++i)
    {
      theta_[i]=vnl_math::pi-theta[i];
      phi_[i]=phi[i];
    }
  index_ = 0;
}
void principal_ray_scan::reset(){index_ = 0;}
bool principal_ray_scan::next(){return (++index_<theta_.size());}

vgl_point_3d<double> principal_ray_scan::pt_on_unit_sphere(unsigned i)
{
  double th = theta_[i], ph = phi_[i];
  double st = vcl_sin(th), ct = vcl_cos(th);
  double x = st*vcl_cos(ph), y = st*vcl_sin(ph), z = ct;
  return vgl_point_3d<double>(x, y, z);
}

vgl_rotation_3d<double> principal_ray_scan::rot(unsigned i, double alpha)
{
  double th = theta_[i], ph = phi_[i];
  double st = vcl_sin(th), ct = vcl_cos(th);
  //principal axis
  double x = st*vcl_cos(ph), y = st*vcl_sin(ph), z = ct;
  vnl_vector_fixed<double, 3> za(0.0, 0.0, 1.0), v(x, y, z);
  //rotation from z axis to principal axis.
  vgl_rotation_3d<double> R_axis(za, v);
  // alpha rotation as a Rodrigues vector
  vnl_vector_fixed<double, 3> vr = alpha*v;
  vgl_rotation_3d<double> R_about_axis(vr);
  return R_about_axis*R_axis;
}

// the solid angle for a pixel, applies only to perspective camera
// cone is tangent to pixel
void vpgl_camera_bounds::
pixel_solid_angle(vpgl_perspective_camera<double> const& cam,
                                unsigned u, unsigned v,
                                vgl_ray_3d<double>& cone_axis,
                                double& cone_half_angle,
                                double& solid_angle)
{
  cone_axis = cam.backproject(u+0.5, v+0.5);
  //get ray through upper left corner
  vgl_ray_3d<double> ul;
  ul = cam.backproject(u, v);
  cone_half_angle = angle(ul, cone_axis);
  solid_angle = 2.0*vnl_math::pi*(1.0-vcl_cos(cone_half_angle));
}

// the solid angle for an image, applies only to perspective camera
// cone axis passes through principal point, i.e. principal ray.
// tangent to a square defined by image diagonal
void vpgl_camera_bounds::
image_solid_angle(vpgl_perspective_camera<double> const& cam,
                                vgl_ray_3d<double>& cone_axis,
                                double& cone_half_angle,
                                double& solid_angle)
{
  vgl_point_2d<double> pp = (cam.get_calibration()).principal_point();
  cone_axis = cam.backproject(pp);
  vgl_ray_3d<double> ul = cam.backproject(0.0, 0.0);
  cone_half_angle = angle(ul, cone_axis);
  solid_angle = 2.0*vnl_math::pi*(1.0-vcl_cos(cone_half_angle));
}
  // the solid angle for a scene bounding box, the cone is tangent to the box
bool vpgl_camera_bounds::
box_solid_angle(vpgl_perspective_camera<double> const& cam,
                              vgl_box_3d<double> const& box,
                              vgl_ray_3d<double>& cone_axis,
                              double& cone_half_angle,
                              double& solid_angle)
{
  //project the box into the image
	vgl_box_2d<double> b2d = vpgl_project::project_bounding_box(cam, box);
  if(b2d.min_x()<0||b2d.min_y()<0)
    return false;//box falls outside the image
  vgl_point_2d<double> pp = cam.get_calibration().principal_point();
  if(b2d.max_x()>=2*pp.x()||b2d.max_y()>=2*pp.y())
    return false;//box falls outside the image
  //ray corresponding to box center
  if(!vpgl_ray::ray(cam, box.centroid(), cone_axis))
    return false;
  double umin = b2d.min_x(), vmin = b2d.min_y();//assume corners are centered
  vgl_ray_3d<double> ul = cam.backproject(umin, vmin);
  cone_half_angle = angle(ul, cone_axis);
  solid_angle = 2.0*vnl_math::pi*(1.0-vcl_cos(cone_half_angle));
  return true;
}

double vpgl_camera_bounds::
rotation_angle_interval(vpgl_perspective_camera<double> const& cam)
{
  //Get the principal point
  vgl_point_2d<double> pp = cam.get_calibration().principal_point();
  double rmin = pp.y();
  if(pp.x()<rmin) rmin = pp.x();
  if(rmin <= 0) return 0;
  //half length is 0.5 (1/2 pixel)
  double half_angle = vcl_atan(0.5/rmin);
  return 2.0*half_angle;
}
