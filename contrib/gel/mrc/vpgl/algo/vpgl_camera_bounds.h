// This is gel/mrc/vpgl/algo/vpgl_camera_bounds.h
#ifndef vpgl_camera_bounds_h_
#define vpgl_camera_bounds_h_
//:
// \file
// \brief Methods for computing various bounds on cameras and scenes
// \author J. L. Mundy
// \date September 18, 2010

#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_vector_3d.h>
#include <vgl/vgl_box_2d.h>
#include <vgl/vgl_box_3d.h>
#include <vpgl/vpgl_proj_camera.h>
#include <vpgl/vpgl_affine_camera.h>
#include <vpgl/vpgl_perspective_camera.h>

class vpgl_camera_bounds
{
 public:
  //: the solid angle for a pixel, applies only to perspective camera
  // cone is tangent to pixel. Angles in radians and steradians
  static void pixel_solid_angle(vpgl_perspective_camera<double> const& cam,
                                unsigned u, unsigned v,
                                vgl_ray_3d<double>& cone_axis,
                                double& cone_half_angle,
                                double& solid_angle);

  //: the solid angle for an image, applies only to perspective camera
  // the cone axis passes through principal point, i.e. principal ray.
  // the cone is tangent to the square defined by the image diagonal
  // angles in radians and steradians
  static void image_solid_angle(vpgl_perspective_camera<double> const& cam,
                                vgl_ray_3d<double>& cone_axis,
                                double& cone_half_angle,
                                double& solid_angle);

  //: the solid angle for a scene bounding box, the cone is tangent 
  // to the 3-d scene box
  // angles in radians and steradians
  static bool box_solid_angle(vpgl_perspective_camera<double> const& cam,
                              vgl_box_3d<double> const& box,
                              vgl_ray_3d<double>& cone_axis,
                              double& cone_half_angle,
                              double& solid_angle);

  //: The angular interval of the rotation about the principal axis 
  //  that bounds a pixel at the smallest image radius, i.e.,
  //  1/2 the smallest image dimension. The angular wedge is tangent to
  //  the pixel
  static double 
    rotation_angle_interval(vpgl_perspective_camera<double> const& cam);

 private:
  //: constructor private - static methods only
  vpgl_camera_bounds();
  ~vpgl_camera_bounds();
};

#endif // vpgl_camera_bounds_h_
