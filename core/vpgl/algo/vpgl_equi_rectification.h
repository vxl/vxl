// This is core/vpgl/algo/vpgl_equi_rectification.h
#ifndef vpgl_equi_rectification_h_
#define vpgl_equi_rectification_h_
//:
// \file
// \brief Rectify a pair of images by applying equal and opposite warp transforms to each image
// \author J.L. Mundy
// \date February 10, 2019
//
// \verbatim
//  Modifications
//   <none>
// \endverbatim
//
// Equi-affine rectification applies the required warp equally to each image, rather than applying
// the required row and column scaling solely to image1.  As a first step, rotations are applied
// to each image, R0 and R1 so that the epipolar lines of aF are horizontal, i.e.,parallel to image rows.
// The image row coordinates are then scaled so that rows are in correspondence. The transform is as follows.
//       _           _
//      |  1   0   0  |
//  V = |  0   sv  tv |
//      |  0   0   1  |
//       -           -
// The values of sv and tv are found by minimizing the square distance between the transformed image1 points
// and the corrsponding points in image0. That is ||V*R1[X1]- R0[X0]||^2 is minimized.
// To determine the equal and opposite transformations it is necessary to define the square root of the
// transformation matrix.
//
//                                    _                      _
//                                   |  1    0         0      |
//                                   |                tv      |
//   V = V^1/2 * V^1/2, where V^1/2 =|  0  sv^1/2  ---------- |
//                                   |            (1 + sv^1/2)|
//                                   |  0   0          1      |
//                                    -                      -
// The transformation is then equalized for each image, i.e. V^1/2 * R1  and (V^1/2)^-1 * R0.
// A similar proceedure is applied to scale the column coordinates, so that overall disparity
// is minimized.

#include <vpgl/vpgl_affine_camera.h>
#include <vpgl/vpgl_affine_fundamental_matrix.h>
#include <vpgl/vpgl_fundamental_matrix.h>
#include <vnl/vnl_vector_fixed.h>
#include <vnl/vnl_matrix_fixed.h>

class vpgl_equi_rectification
{
 public:
  //: given the affine fundamental matrix and a set of correspondences between
  // the two images, image0 and image1, pts0, pts1, determine the corresponding
  // affine homographies, H0 and H1, that warp each image into a rectified pair.
  // A rectified pair has rows in correspondence,i.e., a point in one image has
  // its corresponding point on the same row in the other image. Equivalently,
  // an epipolar line in image 1 is the same row as for a given point in image 0.
  // min_scale limits the amount of warping to avoid aliasing
  static bool rectify_pair(const vpgl_affine_fundamental_matrix<double>& aF,
                           const std::vector<vnl_vector_fixed<double, 3> >& img_pts0,
                           const std::vector<vnl_vector_fixed<double, 3> >& img_pts1,
                           vnl_matrix_fixed<double, 3, 3>& H0, vnl_matrix_fixed<double, 3, 3>& H1,
                           double min_scale = 0.1);

  //: the rectification of images with projective cameras
  static bool rectify_pair(const vpgl_fundamental_matrix<double>& F,
                           const std::vector<vnl_vector_fixed<double, 3> >& img_pts0,
                           const std::vector<vnl_vector_fixed<double, 3> >& img_pts1,
                           vnl_matrix_fixed<double, 3, 3>& H0, vnl_matrix_fixed<double, 3, 3>& H1,
                           double min_scale = 0.1);
 private:
  //: no public constructor - static methods only
  vpgl_equi_rectification() = delete;
};

#endif // vpgl_equi_rectification_h_
