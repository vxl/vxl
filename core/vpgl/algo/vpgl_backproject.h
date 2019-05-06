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

#include <vpgl/vpgl_rational_camera.h>
#include <vpgl/vpgl_local_rational_camera.h>
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

  //:Backproject an image point onto a plane, start with initial_guess
  static bool bproj_plane(vpgl_camera<double> const& cam,
                          vnl_double_2 const& image_point,
                          vnl_double_4 const& plane,
                          vnl_double_3 const& initial_guess,
                          vnl_double_3& world_point,
                          double error_tol = 0.05,
                          double relative_diameter = 1.0);


            // +++ concrete camera interfaces +++

  //:Backproject an image point onto a plane, start with initial_guess
  static bool bproj_plane(vpgl_generic_camera<double> const& cam,
                          vnl_double_2 const& image_point,
                          vnl_double_4 const& plane,
                          vnl_double_3 const& initial_guess,
                          vnl_double_3& world_point,
                          double error_tol = 0.05,
                          double relative_diameter = 1.0);

  //:Backproject an image point onto a plane, start with initial_guess
  static bool bproj_plane(vpgl_proj_camera<double> const& cam,
                          vnl_double_2 const& image_point,
                          vnl_double_4 const& plane,
                          vnl_double_3 const& initial_guess,
                          vnl_double_3& world_point,
                          double error_tol = 0.05,
                          double relative_diameter = 1.0);


       // ==== vgl interface ===

  //: Backproject an image point onto a plane, start with initial_guess
  template <class CAM_T>
  static bool bproj_plane(CAM_T const& cam,
                          vgl_point_2d<double> const& image_point,
                          vgl_plane_3d<double> const& plane,
                          vgl_point_3d<double> const& initial_guess,
                          vgl_point_3d<double>& world_point,
                          double error_tol = 0.05,
                          double relative_diameter = 1.0);

  //:Backproject a point with associated direction vector in the image to a plane in 3-d, passing through the center of projection and containing the point and vector.
  //  ** Defined only for a projective camera **
  static bool bproj_point_vector(vpgl_proj_camera<double> const& cam,
                                 vgl_point_2d<double> const& point,
                                 vgl_vector_2d<double> const& vect,
                                 vgl_plane_3d<double>& plane);

  //: Use backprojection to determine direction to camera from 3-d point
  template <class CAM_T>
  static bool direction_to_camera(CAM_T const& cam,
                                  vgl_point_3d<double> const& point,
                                  vgl_vector_3d<double> &to_camera,
                                  double error_tol = 0.05,
                                  double relative_diameter = 1.0);

 private:
  //: constructor private - static methods only
  vpgl_backproject();
};



//---  Template Definitions ---//

template <class CAM_T>
bool vpgl_backproject::bproj_plane(CAM_T const& cam,
                                   vgl_point_2d<double> const& image_point,
                                   vgl_plane_3d<double> const& plane,
                                   vgl_point_3d<double> const& initial_guess,
                                   vgl_point_3d<double>& world_point,
                                   double error_tol,
                                   double relative_diameter)
{
  //simply convert to vnl interface
  vnl_double_2 ipt;
  vnl_double_3 ig, wp;
  vnl_double_4 pl;
  ipt[0]=image_point.x(); ipt[1]=image_point.y();
  pl[0]=plane.a(); pl[1]=plane.b(); pl[2]=plane.c(); pl[3]=plane.d();
  ig[0]=initial_guess.x();  ig[1]=initial_guess.y();  ig[2]=initial_guess.z();
  bool success = vpgl_backproject::bproj_plane(cam, ipt, pl, ig, wp, error_tol, relative_diameter);
  world_point.set(wp[0], wp[1], wp[2]);
  return success;
}

template<class CAM_T>
bool vpgl_backproject::direction_to_camera(CAM_T const& cam,
                                           vgl_point_3d<double> const& point,
                                           vgl_vector_3d<double> &to_camera,
                                           double error_tol,
                                           double relative_diameter)
{
  // assumes that camera is above point of interest
  // project point to image, and backproject to another z-plane, vector points to sensor
  vgl_point_2d<double> img_pt = cam.project(point);
  constexpr double z_off = 10.0;
  vgl_plane_3d<double> plane_high(0,0,1,-(point.z()+z_off));
  vgl_point_3d<double> point_high;
  vgl_point_3d<double> guess(point.x(),point.y(),point.z() + z_off);
  if (!bproj_plane(cam, img_pt, plane_high, guess, point_high, error_tol, relative_diameter)) {
    return false;
  }
  // this assumes camera z > point.z
  to_camera = point_high - point;
  // normalize vector
  normalize(to_camera);

  return true;
}

#endif // vpgl_backproject_h_
