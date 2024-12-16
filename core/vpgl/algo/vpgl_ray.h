// This is core/vpgl/algo/vpgl_ray.h
#ifndef vpgl_ray_h_
#define vpgl_ray_h_
//:
// \file
// \brief Methods for computing the camera ray direction at a given 3-d point and other operations on camera rays
// \author J. L. Mundy
// \date Dec 22, 2007

#include <vpgl/vpgl_rational_camera.h>
#include <vgl/algo/vgl_rotation_3d.h>
#include <vpgl/vpgl_local_rational_camera.h>
#include <vpgl/vpgl_perspective_camera.h>
#include <vpgl/vpgl_proj_camera.h>
#include <vpgl/vpgl_affine_camera.h>
#include <vpgl/vpgl_generic_camera.h>
#include <vnl/vnl_double_3.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_vector_3d.h>
#include <vgl/vgl_ray_3d.h>

class vpgl_ray
{
public:
  //: Generic camera interfaces (pointer for abstract class)
  // Solves using back-project so will work for any camera
  // The ray direction assumes the camera is in the positive half-space
  // of the x-y plane. This assumption is necessary since some cameras
  // don't have a natural center of projection (e.g. rational cameras).

  // === vnl interface ===

  //: compute the ray at a given 3-d point
  static bool
  ray(const vpgl_camera<double> * cam, const vnl_double_3 & point_3d, vnl_double_3 & ray);

  // === vgl interface ===

  //: compute the ray at a given 3-d point
  static bool
  ray(const vpgl_camera<double> * cam, const vgl_point_3d<double> & point_3d, vgl_vector_3d<double> & ray);

  //: vgl interface, origin_z defines an x-y plane wherein lies the ray origin
  static bool
  ray(const vpgl_camera<double> * cam,
      const vgl_point_3d<double> & point_3d,
      double origin_z,
      vgl_ray_3d<double> & ray);

  //: vgl interface, origin_z defines an x-y plane wherein lies the ray origin
  //  and dz defines a plane to compute a second intersection point for the ray direction
  // the inital guess is for the x and y values where the ray intersects the z planes
  static bool
  ray(const vpgl_camera<double> * cam,
      vgl_point_2d<double> image_pt,
      const vgl_point_2d<double> & inital_guess,
      double origin_z,
      double dz,
      vgl_ray_3d<double> & ray);

  // +++ concrete rational camera interfaces +++

  // === vnl interface ===

  //: compute the ray at a given 3-d point
  static bool
  ray(const vpgl_rational_camera<double> & rcam, const vnl_double_3 & point_3d, vnl_double_3 & ray);

  // === vgl interface ===

  //: compute the ray at a given 3-d point
  static bool
  ray(const vpgl_rational_camera<double> & rcam, const vgl_point_3d<double> & point_3d, vgl_vector_3d<double> & ray);

  //: compute the ray at a given 3-d point
  static bool
  ray(const vpgl_rational_camera<double> & rcam, const vgl_point_3d<double> & point_3d, vgl_ray_3d<double> & ray);

  //: compute a ray in local Cartesian coordinates at a given (u, v)
  static bool
  ray(const vpgl_local_rational_camera<double> & lrcam,
      const double u,
      const double v,
      vgl_point_3d<double> & origin,
      vgl_vector_3d<double> & dir);

  //: compute a ray in local Cartesian coordinates at a given (u, v)
  static bool
  ray(const vpgl_local_rational_camera<double> & lrcam, const double u, const double v, vgl_ray_3d<double> & ray);

  //: compute a ray in local Cartesian coordinates for a local rational cam
  static bool
  plane_ray(const vpgl_local_rational_camera<double> & lrcam,
            const vgl_point_2d<double> image_point1,
            const vgl_point_2d<double> image_point2,
            vgl_plane_3d<double> & plane);
  // ====== projective camera =====
  static bool
  ray(const vpgl_proj_camera<double> & cam, const vgl_point_3d<double> & world_pt, vgl_ray_3d<double> & ray);

  static bool
  principal_ray(const vpgl_proj_camera<double> & cam, vgl_ray_3d<double> & pray);

  // ====== affine camera =====
  static bool
  ray(const vpgl_affine_camera<double> & cam, const vgl_point_3d<double> & world_pt, vgl_ray_3d<double> & ray);


  // ====== perspective camera =====
  static bool
  ray(const vpgl_perspective_camera<double> & cam, const vgl_point_3d<double> & world_pt, vgl_ray_3d<double> & ray);

  static bool
  principal_ray(const vpgl_perspective_camera<double> & cam, vgl_ray_3d<double> & pray)
  {
    const vpgl_proj_camera<double> * procam = static_cast<const vpgl_proj_camera<double> *>(&cam);
    return vpgl_ray::principal_ray(*procam, pray);
  }

  // ====== generic camera =====
  static bool
  ray(const vpgl_generic_camera<double> & cam, const vgl_point_3d<double> & world_pt, vgl_ray_3d<double> & ray);


  // ====== operations on rotation matrices with respect to camera rays ======

  //: angle(radians) between principal ray of one rotation and the principal ray of a second rotation
  // Rotations \p r0 and \p r1 are expressed as vgl_rotation<T>
  static double
  angle_between_rays(const vgl_rotation_3d<double> & r0, const vgl_rotation_3d<double> & r1);

  //: the rotation about the principal ray required to go from \p r0 to \p r1
  static double
  rot_about_ray(const vgl_rotation_3d<double> & r0, const vgl_rotation_3d<double> & r1);

  //: the rotation required to point the principal ray in a given direction, starting with the identity camera
  //: (principal ray in z direction)
  static vgl_rotation_3d<double>
  rot_to_point_ray(const vgl_vector_3d<double> & ray_dir);
  //: define the principal ray in spherical coordinates (in degrees, azimuth [0 360], elevation [0, 180], x axis = (0,
  //: 90), y axis = (90, 90, z axis = (0, 0)).
  static vgl_rotation_3d<double>
  rot_to_point_ray(double azimuth, double elevation);

private:
  //: constructor/destructor private - static methods only
  vpgl_ray() = delete;
  ~vpgl_ray() = delete;
};

#endif // vpgl_ray_h_
