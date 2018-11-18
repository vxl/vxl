#include <iostream>
#include <cmath>
#include "vsph_camera_bounds.h"
#include <bpgl/algo/bpgl_project.h>
#include <vpgl/algo/vpgl_ray.h>
#include <vgl/algo/vgl_rotation_3d.h>
#include <vgl/vgl_closest_point.h>
#include <vgl/vgl_plane_3d.h>
#include <vgl/vgl_intersection.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vnl/vnl_math.h>

principal_ray_scan::principal_ray_scan(double cone_half_angle,
                                       unsigned& n_samples)
{
  //temporary array for samples
  std::vector<double> theta(n_samples+1);
  std::vector<double> phi(n_samples+1);
  //find the fraction of the sphere defined by the cone half angle
  double cone_solid_angle = vnl_math::twopi*(1.0-std::cos(cone_half_angle));
  double fraction_of_sphere = cone_solid_angle/(4.0*vnl_math::pi);
  //number of points that would cover the entire sphere
  auto limit = static_cast<unsigned>(n_samples/fraction_of_sphere);
  //the following algorithm assumes that the entire sphere is uniformly
  //sampled
  // ======   sphere sampling with the spiral algorithm =======
  double p = 0.5;
  auto ns = static_cast<double>(limit);
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
    double rk  = std::sqrt(1-h*h);
    theta[k-1] = std::acos(h);
    // cut off the scan when the elevation reaches the cone half angle
    elv = vnl_math::pi-theta[k-1];
    double temp  = phi[k-2] + 3.6/std::sqrt(ns)*2/(rkm1+rk);
    phi[k-1] = vnl_math::angle_0_to_2pi(temp);
    rkm1 = rk;
  }
  //if the entire sphere is covered then add the North pole
  if ( limit == n_samples) {
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

vgl_point_3d<double> principal_ray_scan::pt_on_unit_sphere(unsigned i) const
{
  double th = theta_[i], ph = phi_[i];
  double st = std::sin(th), ct = std::cos(th);
  double x = st*std::cos(ph), y = st*std::sin(ph), z = ct;
  return {x, y, z};
}

vgl_rotation_3d<double> principal_ray_scan::rot(unsigned i, double alpha) const
{
  double th = theta_[i], ph = phi_[i];
  double st = std::sin(th), ct = std::cos(th);
  //principal axis
  double x = st*std::cos(ph), y = st*std::sin(ph), z = ct;
  vnl_vector_fixed<double, 3> za(0.0, 0.0, 1.0), v(x, y, z);
  //rotation from principal axis to z axis
  vgl_rotation_3d<double> R_axis(v, za);
  // alpha rotation as a Rodrigues vector
  vnl_vector_fixed<double, 3> vr = alpha*za;
  vgl_rotation_3d<double> R_about_axis(vr);
  return R_about_axis*R_axis;
}

double vsph_camera_bounds::solid_angle(double cone_half_angle)
{
  return vnl_math::twopi*(1.0-std::cos(cone_half_angle));
}

double vsph_camera_bounds::cone_half_angle(double solid_angle)
{
  double temp = solid_angle/vnl_math::twopi;
  temp = std::acos(1.0-temp);
  return temp;
}

// the solid angle for a pixel, applies only to perspective camera
// cone is tangent to pixel
void vsph_camera_bounds::
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
  solid_angle = vsph_camera_bounds::solid_angle(cone_half_angle);
}

// solid angle at principal point
void vsph_camera_bounds::
pixel_solid_angle(vpgl_perspective_camera<double> const& cam,
                  double& cone_half_angle,
                  double& solid_angle)
{
  vgl_point_2d<double> pp = cam.get_calibration().principal_point();
  auto u = static_cast<unsigned>(pp.x()),
           v = static_cast<unsigned>(pp.y());
  vgl_ray_3d<double> ray;
  vsph_camera_bounds::pixel_solid_angle(cam, u, v, ray, cone_half_angle,
                                        solid_angle);
}

// the solid angle for an image, applies only to perspective camera
// cone axis passes through principal point, i.e. principal ray.
// tangent to a square defined by image diagonal
void vsph_camera_bounds::
image_solid_angle(vpgl_perspective_camera<double> const& cam,
                  vgl_ray_3d<double>& cone_axis,
                  double& cone_half_angle,
                  double& solid_angle)
{
  vgl_point_2d<double> pp = cam.get_calibration().principal_point();
  cone_axis = cam.backproject(pp);
  vgl_ray_3d<double> ul = cam.backproject(0.0, 0.0);
  cone_half_angle = angle(ul, cone_axis);
  solid_angle = vsph_camera_bounds::solid_angle(cone_half_angle);
}

void vsph_camera_bounds::
image_solid_angle(vpgl_perspective_camera<double> const& cam,
                  double& cone_half_angle,
                  double& solid_angle)
{
  vgl_ray_3d<double> ray;
  vsph_camera_bounds::image_solid_angle(cam, ray, cone_half_angle,
                                        solid_angle);
}

  // the solid angle for a scene bounding box, the cone is tangent to the box
