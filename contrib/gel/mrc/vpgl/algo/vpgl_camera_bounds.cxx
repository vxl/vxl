#include "vpgl_camera_bounds.h"
#include <vpgl/algo/vpgl_project.h>
#include <vpgl/algo/vpgl_ray.h>
#include <vpgl/vpgl_calibration_matrix.h>
#include <vgl/algo/vgl_rotation_3d.h>
#include <vgl/vgl_closest_point.h>
#include <vcl_cmath.h>
#include <vnl/vnl_math.h>

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
  unsigned limit = static_cast<unsigned>(n_samples/fraction_of_sphere);
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
  for (k = 2; k<=(limit-1)&&elv<=cone_half_angle; ++k) {
    double kp  = a*k + b;
    double h   = -1.0 + 2*(kp-1.0)/(limit-1);
    double rk  = vcl_sqrt(1-h*h);
    theta[k-1] = vcl_acos(h);
    // cut off the scan when the elevation reaches the cone half angle
    elv = vnl_math::pi-theta[k-1];
    double temp  = phi[k-2] + 3.6/vcl_sqrt(ns)*2/(rkm1+rk);
    phi[k-1] = mod_two_pi(temp);
    rkm1 = rk;
  }
  //if the entire sphere is covered then add the North pole
  if (limit == n_samples) {
    theta[n_samples-1] = 0.0;
    phi[n_samples-1]   = 0.0;
  }
  //adjust for the actual number of samples
  if ((k-1)<n_samples)
    n_samples = k-1;
  theta_.resize(n_samples);
  phi_.resize(n_samples);
  //move samples to the North pole so the scan is about the identity rotation
  for (unsigned i = 0; i<n_samples; ++i)
  {
    theta_[i]=vnl_math::pi-theta[i];
    phi_[i]=phi[i];
  }
  index_ = 0;
}

void principal_ray_scan::reset() { index_ = -1; }

bool principal_ray_scan::next()
{return ++index_<static_cast<int>(theta_.size());}

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
  //rotation from principal axis to z axis
  vgl_rotation_3d<double> R_axis(v, za);
  // alpha rotation as a Rodrigues vector
  vnl_vector_fixed<double, 3> vr = alpha*za;
  vgl_rotation_3d<double> R_about_axis(vr);
  return R_about_axis*R_axis;
}

double vpgl_camera_bounds::solid_angle(double cone_half_angle)
{
  return 2.0*vnl_math::pi*(1.0-vcl_cos(cone_half_angle));
}

double vpgl_camera_bounds::cone_half_angle(double solid_angle)
{
  double temp = solid_angle/(2.0*vnl_math::pi);
  temp = vcl_acos(1.0-temp);
  return temp;
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
  solid_angle = vpgl_camera_bounds::solid_angle(cone_half_angle);
}

// solid angle at principal point
void vpgl_camera_bounds::
pixel_solid_angle(vpgl_perspective_camera<double> const& cam,
                  double& cone_half_angle,
                  double& solid_angle)
{
  vgl_point_2d<double> pp = (cam.get_calibration()).principal_point();
  unsigned u = static_cast<unsigned>(pp.x()),
           v = static_cast<unsigned>(pp.y());
  vgl_ray_3d<double> ray;
  vpgl_camera_bounds::pixel_solid_angle(cam, u, v, ray, cone_half_angle,
                                        solid_angle);
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
  solid_angle = vpgl_camera_bounds::solid_angle(cone_half_angle);
}

void vpgl_camera_bounds::
image_solid_angle(vpgl_perspective_camera<double> const& cam,
                  double& cone_half_angle,
                  double& solid_angle)
{
  vgl_ray_3d<double> ray;
  vpgl_camera_bounds::image_solid_angle(cam, ray, cone_half_angle,
                                        solid_angle);
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
  if (b2d.min_x()<0||b2d.min_y()<0)
    return false;//box falls outside the image
  vgl_point_2d<double> pp = cam.get_calibration().principal_point();
  if (b2d.max_x()>=2*pp.x()||b2d.max_y()>=2*pp.y())
    return false;//box falls outside the image
  //ray corresponding to box center
  if (!vpgl_ray::ray(cam, box.centroid(), cone_axis))
    return false;
  double umin = b2d.min_x(), vmin = b2d.min_y();//assume corners are centered
  vgl_ray_3d<double> ul = cam.backproject(umin, vmin);
  cone_half_angle = angle(ul, cone_axis);
  solid_angle = vpgl_camera_bounds::solid_angle(cone_half_angle);
  return true;
}

