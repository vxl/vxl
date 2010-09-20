#include "vpgl_camera_bounds.h"
#include <vpgl/algo/vpgl_project.h>
#include <vpgl/algo/vpgl_ray.h>
#include <vpgl/vpgl_calibration_matrix.h>
#include <vgl/algo/vgl_rotation_3d.h>
#include <vcl_cmath.h>
#include <vnl/vnl_math.h>

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
