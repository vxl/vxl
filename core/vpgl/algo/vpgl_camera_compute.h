// This is core/vpgl/algo/vpgl_camera_compute.h
#ifndef vpgl_camera_compute_h_
#define vpgl_camera_compute_h_
//:
// \file
// \brief Several routines for computing different kinds of cameras from world-point correspondences.
// \author Thomas Pollard
// \date July 18, 2005
//
// Should template this class.

#include <vector>
#include <vpgl/vpgl_perspective_camera.h>
#include <vpgl/vpgl_proj_camera.h>
#include <vpgl/vpgl_affine_camera.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vgl/vgl_fwd.h>
#include <vnl/vnl_fwd.h>

//: Basic least squares solution for a general projective camera given corresponding world and image points.
class vpgl_proj_camera_compute
{
 public:
  //: Compute from two sets of corresponding points.
  // Put the resulting camera into \p camera
  // \return true if successful.
  static bool compute( const std::vector< vgl_homg_point_2d<double> >& image_pts,
                       const std::vector< vgl_homg_point_3d<double> >& world_pts,
                       vpgl_proj_camera<double>& camera );
  //: Compute from two sets of corresponding points.
  // Put the resulting camera into \p camera
  // \return true if successful.
  static bool compute( const std::vector< vgl_point_2d<double> >& image_pts,
                       const std::vector< vgl_point_3d<double> >& world_pts,
                       vpgl_proj_camera<double>& camera );
 private:
  //:default constructor (is private)
  vpgl_proj_camera_compute() = delete;
};


//: Basic least squares solution for an affine camera given corresponding world and image points.
class vpgl_affine_camera_compute
{
 public:
  //: Compute from two sets of corresponding points.
  // Put the resulting camera into \p camera
  // \return true if successful.
  static bool compute( const std::vector< vgl_point_2d<double> >& image_pts,
                       const std::vector< vgl_point_3d<double> >& world_pts,
                       vpgl_affine_camera<double>& camera );
 private:
  vpgl_affine_camera_compute() = delete;
};


//:Various methods for computing a perspective camera
class vpgl_perspective_camera_compute
{
 public:
  //: Compute from two sets of corresponding points.
  // Put the resulting camera into \p camera
  // \return true if successful.
  static bool compute( const std::vector< vgl_point_2d<double> >& image_pts,
                       const std::vector< vgl_point_3d<double> >& world_pts,
                       const vpgl_calibration_matrix<double>& K,
                       vpgl_perspective_camera<double>& camera );


  //: Uses the direct linear transform algorithm described in "Multiple
  // View Geometry in Computer Vision" to find the projection matrix,
  // and extracts the parameters of the camera from this projection matrix.
  // Requires: \p image_pts and \p world_pts are correspondences.
  //  \p image_pts is the projected form, and \p world_pts is the unprojected form.
  //  There need to be at least 6 points.
  // \returns true if successful.
  // \p err is filled with the two-norm of the projection error vector.
  // \p camera is filled with the perspective decomposition of the projection matrix.
  static bool compute_dlt ( const std::vector< vgl_point_2d<double> >& image_pts,
                            const std::vector< vgl_point_3d<double> >& world_pts,
                            vpgl_perspective_camera<double> &camera,
                            double &err);

  //: Compute from two sets of corresponding 2D points (image and ground plane).
  // \param ground_pts are 2D points representing world points with Z=0
  // The calibration matrix of \p camera is enforced
  // This computation is simpler than the general case above and only requires 4 points
  // Put the resulting camera into \p camera
  // \return true if successful.
  static bool compute( const std::vector< vgl_point_2d<double> >& image_pts,
                       const std::vector< vgl_point_2d<double> >& ground_pts,
                       vpgl_perspective_camera<double>& camera );

 private:
  vpgl_perspective_camera_compute() = delete;
};

#endif // vpgl_camera_compute_h_