double vpgl_camera_bounds::
rotation_angle_interval(vpgl_perspective_camera<double> const& cam)
{
  //Get the principal point
  vgl_point_2d<double> pp = cam.get_calibration().principal_point();
  double rmin = pp.y();
  if (pp.x()<rmin) rmin = pp.x();
  if (rmin <= 0) return 0;
  //half length is 0.5 (1/2 pixel)
  double half_angle = vcl_atan(0.5/rmin);
  return 2.0*half_angle;
}
#if 0 // moved to vpgl_ray
double vpgl_camera_bounds::angle_between_rays(vgl_rotation_3d<double> const& r0,
                                              vgl_rotation_3d<double> const& r1)
{
  vnl_vector_fixed<double, 3> zaxis, a0, a1;
  zaxis[0]=0.0;  zaxis[1]=0.0;  zaxis[2]=1.0;
  vgl_rotation_3d<double> r0i = r0.inverse(), r1i = r1.inverse();
  a0 = r0i*zaxis; a1 = r1i*zaxis;
  double dp = dot_product(a0, a1);
  return vcl_acos(dp);
}

double vpgl_camera_bounds::
rot_about_ray(vgl_rotation_3d<double> const& r0, vgl_rotation_3d<double> const& r1)
{
  // find axes for each rotation
  vnl_vector_fixed<double, 3> zaxis, a0, a1;
  zaxis[0]=0.0;  zaxis[1]=0.0;  zaxis[2]=1.0;
  vgl_rotation_3d<double> r0i = r0.inverse(), r1i = r1.inverse();
  a0 = r0i*zaxis; a1 = r1i*zaxis;
  // find the transforms that map the z-axis to each axis
  vgl_rotation_3d<double> r0b(zaxis, a0), r1b(zaxis,a1);
  //  find rotations about z axis
  vgl_rotation_3d<double> r0_alpha = r0*r0b, r1_alpha = r1*r1b;
  vnl_vector_fixed<double, 3> r0_alpha_rod = r0_alpha.as_rodrigues(), r1_alpha_rod = r1_alpha.as_rodrigues();
  // get angle difference
  double ang0 = r0_alpha_rod.magnitude(), ang1 = r1_alpha_rod.magnitude();
  return vcl_fabs(ang0-ang1);
}
#endif
void vpgl_camera_bounds::
relative_transf(vpgl_perspective_camera<double> const& c0,
                vpgl_perspective_camera<double> const& c1,
                vgl_rotation_3d<double>& rel_rot,
                vgl_vector_3d<double>& rel_trans)
{
  vgl_vector_3d<double> t0 = c0.get_translation();
  vgl_vector_3d<double> t1 = c1.get_translation();
  vgl_rotation_3d<double> R0 = c0.get_rotation();
  vgl_rotation_3d<double> R1 = c1.get_rotation();
  rel_rot = R1*(R0.transpose());
  vgl_vector_3d<double> td = rel_rot*t0;
  rel_trans = -td + t1;
}

bool vpgl_camera_bounds::pixel_cylinder(vpgl_generic_camera<double> const& cam,
                                        unsigned u, unsigned v,
                                        vgl_ray_3d<double>& cylinder_axis,
                                        double& cylinder_radius)
{
  unsigned nc = cam.cols(), nr = cam.rows();
  cylinder_axis = cam.ray(u, v);
  vgl_point_3d<double> axis_origin = cylinder_axis.origin();
  vgl_ray_3d<double> corner_ray;
  if(u>0&&v>0&&u<nc&&v<nr)
    corner_ray = cam.ray(u-0.5, v-0.5);
  else if(u>0&&v==0&&u<nc)
    corner_ray = cam.ray(u-0.5, v+0.5);
  else if(u==0&&v>0&&v<nr)
    corner_ray = cam.ray(u+0.5, v-0.5);
  else if(u==0&&v==0)
    corner_ray = cam.ray(u+0.5, v+0.5);
  else{
    cylinder_radius = 0;
    return false;
  }
  vgl_point_3d<double> cp = vgl_closest_point(axis_origin, corner_ray);
  cylinder_radius = (axis_origin-cp).length();
  return true;
}