bool vsph_camera_bounds::
box_solid_angle(vpgl_perspective_camera<double> const& cam,
                vgl_box_3d<double> const& box,
                vgl_ray_3d<double>& cone_axis,
                double& cone_half_angle,
                double& solid_angle)
{
  //project the box into the image
  vgl_box_2d<double> b2d = bpgl_project::project_bounding_box(cam, box);
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
  solid_angle = vsph_camera_bounds::solid_angle(cone_half_angle);
  return true;
}

double vsph_camera_bounds::
rotation_angle_interval(vpgl_perspective_camera<double> const& cam)
{
  //Get the principal point
  vgl_point_2d<double> pp = cam.get_calibration().principal_point();
  double rmin = pp.y();
  if (pp.x()<rmin) rmin = pp.x();
  if (rmin <= 0) return 0;
  //half length is 0.5 (1/2 pixel)
  double half_angle = std::atan(0.5/rmin);
  return 2.0*half_angle;
}

void vsph_camera_bounds::
relative_transf(vpgl_perspective_camera<double> const& c0,
                vpgl_perspective_camera<double> const& c1,
                vgl_rotation_3d<double>& rel_rot,
                vgl_vector_3d<double>& rel_trans)
{
  vgl_vector_3d<double> t0 = c0.get_translation();
  vgl_vector_3d<double> t1 = c1.get_translation();
  const vgl_rotation_3d<double>& R0 = c0.get_rotation();
  const vgl_rotation_3d<double>& R1 = c1.get_rotation();
  rel_rot = R1*(R0.transpose());
  vgl_vector_3d<double> td = rel_rot*t0;
  rel_trans = -td + t1;
}

bool vsph_camera_bounds::pixel_cylinder(vpgl_generic_camera<double> const& cam,
                                        unsigned u, unsigned v,
                                        vgl_ray_3d<double>& cylinder_axis,
                                        double& cylinder_radius)
{
  unsigned nc = cam.cols(), nr = cam.rows();
  cylinder_axis = cam.ray(u, v);
  vgl_point_3d<double> axis_origin = cylinder_axis.origin();
  vgl_ray_3d<double> corner_ray;
  if (u>0&&v>0&&u<nc&&v<nr)
    corner_ray = cam.ray(u-0.5, v-0.5);
  else if (u>0&&v==0&&u<nc)
    corner_ray = cam.ray(u-0.5, v+0.5);
  else if (u==0&&v>0&&v<nr)
    corner_ray = cam.ray(u+0.5, v-0.5);
  else if (u==0&&v==0)
    corner_ray = cam.ray(u+0.5, v+0.5);
  else {
    cylinder_radius = 0;
    return false;
  }
  vgl_point_3d<double> cp = vgl_closest_point(axis_origin, corner_ray);
  cylinder_radius = (axis_origin-cp).length();
  return true;
}

bool vsph_camera_bounds::planar_bounding_box(vpgl_perspective_camera<double> const& c,
                                             vgl_box_2d<double>& bbox,
                                             double z_plane)
{
  //principal point for image size
  vgl_point_2d<double> pp = c.get_calibration().principal_point();

  //backproject four corners of the iamge
  vgl_ray_3d<double> ul = c.backproject(0.0, 0.0);
  vgl_ray_3d<double> ur = c.backproject(2*pp.x(), 0.0);
  vgl_ray_3d<double> bl = c.backproject(0.0, 2*pp.y());
  vgl_ray_3d<double> br = c.backproject(2*pp.x(), 2*pp.y());

  //define z plane
  vgl_plane_3d<double> zp( vgl_point_3d<double>( 1.0,  1.0, z_plane),
                           vgl_point_3d<double>( 1.0, -1.0, z_plane),
                           vgl_point_3d<double>(-1.0,  1.0, z_plane) );

  //intersect each ray with z plane
  vgl_point_3d<double> ulp, urp, blp, brp;
  bool good =    vgl_intersection(ul, zp, ulp)
              && vgl_intersection(ur, zp, urp)
              && vgl_intersection(bl, zp, blp)
              && vgl_intersection(br, zp, brp);

  //add points to box
  if (good) {
    bbox.add( vgl_point_2d<double>(ulp.x(),ulp.y()) );
    bbox.add( vgl_point_2d<double>(urp.x(),urp.y()) );
    bbox.add( vgl_point_2d<double>(blp.x(),blp.y()) );
    bbox.add( vgl_point_2d<double>(brp.x(),brp.y()) );
  }
  return good;
}

bool vsph_camera_bounds::planar_bounding_box(std::vector<vpgl_perspective_camera<double> > const& cams,
                                             vgl_box_2d<double>& bbox,
                                             double z_plane)
{
  bool good = true;
  for (const auto & cam : cams) {
    vgl_box_2d<double> b;
    if ( planar_bounding_box( cam, b, z_plane ) )
      bbox.add(b);
    else
      good = false;
  }
  return good;
}
