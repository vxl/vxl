// This is core/vpgl/algo/vpgl_backproject.h
#ifndef vpgl_backproject_h_
#define vpgl_backproject_h_
// :
// \file
// \brief Methods for back_projecting from cameras to 3-d geometric structures
// \author J. L. Mundy
// \date Oct 29, 2006
//
// \verbatim
//   Modifications
//    Yi Dong  Jun-2015   added relative diameter as one argument, with default value 1.0 (same as before)
// \endverbatim

#include <vpgl/vpgl_rational_camera.h>
#include <vpgl/vpgl_local_rational_camera.h>
#include <vpgl/vpgl_proj_camera.h>
#include <vgl/vgl_fwd.h>
#include <vnl/vnl_double_2.h>
#include <vnl/vnl_double_3.h>
#include <vnl/vnl_double_4.h>

class vpgl_backproject
{
public: ~vpgl_backproject();

  // : Generic camera interfaces (pointer for abstract class)
  // An iterative solution using forward projection
  // vnl interface

  // :Backproject an image point onto a plane, start with initial_guess
  static bool bproj_plane(const vpgl_camera<double>* cam, vnl_double_2 const& image_point, vnl_double_4 const& plane,
                          vnl_double_3 const& initial_guess, vnl_double_3& world_point, double error_tol = 0.05,
                          double relative_diameter = 1.0);

  // === vgl interface ===

  // :Backproject an image point onto a plane, start with initial_guess
  static bool bproj_plane(const vpgl_camera<double>*  cam, vgl_point_2d<double> const& image_point,
                          vgl_plane_3d<double> const& plane, vgl_point_3d<double> const& initial_guess,
                          vgl_point_3d<double>& world_point, double error_tol = 0.05, double relative_diameter = 1.0);

  // +++ concrete rational camera interfaces +++

  // === vnl interface ===

  // :Backproject an image point onto a plane, start with initial_guess
  static bool bproj_plane(vpgl_rational_camera<double> const& rcam, vnl_double_2 const& image_point,
                          vnl_double_4 const& plane, vnl_double_3 const& initial_guess, vnl_double_3& world_point,
                          double error_tol = 0.05, double relative_diameter = 1.0);

  // ==== vgl interface ===

  // :Backproject an image point onto a plane, start with initial_guess
  static bool bproj_plane(vpgl_rational_camera<double> const& rcam, vgl_point_2d<double> const& image_point,
                          vgl_plane_3d<double> const& plane, vgl_point_3d<double> const& initial_guess,
                          vgl_point_3d<double>& world_point, double error_tol = 0.05, double relative_diameter = 1.0);

  // :Backproject a point with associated direction vector in the image to a plane in 3-d, passing through the center of projection and containing the point and vector.
  //  ** Defined only for a projective camera **
  static bool bproj_point_vector(vpgl_proj_camera<double> const& cam, vgl_point_2d<double> const& point,
                                 vgl_vector_2d<double> const& vect, vgl_plane_3d<double>& plane);

  // : Use backprojection to determine direction to camera from 3-d point
  static bool direction_to_camera(vpgl_local_rational_camera<double> const& cam, vgl_point_3d<double> const& point,
                                  vgl_vector_3d<double> & to_camera, double error_tol = 0.05,
                                  double relative_diameter = 1.0);

private:
  // : constructor private - static methods only
  vpgl_backproject();
};

#endif // vpgl_backproject_h_
