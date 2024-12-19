// This is core/vpgl/algo/vpgl_backproject.h
#ifndef vpgl_backproject_h_
#define vpgl_backproject_h_
//:
// \file
// \brief Methods for back_projecting from cameras to 3-d geometric structures
// \author J. L. Mundy
// \date Oct 29, 2006
//
// \verbatim
//   Modifications
//    Yi Dong  Jun-2015   added relative diameter as one argument, with default value 1.0 (same as before)
// \endverbatim

#include <vpgl/vpgl_camera.h>
#include <vpgl/vpgl_generic_camera.h>
#include <vpgl/vpgl_proj_camera.h>
#include <vgl/vgl_fwd.h>
#include <vnl/vnl_double_2.h>
#include <vnl/vnl_double_3.h>
#include <vnl/vnl_double_4.h>
#include <vgl/vgl_plane_3d.h>

class vpgl_backproject
{
public:
  ~vpgl_backproject();

  //: Generic camera interfaces (pointer for abstract class)
  // An iterative solution using forward projection
  // vnl interface

  //: Backproject an image point onto a plane, start with initial_guess
  static bool
  bproj_plane(const vpgl_camera<double> & cam,
              const vnl_double_2 & image_point,
              const vnl_double_4 & plane,
              const vnl_double_3 & initial_guess,
              vnl_double_3 & world_point,
              double error_tol = 0.05,
              double relative_diameter = 1.0);


  //: wrapper function to keep backwards-compatibility with previous API
  static bool
  bproj_plane(const vpgl_camera<double> * cam,
              const vnl_double_2 & image_point,
              const vnl_double_4 & plane,
              const vnl_double_3 & initial_guess,
              vnl_double_3 & world_point,
              double error_tol = 0.05,
              double relative_diameter = 1.0)
  {
    return bproj_plane(*cam, image_point, plane, initial_guess, world_point, error_tol, relative_diameter);
  }


  // +++ concrete camera interfaces +++

  //: Backproject an image point onto a plane, start with initial_guess
  static bool
  bproj_plane(const vpgl_generic_camera<double> & cam,
              const vnl_double_2 & image_point,
              const vnl_double_4 & plane,
              const vnl_double_3 & initial_guess,
              vnl_double_3 & world_point,
              double error_tol = 0.05,
              double relative_diameter = 1.0);

  //: Backproject an image point onto a plane, start with initial_guess
  static bool
  bproj_plane(const vpgl_proj_camera<double> & cam,
              const vnl_double_2 & image_point,
              const vnl_double_4 & plane,
              const vnl_double_3 & initial_guess,
              vnl_double_3 & world_point,
              double error_tol = 0.05,
              double relative_diameter = 1.0);


  // ==== vgl interface ===

  //: Backproject an image point onto a plane, start with initial_guess
  template <class CAM_T>
  static bool
  bproj_plane(CAM_T const & cam,
              const vgl_point_2d<double> & image_point,
              const vgl_plane_3d<double> & plane,
              const vgl_point_3d<double> & initial_guess,
              vgl_point_3d<double> & world_point,
              double error_tol = 0.05,
              double relative_diameter = 1.0);


  //: wrapper function to keep backwards compatibility with the previous API
  static bool
  bproj_plane(const vpgl_camera<double> * cam,
              const vgl_point_2d<double> & image_point,
              const vgl_plane_3d<double> & plane,
              const vgl_point_3d<double> & initial_guess,
              vgl_point_3d<double> & world_point,
              double error_tol = 0.05,
              double relative_diameter = 1.0)
  {
    return bproj_plane(*cam, image_point, plane, initial_guess, world_point, error_tol, relative_diameter);
  }

  //: Backproject a point with associated direction vector in the image to a plane in 3-d, passing through the center of
  //: projection and containing the point and vector.
  //  ** Defined only for a projective camera **
  static bool
  bproj_point_vector(const vpgl_proj_camera<double> & cam,
                     const vgl_point_2d<double> & point,
                     const vgl_vector_2d<double> & vect,
                     vgl_plane_3d<double> & plane);

  //: Use backprojection to determine direction to camera from 3-d point
  template <class CAM_T>
  static bool
  direction_to_camera(CAM_T const & cam,
                      const vgl_point_3d<double> & point,
                      vgl_vector_3d<double> & to_camera,
                      double error_tol = 0.05,
                      double relative_diameter = 1.0);

private:
  //: constructor private - static methods only
  vpgl_backproject();
};


//---  Template Definitions ---//

template <class CAM_T>
bool
vpgl_backproject::bproj_plane(CAM_T const & cam,
                              const vgl_point_2d<double> & image_point,
                              const vgl_plane_3d<double> & plane,
                              const vgl_point_3d<double> & initial_guess,
                              vgl_point_3d<double> & world_point,
                              double error_tol,
                              double relative_diameter)
{
  // simply convert to vnl interface
  vnl_double_2 ipt;
  vnl_double_3 ig, wp;
  vnl_double_4 pl;
  ipt[0] = image_point.x();
  ipt[1] = image_point.y();
  pl[0] = plane.a();
  pl[1] = plane.b();
  pl[2] = plane.c();
  pl[3] = plane.d();
  ig[0] = initial_guess.x();
  ig[1] = initial_guess.y();
  ig[2] = initial_guess.z();
  bool success = vpgl_backproject::bproj_plane(cam, ipt, pl, ig, wp, error_tol, relative_diameter);
  world_point.set(wp[0], wp[1], wp[2]);
  return success;
}

template <class CAM_T>
bool
vpgl_backproject::direction_to_camera(CAM_T const & cam,
                                      const vgl_point_3d<double> & point,
                                      vgl_vector_3d<double> & to_camera,
                                      double error_tol,
                                      double relative_diameter)
{
  // assumes that camera is above point of interest
  // project point to image, and backproject to another z-plane, vector points to sensor
  vgl_point_2d<double> img_pt = cam.project(point);
  constexpr double z_off = 10.0;
  vgl_plane_3d<double> plane_high(0, 0, 1, -(point.z() + z_off));
  vgl_point_3d<double> point_high;
  vgl_point_3d<double> guess(point.x(), point.y(), point.z() + z_off);
  if (!bproj_plane(cam, img_pt, plane_high, guess, point_high, error_tol, relative_diameter))
  {
    return false;
  }
  // this assumes camera z > point.z
  to_camera = point_high - point;
  // normalize vector
  normalize(to_camera);

  return true;
}

#endif // vpgl_backproject_h_
