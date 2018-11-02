// This is core/vpgl/algo/vpgl_calibration_matrix_compute.h
#ifndef vpgl_calibration_matrix_compute_h_
#define vpgl_calibration_matrix_compute_h_

//:
// \file
// \brief Computes a camera calibration matrix from points on a 3D world plane.
// \author Peter Carr
// \date 14 Feb 2013

#include <vector>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vgl/vgl_point_2d.h>
#include <vgl/algo/vgl_h_matrix_2d.h>
#include <vpgl/vpgl_calibration_matrix.h>

//:Various methods for computing the intrinsic matrix of a perspective camera
class vpgl_calibration_matrix_compute
{
  public:
  //: Computes the calibration matrix for a natural camera (zero skew, square pixels)
  // from a ground plane to image plane homography. A value for the
  // principal point is needed for the computation.
  // \param homography represents the mapping from the ground plane to the image plane
  // \param principal_point is the optical centre of the image
  // \param estimated calibration matrix with square pixels and zero skew
  // \return true if successful.

  static
  bool
  natural(const vgl_h_matrix_2d<double>& homography,
          const vgl_point_2d<double>& principal_point,
          vpgl_calibration_matrix<double>& K);

  static
  bool
  natural(const std::vector< vgl_point_2d<double> >& image_pts,
          const std::vector< vgl_point_2d<double> >& ground_pts,
          const vgl_point_2d<double>& principal_point,
          vpgl_calibration_matrix<double>& K);
};

#endif // vpgl_calibration_matrix_compute_h_
