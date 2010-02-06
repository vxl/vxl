// This is gel/mrc/vpgl/algo/vpgl_ray.h
#ifndef vpgl_ray_h_
#define vpgl_ray_h_
//:
// \file
// \brief Methods for computing the camera ray direction at a given 3-d point
// \author J. L. Mundy
// \date Dec 22, 2007

#include <vpgl/vpgl_rational_camera.h>
#include <vpgl/vpgl_local_rational_camera.h>
#include <vnl/vnl_double_3.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_vector_3d.h>

class vpgl_ray
{
 public:
  ~vpgl_ray();

  //: Generic camera interfaces (pointer for abstract class)
  // Solves using back-project so will work for any camera
  // The ray direction assumes the camera is in the positive half-space
  // of the x-y plane. This assumption is necessary since some cameras
  // don't have a natural center of projection (e.g. rational cameras).

       // === vnl interface ===

  //: compute the ray at a given 3-d point
  static  bool ray(const vpgl_camera<double>* cam,
                   vnl_double_3 const& point_3d,
                   vnl_double_3& ray);

       // === vgl interface ===

  //: compute the ray at a given 3-d point
  static bool ray(const vpgl_camera<double>*  cam,
                  vgl_point_3d<double> const& point_3d,
                  vgl_vector_3d<double>& ray);

            // +++ concrete rational camera interfaces +++

       // === vnl interface ===

  //: compute the ray at a given 3-d point
  static bool ray(vpgl_rational_camera<double> const& rcam,
                  vnl_double_3 const& point_3d,
                  vnl_double_3& ray);

       // === vgl interface ===

  //: compute the ray at a given 3-d point
  static bool ray(vpgl_rational_camera<double> const& rcam,
                  vgl_point_3d<double> const& point_3d,
                  vgl_vector_3d<double>& ray);

  //: compute a ray in local Cartesian coordinates at a given (u, v)
  static bool ray(vpgl_local_rational_camera<double> const& lrcam,
                  const double u, const double v, 
                  vgl_point_3d<double>& origin, vgl_vector_3d<double>& dir);

// compute a ray in local Cartesian coordinates for a local rational cam
  static bool plane_ray(vpgl_local_rational_camera<double> const& lrcam,
                        const vgl_point_2d<double> image_point1,
                        const vgl_point_2d<double> image_point2,
                        vgl_plane_3d<double>& plane);

 private:
  //: constructor private - static methods only
  vpgl_ray();
};

#endif // vpgl_ray_h_
